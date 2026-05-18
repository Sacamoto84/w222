#include "ftp_server.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

#define FTP_LINE_MAX 256
#define FTP_PATH_MAX 512
#define FTP_IO_BUF_SIZE 1024

typedef struct {
    ftp_server_config_t config;
    TaskHandle_t task;
    uint16_t next_passive_port;
} ftp_server_t;

typedef struct {
    ftp_server_t *server;
    int control_fd;
    int passive_fd;
    char cwd[FTP_PATH_MAX];
    bool logged_in;
    char rename_from[FTP_PATH_MAX];
} ftp_client_t;

static const char *TAG = "ftp";
static ftp_server_t s_ftp;

static size_t ftp_strlcpy(char *dst, const char *src, size_t dst_size)
{
    size_t src_len = strlen(src);
    if (dst_size > 0) {
        size_t copy_len = src_len >= dst_size ? dst_size - 1 : src_len;
        memcpy(dst, src, copy_len);
        dst[copy_len] = '\0';
    }
    return src_len;
}

static size_t ftp_strlcat(char *dst, const char *src, size_t dst_size)
{
    size_t dst_len = strnlen(dst, dst_size);
    size_t src_len = strlen(src);
    if (dst_len < dst_size) {
        size_t copy_len = src_len >= (dst_size - dst_len) ? (dst_size - dst_len - 1) : src_len;
        memcpy(dst + dst_len, src, copy_len);
        dst[dst_len + copy_len] = '\0';
    }
    return dst_len + src_len;
}

static int ftp_sendf(int fd, const char *fmt, ...)
{
    char line[FTP_LINE_MAX];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(line, sizeof(line), fmt, args);
    va_end(args);
    if (len < 0) {
        return len;
    }
    if (len >= (int)sizeof(line)) {
        len = sizeof(line) - 1;
    }
    return send(fd, line, len, 0);
}

static int ftp_readline(int fd, char *line, size_t size)
{
    size_t pos = 0;
    while (pos + 1 < size) {
        char ch;
        int ret = recv(fd, &ch, 1, 0);
        if (ret <= 0) {
            return ret;
        }
        if (ch == '\n') {
            break;
        }
        if (ch != '\r') {
            line[pos++] = ch;
        }
    }
    line[pos] = '\0';
    return (int)pos;
}

static void normalize_virtual_path(const char *cwd, const char *arg, char *out, size_t out_size)
{
    char tmp[FTP_PATH_MAX];
    if (arg == NULL || arg[0] == '\0') {
        snprintf(tmp, sizeof(tmp), "%s", cwd);
    } else if (arg[0] == '/') {
        snprintf(tmp, sizeof(tmp), "%s", arg);
    } else if (strcmp(cwd, "/") == 0) {
        snprintf(tmp, sizeof(tmp), "/%s", arg);
    } else {
        snprintf(tmp, sizeof(tmp), "%s/%s", cwd, arg);
    }

    char result[FTP_PATH_MAX] = "/";
    char *saveptr = NULL;
    char *token = strtok_r(tmp, "/", &saveptr);
    while (token != NULL) {
        if (strcmp(token, ".") == 0 || token[0] == '\0') {
        } else if (strcmp(token, "..") == 0) {
            if (strcmp(result, "/") != 0) {
                char *slash = strrchr(result, '/');
                if (slash == result) {
                    result[1] = '\0';
                } else if (slash != NULL) {
                    *slash = '\0';
                }
            }
        } else {
            if (strcmp(result, "/") != 0) {
                ftp_strlcat(result, "/", sizeof(result));
            }
            ftp_strlcat(result, token, sizeof(result));
        }
        token = strtok_r(NULL, "/", &saveptr);
    }

    ftp_strlcpy(out, result, out_size);
}

static void virtual_to_fs_path(const ftp_client_t *client, const char *virtual_path, char *out, size_t out_size)
{
    if (strcmp(virtual_path, "/") == 0) {
        snprintf(out, out_size, "%s", client->server->config.root_path);
    } else {
        snprintf(out, out_size, "%s%s", client->server->config.root_path, virtual_path);
    }
}

static const char *path_basename(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

static bool is_dir_path(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int create_passive_listener(ftp_client_t *client, bool extended)
{
    if (client->passive_fd >= 0) {
        close(client->passive_fd);
        client->passive_fd = -1;
    }

    ftp_server_t *server = client->server;
    for (uint16_t tries = 0; tries <= (server->config.passive_port_max - server->config.passive_port_min); tries++) {
        uint16_t port = server->next_passive_port++;
        if (server->next_passive_port > server->config.passive_port_max) {
            server->next_passive_port = server->config.passive_port_min;
        }

        int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (fd < 0) {
            return -1;
        }

        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr.s_addr = htonl(INADDR_ANY),
        };

        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0 && listen(fd, 1) == 0) {
            client->passive_fd = fd;

            if (extended) {
                ftp_sendf(client->control_fd, "229 Entering Extended Passive Mode (|||%u|)\r\n", port);
            } else {
                struct sockaddr_in local_addr;
                socklen_t len = sizeof(local_addr);
                getsockname(client->control_fd, (struct sockaddr *)&local_addr, &len);
                uint32_t ip = ntohl(local_addr.sin_addr.s_addr);
                ftp_sendf(client->control_fd,
                          "227 Entering Passive Mode (%lu,%lu,%lu,%lu,%u,%u)\r\n",
                          (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff,
                          port / 256, port % 256);
            }
            return fd;
        }

        close(fd);
    }

    return -1;
}

static int accept_data_connection(ftp_client_t *client)
{
    if (client->passive_fd < 0) {
        ftp_sendf(client->control_fd, "425 Use PASV first.\r\n");
        return -1;
    }

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(client->passive_fd, &rfds);
    struct timeval timeout = {
        .tv_sec = 15,
        .tv_usec = 0,
    };

    int ready = select(client->passive_fd + 1, &rfds, NULL, NULL, &timeout);
    if (ready <= 0) {
        close(client->passive_fd);
        client->passive_fd = -1;
        ftp_sendf(client->control_fd, "425 Data connection timed out.\r\n");
        return -1;
    }

    int data_fd = accept(client->passive_fd, NULL, NULL);
    close(client->passive_fd);
    client->passive_fd = -1;
    if (data_fd < 0) {
        ftp_sendf(client->control_fd, "425 Can't open data connection.\r\n");
    }
    return data_fd;
}

static void send_list_line(int data_fd, const char *full_path, const char *name)
{
    struct stat st;
    if (stat(full_path, &st) != 0) {
        return;
    }

    char line[FTP_LINE_MAX];
    snprintf(line, sizeof(line), "%crw-r--r-- 1 esp32 esp32 %ld Jan 01 00:00 %s\r\n",
             S_ISDIR(st.st_mode) ? 'd' : '-', (long)st.st_size, name);
    send(data_fd, line, strlen(line), 0);
}

static void handle_list(ftp_client_t *client, const char *arg, bool names_only)
{
    char virtual_path[FTP_PATH_MAX];
    char fs_path[FTP_PATH_MAX];
    normalize_virtual_path(client->cwd, arg, virtual_path, sizeof(virtual_path));
    virtual_to_fs_path(client, virtual_path, fs_path, sizeof(fs_path));

    ftp_sendf(client->control_fd, "150 Opening data connection.\r\n");
    int data_fd = accept_data_connection(client);
    if (data_fd < 0) {
        return;
    }

    if (is_dir_path(fs_path)) {
        DIR *dir = opendir(fs_path);
        if (dir != NULL) {
            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                if (names_only) {
                    char line[FTP_LINE_MAX];
                    snprintf(line, sizeof(line), "%s\r\n", entry->d_name);
                    send(data_fd, line, strlen(line), 0);
                } else {
                    char child[FTP_PATH_MAX];
                    snprintf(child, sizeof(child), "%s/%s", fs_path, entry->d_name);
                    send_list_line(data_fd, child, entry->d_name);
                }
            }
            closedir(dir);
        }
    } else {
        if (names_only) {
            char line[FTP_LINE_MAX];
            snprintf(line, sizeof(line), "%s\r\n", path_basename(fs_path));
            send(data_fd, line, strlen(line), 0);
        } else {
            send_list_line(data_fd, fs_path, path_basename(fs_path));
        }
    }

    close(data_fd);
    ftp_sendf(client->control_fd, "226 Transfer complete.\r\n");
}

static void handle_retr(ftp_client_t *client, const char *arg)
{
    char virtual_path[FTP_PATH_MAX];
    char fs_path[FTP_PATH_MAX];
    normalize_virtual_path(client->cwd, arg, virtual_path, sizeof(virtual_path));
    virtual_to_fs_path(client, virtual_path, fs_path, sizeof(fs_path));

    FILE *file = fopen(fs_path, "rb");
    if (file == NULL) {
        ftp_sendf(client->control_fd, "550 File not found.\r\n");
        return;
    }

    ftp_sendf(client->control_fd, "150 Opening data connection.\r\n");
    int data_fd = accept_data_connection(client);
    if (data_fd < 0) {
        fclose(file);
        return;
    }

    char *buf = malloc(FTP_IO_BUF_SIZE);
    if (buf != NULL) {
        size_t read_len;
        while ((read_len = fread(buf, 1, FTP_IO_BUF_SIZE, file)) > 0) {
            if (send(data_fd, buf, read_len, 0) < 0) {
                break;
            }
        }
        free(buf);
    }
    fclose(file);
    close(data_fd);
    ftp_sendf(client->control_fd, "226 Transfer complete.\r\n");
}

static void handle_stor(ftp_client_t *client, const char *arg, const char *mode)
{
    char virtual_path[FTP_PATH_MAX];
    char fs_path[FTP_PATH_MAX];
    normalize_virtual_path(client->cwd, arg, virtual_path, sizeof(virtual_path));
    virtual_to_fs_path(client, virtual_path, fs_path, sizeof(fs_path));

    FILE *file = fopen(fs_path, mode);
    if (file == NULL) {
        ESP_LOGW(TAG, "can't open %s for upload: errno=%d", fs_path, errno);
        ftp_sendf(client->control_fd, "550 Can't create file.\r\n");
        return;
    }

    ftp_sendf(client->control_fd, "150 Opening data connection.\r\n");
    int data_fd = accept_data_connection(client);
    if (data_fd < 0) {
        fclose(file);
        return;
    }

    char *buf = malloc(FTP_IO_BUF_SIZE);
    bool write_ok = buf != NULL;
    if (buf != NULL) {
        int len;
        while ((len = recv(data_fd, buf, FTP_IO_BUF_SIZE, 0)) > 0) {
            if (fwrite(buf, 1, len, file) != (size_t)len) {
                write_ok = false;
                ESP_LOGW(TAG, "write failed for %s: errno=%d", fs_path, errno);
                break;
            }
        }
        free(buf);
    }
    fflush(file);
    fclose(file);
    close(data_fd);
    ftp_sendf(client->control_fd, write_ok ? "226 Transfer complete.\r\n" : "451 Write failed.\r\n");
}

static void handle_client(ftp_server_t *server, int client_fd)
{
    ftp_client_t client = {
        .server = server,
        .control_fd = client_fd,
        .passive_fd = -1,
        .cwd = "/",
    };

    ftp_sendf(client_fd, "220 ESP32 FTP server ready.\r\n");

    char line[FTP_LINE_MAX];
    while (ftp_readline(client_fd, line, sizeof(line)) > 0) {
        char *arg = strchr(line, ' ');
        if (arg != NULL) {
            *arg++ = '\0';
            while (*arg == ' ') {
                arg++;
            }
        } else {
            arg = "";
        }

        for (char *p = line; *p; p++) {
            *p = (char)toupper((unsigned char)*p);
        }

        ESP_LOGD(TAG, "cmd=%s arg=%s", line, arg);

        if (strcmp(line, "USER") == 0) {
            ftp_sendf(client_fd, "331 Password required.\r\n");
        } else if (strcmp(line, "PASS") == 0) {
            client.logged_in = strcmp(arg, server->config.password) == 0;
            ftp_sendf(client_fd, client.logged_in ? "230 Logged in.\r\n" : "530 Login incorrect.\r\n");
        } else if (!client.logged_in) {
            ftp_sendf(client_fd, "530 Please login with USER and PASS.\r\n");
        } else if (strcmp(line, "SYST") == 0) {
            ftp_sendf(client_fd, "215 UNIX Type: L8\r\n");
        } else if (strcmp(line, "FEAT") == 0) {
            ftp_sendf(client_fd, "211-Features\r\n EPSV\r\n PASV\r\n SIZE\r\n UTF8\r\n211 End\r\n");
        } else if (strcmp(line, "PWD") == 0 || strcmp(line, "XPWD") == 0) {
            ftp_sendf(client_fd, "257 \"%s\"\r\n", client.cwd);
        } else if (strcmp(line, "CWD") == 0) {
            char virtual_path[FTP_PATH_MAX];
            char fs_path[FTP_PATH_MAX];
            normalize_virtual_path(client.cwd, arg, virtual_path, sizeof(virtual_path));
            virtual_to_fs_path(&client, virtual_path, fs_path, sizeof(fs_path));
            if (is_dir_path(fs_path)) {
                ftp_strlcpy(client.cwd, virtual_path, sizeof(client.cwd));
                ftp_sendf(client_fd, "250 Directory changed.\r\n");
            } else {
                ftp_sendf(client_fd, "550 Not a directory.\r\n");
            }
        } else if (strcmp(line, "CDUP") == 0) {
            normalize_virtual_path(client.cwd, "..", client.cwd, sizeof(client.cwd));
            ftp_sendf(client_fd, "250 Directory changed.\r\n");
        } else if (strcmp(line, "TYPE") == 0 || strcmp(line, "MODE") == 0 || strcmp(line, "STRU") == 0) {
            ftp_sendf(client_fd, "200 OK.\r\n");
        } else if (strcmp(line, "OPTS") == 0) {
            ftp_sendf(client_fd, "200 OK.\r\n");
        } else if (strcmp(line, "ALLO") == 0 || strcmp(line, "REST") == 0) {
            ftp_sendf(client_fd, "202 Command not needed.\r\n");
        } else if (strcmp(line, "PASV") == 0) {
            if (create_passive_listener(&client, false) < 0) {
                ftp_sendf(client_fd, "425 Can't enter passive mode.\r\n");
            }
        } else if (strcmp(line, "EPSV") == 0) {
            if (create_passive_listener(&client, true) < 0) {
                ftp_sendf(client_fd, "425 Can't enter passive mode.\r\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            handle_list(&client, arg, false);
        } else if (strcmp(line, "NLST") == 0) {
            handle_list(&client, arg, true);
        } else if (strcmp(line, "RETR") == 0) {
            handle_retr(&client, arg);
        } else if (strcmp(line, "STOR") == 0) {
            handle_stor(&client, arg, "wb");
        } else if (strcmp(line, "APPE") == 0) {
            handle_stor(&client, arg, "ab");
        } else if (strcmp(line, "SIZE") == 0) {
            char virtual_path[FTP_PATH_MAX];
            char fs_path[FTP_PATH_MAX];
            struct stat st;
            normalize_virtual_path(client.cwd, arg, virtual_path, sizeof(virtual_path));
            virtual_to_fs_path(&client, virtual_path, fs_path, sizeof(fs_path));
            if (stat(fs_path, &st) == 0) {
                ftp_sendf(client_fd, "213 %ld\r\n", (long)st.st_size);
            } else {
                ftp_sendf(client_fd, "550 File not found.\r\n");
            }
        } else if (strcmp(line, "DELE") == 0) {
            char virtual_path[FTP_PATH_MAX];
            char fs_path[FTP_PATH_MAX];
            normalize_virtual_path(client.cwd, arg, virtual_path, sizeof(virtual_path));
            virtual_to_fs_path(&client, virtual_path, fs_path, sizeof(fs_path));
            ftp_sendf(client_fd, unlink(fs_path) == 0 ? "250 Deleted.\r\n" : "550 Delete failed.\r\n");
        } else if (strcmp(line, "MKD") == 0 || strcmp(line, "XMKD") == 0) {
            char virtual_path[FTP_PATH_MAX];
            char fs_path[FTP_PATH_MAX];
            normalize_virtual_path(client.cwd, arg, virtual_path, sizeof(virtual_path));
            virtual_to_fs_path(&client, virtual_path, fs_path, sizeof(fs_path));
            ftp_sendf(client_fd, mkdir(fs_path, 0775) == 0 ? "257 Directory created.\r\n" : "550 MKD failed.\r\n");
        } else if (strcmp(line, "RMD") == 0 || strcmp(line, "XRMD") == 0) {
            char virtual_path[FTP_PATH_MAX];
            char fs_path[FTP_PATH_MAX];
            normalize_virtual_path(client.cwd, arg, virtual_path, sizeof(virtual_path));
            virtual_to_fs_path(&client, virtual_path, fs_path, sizeof(fs_path));
            ftp_sendf(client_fd, rmdir(fs_path) == 0 ? "250 Removed.\r\n" : "550 RMD failed.\r\n");
        } else if (strcmp(line, "RNFR") == 0) {
            normalize_virtual_path(client.cwd, arg, client.rename_from, sizeof(client.rename_from));
            ftp_sendf(client_fd, "350 Ready for RNTO.\r\n");
        } else if (strcmp(line, "RNTO") == 0) {
            char to_virtual[FTP_PATH_MAX];
            char from_fs[FTP_PATH_MAX];
            char to_fs[FTP_PATH_MAX];
            normalize_virtual_path(client.cwd, arg, to_virtual, sizeof(to_virtual));
            virtual_to_fs_path(&client, client.rename_from, from_fs, sizeof(from_fs));
            virtual_to_fs_path(&client, to_virtual, to_fs, sizeof(to_fs));
            ftp_sendf(client_fd, rename(from_fs, to_fs) == 0 ? "250 Renamed.\r\n" : "550 Rename failed.\r\n");
        } else if (strcmp(line, "NOOP") == 0) {
            ftp_sendf(client_fd, "200 OK.\r\n");
        } else if (strcmp(line, "QUIT") == 0) {
            ftp_sendf(client_fd, "221 Bye.\r\n");
            break;
        } else {
            ftp_sendf(client_fd, "502 Command not implemented.\r\n");
        }
    }

    if (client.passive_fd >= 0) {
        close(client.passive_fd);
    }
}

static void ftp_server_task(void *arg)
{
    ftp_server_t *server = (ftp_server_t *)arg;
    int listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_fd < 0) {
        ESP_LOGE(TAG, "socket failed: errno=%d", errno);
        vTaskDelete(NULL);
        return;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(server->config.control_port),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(listen_fd, 1) != 0) {
        ESP_LOGE(TAG, "bind/listen failed: errno=%d", errno);
        close(listen_fd);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "FTP server started on port %u, root=%s",
             server->config.control_port, server->config.root_path);

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            ESP_LOGW(TAG, "accept failed: errno=%d", errno);
            continue;
        }

        ESP_LOGI(TAG, "client connected: %s", inet_ntoa(client_addr.sin_addr));
        handle_client(server, client_fd);
        close(client_fd);
        ESP_LOGI(TAG, "client disconnected");
    }
}

esp_err_t ftp_server_start(const ftp_server_config_t *config)
{
    if (s_ftp.task != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (config == NULL || config->root_path == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    s_ftp.config = *config;
    s_ftp.next_passive_port = config->passive_port_min;

    BaseType_t ok = xTaskCreate(
        ftp_server_task,
        "ftp_server",
        config->task_stack,
        &s_ftp,
        config->task_priority,
        &s_ftp.task);

    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
