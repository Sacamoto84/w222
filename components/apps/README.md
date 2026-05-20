# App Structure

Use this layout for new apps under `components/apps` so they can be shown by the lightweight LVGL launcher.

## Required shape

- Create one folder per app: `components/apps/<app_name>/`
- Keep the app entrypoints in that folder:
  - `<AppName>.hpp`
  - `<AppName>.cpp`
- Derive the app class from `LiteApp`
- Implement the lightweight lifecycle used by the launcher:
  - `init()`
  - `open(lv_obj_t *parent)`
  - `back()`
  - `close()`

## Recommended folders

- `assets/` for launcher icons and app-local image assets
- `ui/` or `app_gui/` only if the app has generated or separated UI code

## Wiring rules

- Add the app header include to `components/apps/apps.h`
- Instantiate the app and call `launcher.add_app()` in `main/main.cpp`
- Keep app-specific storage paths inside the app instead of hardcoding behavior in `main/main.cpp`
- Fail gracefully when optional resources are missing so boot continues

## UI rules

- Create one root object under the `parent` passed to `open()`
- Use `back()` only for app-local back behavior. Return `false` to let the launcher close the app
- Prefer app-local helper functions over global state unless the app already follows a different established pattern

## Storage rules

- Treat `/sdcard` as optional and check availability before using it
- Treat `BSP_SPIFFS_MOUNT_POINT` as always-on app storage when mounted by startup
- Avoid `ESP_ERROR_CHECK` for optional content paths that can be absent on user devices

## Build rules

- Keep new sources under `components/apps/<app_name>/`; the app component already auto-collects sources recursively
- App `assets/` folders are not auto-compiled by default. Add heavy assets explicitly when an app really needs them
- Reuse the existing ESP-IDF build workflow and verify with a full project build after adding the app
