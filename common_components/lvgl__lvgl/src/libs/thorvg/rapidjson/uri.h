// Tencent рада поддержать сообщество открытого исходного кода, созданного доступным RapidJSON.
//
// (C) Copyright IBM Corporation 2021
//
// Лицензия MIT («Лицензия»); вы не можете использовать этот файл, за исключением
// в соответствии с Лицензией. Вы можете получить копию Лицензии по адресу
//
// http://opensource.org/licenses/MIT
//
// Если это не требуется действующим законодательством или не согласовано в письменной форме, распространяемое программное обеспечение
// по Лицензии распространяется на " AS IS " BASIS , WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND , явный или подразумеваемый. См. Лицензию на
// конкретный язык, регулирующий разрешения и ограничения по Лицензии.

#ifndef RAPIDJSON_URI_H_
#define RAPIDJSON_URI_H_

#include "internal/strfunc.h"

#if defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(c++98-compat)
#elif defined(_MSC_VER)
RAPIDJSON_DIAG_OFF(4512) // не удалось сгенерировать оператор присваивания
#endif

RAPIDJSON_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// ОбщийUri

template <typename ValueType, typename Allocator=CrtAllocator>
class GenericUri {
public:
    typedef typename ValueType::Ch Ch;
#if RAPIDJSON_HAS_STDSTRING
    typedef std::basic_string<Ch> String;
#endif

    //! Конструкторы
    GenericUri(Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
    }

    GenericUri(const Ch* uri, SizeType len, Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        Parse(uri, len);
    }

    GenericUri(const Ch* uri, Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        Parse(uri, internal::StrLen<Ch>(uri));
    }

    // Используйте со специализациями GenericValue.
    template<typename T> GenericUri(const T& uri, Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        const Ch* u = uri.template Get<const Ch*>(); // TypeHelper от document.h
        Parse(u, internal::StrLen<Ch>(u));
    }

#if RAPIDJSON_HAS_STDSTRING
    GenericUri(const String& uri, Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        Parse(uri.c_str(), internal::StrLen<Ch>(uri.c_str()));
    }
#endif

    //! Копировать конструктор
    GenericUri(const GenericUri& rhs) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(), ownAllocator_() {
        *this = rhs;
    }

    //! Копировать конструктор
    GenericUri(const GenericUri& rhs, Allocator* allocator) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        *this = rhs;
    }

    //! Деструктор.
    ~GenericUri() {
        Free();
        RAPIDJSON_DELETE(ownAllocator_);
    }

    //! Оператор присваивания
    GenericUri& operator=(const GenericUri& rhs) {
        if (this != &rhs) {
            // Не удалять ownAllocator
            Free();
            Allocate(rhs.GetStringLength());
            auth_ = CopyPart(scheme_, rhs.scheme_, rhs.GetSchemeStringLength());
            path_ = CopyPart(auth_, rhs.auth_, rhs.GetAuthStringLength());
            query_ = CopyPart(path_, rhs.path_, rhs.GetPathStringLength());
            frag_ = CopyPart(query_, rhs.query_, rhs.GetQueryStringLength());
            base_ = CopyPart(frag_, rhs.frag_, rhs.GetFragStringLength());
            uri_ = CopyPart(base_, rhs.base_, rhs.GetBaseStringLength());
            CopyPart(uri_, rhs.uri_, rhs.GetStringLength());
        }
        return *this;
    }

    //! Геттеры
    // Используйте со специализациями GenericValue.
    template<typename T> void Get(T& uri, Allocator& allocator) {
        uri.template Set<const Ch*>(this->GetString(), allocator); // TypeHelper от document.h
    }

    const Ch* GetString() const { return uri_; }
    SizeType GetStringLength() const { return uri_ == 0 ? 0 : internal::StrLen<Ch>(uri_); }
    const Ch* GetBaseString() const { return base_; }
    SizeType GetBaseStringLength() const { return base_ == 0 ? 0 : internal::StrLen<Ch>(base_); }
    const Ch* GetSchemeString() const { return scheme_; }
    SizeType GetSchemeStringLength() const { return scheme_ == 0 ? 0 : internal::StrLen<Ch>(scheme_); }
    const Ch* GetAuthString() const { return auth_; }
    SizeType GetAuthStringLength() const { return auth_ == 0 ? 0 : internal::StrLen<Ch>(auth_); }
    const Ch* GetPathString() const { return path_; }
    SizeType GetPathStringLength() const { return path_ == 0 ? 0 : internal::StrLen<Ch>(path_); }
    const Ch* GetQueryString() const { return query_; }
    SizeType GetQueryStringLength() const { return query_ == 0 ? 0 : internal::StrLen<Ch>(query_); }
    const Ch* GetFragString() const { return frag_; }
    SizeType GetFragStringLength() const { return frag_ == 0 ? 0 : internal::StrLen<Ch>(frag_); }

#if RAPIDJSON_HAS_STDSTRING
    static String Get(const GenericUri& uri) { return String(uri.GetString(), uri.GetStringLength()); }
    static String GetBase(const GenericUri& uri) { return String(uri.GetBaseString(), uri.GetBaseStringLength()); }
    static String GetScheme(const GenericUri& uri) { return String(uri.GetSchemeString(), uri.GetSchemeStringLength()); }
    static String GetAuth(const GenericUri& uri) { return String(uri.GetAuthString(), uri.GetAuthStringLength()); }
    static String GetPath(const GenericUri& uri) { return String(uri.GetPathString(), uri.GetPathStringLength()); }
    static String GetQuery(const GenericUri& uri) { return String(uri.GetQueryString(), uri.GetQueryStringLength()); }
    static String GetFrag(const GenericUri& uri) { return String(uri.GetFragString(), uri.GetFragStringLength()); }
#endif

    //! Операторы равенства
    bool operator==(const GenericUri& rhs) const {
        return Match(rhs, true);
    }

    bool operator!=(const GenericUri& rhs) const {
        return !Match(rhs, true);
    }

    bool Match(const GenericUri& uri, bool full = true) const {
        Ch* s1;
        Ch* s2;
        if (full) {
            s1 = uri_;
            s2 = uri.uri_;
        } else {
            s1 = base_;
            s2 = uri.base_;
        }
        if (s1 == s2) return true;
        if (s1 == 0 || s2 == 0) return false;
        return internal::StrCmp<Ch>(s1, s2) == 0;
    }

    //! Сопоставьте этот URI с другим (базовым) URI в соответствии с правилами разрешения URI.
    // См.  https://tools.ietf.org/html/rfc3986.
    // Используйте для входа идентификатор или $ref с идентификатором в области видимости.
    // Возвращает новый GenericUri для разрешенногоURI.
    GenericUri Resolve(const GenericUri& baseuri, Allocator* allocator = 0) {
        GenericUri resuri;
        resuri.allocator_ = allocator;
        // Обеспечьте достаточно места для объединения путей.
        resuri.Allocate(GetStringLength() + baseuri.GetStringLength() + 1); // + 1 за присоединение косой черты

        if (!(GetSchemeStringLength() == 0)) {
            // Используйте все это URI
            resuri.auth_ = CopyPart(resuri.scheme_, scheme_, GetSchemeStringLength());
            resuri.path_ = CopyPart(resuri.auth_, auth_, GetAuthStringLength());
            resuri.query_ = CopyPart(resuri.path_, path_, GetPathStringLength());
            resuri.frag_ = CopyPart(resuri.query_, query_, GetQueryStringLength());
            resuri.RemoveDotSegments();
        } else {
            // Используйте базовую схему
            resuri.auth_ = CopyPart(resuri.scheme_, baseuri.scheme_, baseuri.GetSchemeStringLength());
            if (!(GetAuthStringLength() == 0)) {
                // Используйте эту аутентификацию, путь, запрос
                resuri.path_ = CopyPart(resuri.auth_, auth_, GetAuthStringLength());
                resuri.query_ = CopyPart(resuri.path_, path_, GetPathStringLength());
                resuri.frag_ = CopyPart(resuri.query_, query_, GetQueryStringLength());
                resuri.RemoveDotSegments();
            } else {
                // Используйте базовую аутентификацию
                resuri.path_ = CopyPart(resuri.auth_, baseuri.auth_, baseuri.GetAuthStringLength());
                if (GetPathStringLength() == 0) {
                    // Используйте базовый путь
                    resuri.query_ = CopyPart(resuri.path_, baseuri.path_, baseuri.GetPathStringLength());
                    if (GetQueryStringLength() == 0) {
                        // Используйте базовый запрос
                        resuri.frag_ = CopyPart(resuri.query_, baseuri.query_, baseuri.GetQueryStringLength());
                    } else {
                        // Используйте этот запрос
                        resuri.frag_ = CopyPart(resuri.query_, query_, GetQueryStringLength());
                    }
                } else {
                    if (path_[0] == '/') {
                        // Абсолютный путь — используйте весь этот путь
                        resuri.query_ = CopyPart(resuri.path_, path_, GetPathStringLength());
                        resuri.RemoveDotSegments();
                    } else {
                        // Относительный путь — добавьте этот путь к базовому пути после последней косой черты базового пути.
                        size_t pos = 0;
                        if (!(baseuri.GetAuthStringLength() == 0) && baseuri.GetPathStringLength() == 0) {
                            resuri.path_[pos] = '/';
                            pos++;
                        }
                        size_t lastslashpos = baseuri.GetPathStringLength();
                        while (lastslashpos > 0) {
                            if (baseuri.path_[lastslashpos - 1] == '/') break;
                            lastslashpos--;
                        }
                        std::memcpy(&resuri.path_[pos], baseuri.path_, lastslashpos * sizeof(Ch));
                        pos += lastslashpos;
                        resuri.query_ = CopyPart(&resuri.path_[pos], path_, GetPathStringLength());
                        resuri.RemoveDotSegments();
                    }
                    // Используйте этот запрос
                    resuri.frag_ = CopyPart(resuri.query_, query_, GetQueryStringLength());
                }
            }
        }
        // Всегда используйте этот фрагмент
        resuri.base_ = CopyPart(resuri.frag_, frag_, GetFragStringLength());

        // Восстановитеbase_иuri_.
        resuri.SetBase();
        resuri.uri_ = resuri.base_ + resuri.GetBaseStringLength() + 1;
        resuri.SetUri();
        return resuri;
    }

    //! Получите доставку этого GenericUri.
    Allocator& GetAllocator() { return *allocator_; }

private:
    // Выделить память для URI
    // Возвращает общую выделенную сумму
    std::size_t Allocate(std::size_t len) {
        // Создайте собственный распределитель, если пользователь не предоставил его.
        if (!allocator_)
            ownAllocator_ =  allocator_ = RAPIDJSON_NEW(Allocator)();

        // Выделите один блок, содержащий каждую часть URI (5) плюс базовый плюс полный URI , все с нулевым завершением.
        // Order: схема, аутентификация, путь, запрос, фрагмент, база, uri
        // Обратите внимание, что необходимо устанавливать, увеличивать и назначать в три этапа, чтобы избежать ошибки предупреждения компилятора.
        size_t total = (3 * len + 7) * sizeof(Ch);
        scheme_ = static_cast<Ch*>(allocator_->Malloc(total));
        *scheme_ = '\0';
        auth_ = scheme_;
        auth_++;
        *auth_ = '\0';
        path_ = auth_;
        path_++;
        *path_ = '\0';
        query_ = path_;
        query_++;
        *query_ = '\0';
        frag_ = query_;
        frag_++;
        *frag_ = '\0';
        base_ = frag_;
        base_++;
        *base_ = '\0';
        uri_ = base_;
        uri_++;
        *uri_ = '\0';
        return total;
    }

    // Свободная память для URI
    void Free() {
        if (scheme_) {
            Allocator::Free(scheme_);
            scheme_ = 0;
        }
    }

    // Разобрать URI на составляющие схему, полномочия, путь, запрос и части фрагмента.
    // Supports URIs that match regex ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))? as per
    // https://tools.ietf.org/html/rfc3986
    void Parse(const Ch* uri, std::size_t len) {
        std::size_t start = 0, pos1 = 0, pos2 = 0;
        Allocate(len);

        // Look for scheme ([^:/?#]+):)?
        if (start < len) {
            while (pos1 < len) {
                if (uri[pos1] == ':') break;
                pos1++;
            }
            if (pos1 != len) {
                while (pos2 < len) {
                    if (uri[pos2] == '/') break;
                    if (uri[pos2] == '?') break;
                    if (uri[pos2] == '#') break;
                    pos2++;
                }
                if (pos1 < pos2) {
                    pos1++;
                    std::memcpy(scheme_, &uri[start], pos1 * sizeof(Ch));
                    scheme_[pos1] = '\0';
                    start = pos1;
                }
            }
        }
        // Look for auth (//([^/?#]*))?
        // Обратите внимание, что необходимо устанавливать, увеличивать и назначать в три этапа, чтобы избежать ошибки предупреждения компилятора.
        auth_ = scheme_ + GetSchemeStringLength();
        auth_++;
        *auth_ = '\0';
        if (start < len - 1 && uri[start] == '/' && uri[start + 1] == '/') {
            pos2 = start + 2;
            while (pos2 < len) {
                if (uri[pos2] == '/') break;
                if (uri[pos2] == '?') break;
                if (uri[pos2] == '#') break;
                pos2++;
            }
            std::memcpy(auth_, &uri[start], (pos2 - start) * sizeof(Ch));
            auth_[pos2 - start] = '\0';
            start = pos2;
        }
        // Look for path ([^?#]*)
        // Обратите внимание, что необходимо устанавливать, увеличивать и назначать в три этапа, чтобы избежать ошибки предупреждения компилятора.
        path_ = auth_ + GetAuthStringLength();
        path_++;
        *path_ = '\0';
        if (start < len) {
            pos2 = start;
            while (pos2 < len) {
                if (uri[pos2] == '?') break;
                if (uri[pos2] == '#') break;
                pos2++;
            }
            if (start != pos2) {
                std::memcpy(path_, &uri[start], (pos2 - start) * sizeof(Ch));
                path_[pos2 - start] = '\0';
                if (path_[0] == '/')
                    RemoveDotSegments();   // абсолютный путь - нормализовать
                start = pos2;
            }
        }
        // Look for query (\?([^#]*))?
        // Обратите внимание, что необходимо устанавливать, увеличивать и назначать в три этапа, чтобы избежать ошибки предупреждения компилятора.
        query_ = path_ + GetPathStringLength();
        query_++;
        *query_ = '\0';
        if (start < len && uri[start] == '?') {
            pos2 = start + 1;
            while (pos2 < len) {
                if (uri[pos2] == '#') break;
                pos2++;
            }
            if (start != pos2) {
                std::memcpy(query_, &uri[start], (pos2 - start) * sizeof(Ch));
                query_[pos2 - start] = '\0';
                start = pos2;
            }
        }
        // Look for fragment (#(.*))?
        // Обратите внимание, что необходимо устанавливать, увеличивать и назначать в три этапа, чтобы избежать ошибки предупреждения компилятора.
        frag_ = query_ + GetQueryStringLength();
        frag_++;
        *frag_ = '\0';
        if (start < len && uri[start] == '#') {
            std::memcpy(frag_, &uri[start], (len - start) * sizeof(Ch));
            frag_[len - start] = '\0';
        }

        // Восстановитеbase_иuri_.
        base_ = frag_ + GetFragStringLength() + 1;
        SetBase();
        uri_ = base_ + GetBaseStringLength() + 1;
        SetUri();
    }

    // Восстановить базу
    void SetBase() {
        Ch* next = base_;
        std::memcpy(next, scheme_, GetSchemeStringLength() * sizeof(Ch));
        next+= GetSchemeStringLength();
        std::memcpy(next, auth_, GetAuthStringLength() * sizeof(Ch));
        next+= GetAuthStringLength();
        std::memcpy(next, path_, GetPathStringLength() * sizeof(Ch));
        next+= GetPathStringLength();
        std::memcpy(next, query_, GetQueryStringLength() * sizeof(Ch));
        next+= GetQueryStringLength();
        *next = '\0';
    }

    // Восстановить ури
    void SetUri() {
        Ch* next = uri_;
        std::memcpy(next, base_, GetBaseStringLength() * sizeof(Ch));
        next+= GetBaseStringLength();
        std::memcpy(next, frag_, GetFragStringLength() * sizeof(Ch));
        next+= GetFragStringLength();
        *next = '\0';
    }

    // Скопируйте часть из одного GenericUri в другой.
    // Верните указатель на следующую часть, в которую нужно скопировать.
    Ch* CopyPart(Ch* to, Ch* from, std::size_t len) {
        RAPIDJSON_ASSERT(to != 0);
        RAPIDJSON_ASSERT(from != 0);
        std::memcpy(to, from, len * sizeof(Ch));
        to[len] = '\0';
        Ch* next = to + len + 1;
        return next;
    }

    // Удалить. и .. сегменты участниковpath_.
    // https://tools.ietf.org/html/rfc3986
    // Это делается на месте, поскольку мы удаляем только сегменты.
    void RemoveDotSegments() {
        std::size_t pathlen = GetPathStringLength();
        std::size_t pathpos = 0;  // Позиция в path_
        std::size_t newpos = 0;   // Позиция в новом path_

        // Прокрутите каждый сегмент в исходномpath_.
        while (pathpos < pathlen) {
            // Получить следующий сегмент, ограниченный '/' или концом
            size_t slashpos = 0;
            while ((pathpos + slashpos) < pathlen) {
                if (path_[pathpos + slashpos] == '/') break;
                slashpos++;
            }
            // Проверьте .. и . сегменты
            if (slashpos == 2 && path_[pathpos] == '.' && path_[pathpos + 1] == '.') {
                // Резервное копирование сегмента .. в новом path_
                // Мы ожидаем найти ранее добавленную косую черту в конце или ничего.
                RAPIDJSON_ASSERT(newpos == 0 || path_[newpos - 1] == '/');
                size_t lastslashpos = newpos;
                // Следим, чтобы мы не выходили за пределы стартового сегмента
                if (lastslashpos > 1) {
                    // Найдите предпоследнюю косую черту и вернитесь к ней.
                    lastslashpos--;
                    while (lastslashpos > 0) {
                        if (path_[lastslashpos - 1] == '/') break;
                        lastslashpos--;
                    }
                    // Установите новую позицию path_.
                    newpos = lastslashpos;
                }
            } else if (slashpos == 1 && path_[pathpos] == '.') {
                // Отбросить. сегмент, оставить новыйpath_без изменений
            } else {
                // Переместите сегмент любого другого типа в новыйpath_.
                RAPIDJSON_ASSERT(newpos <= pathpos);
                std::memmove(&path_[newpos], &path_[pathpos], slashpos * sizeof(Ch));
                newpos += slashpos;
                // Добавьте косую черту, если не в конце
                if ((pathpos + slashpos) < pathlen) {
                    path_[newpos] = '/';
                    newpos++;
                }
            }
            // Перейти к следующему сегменту
            pathpos += slashpos + 1;
        }
        path_[newpos] = '\0';
    }

    Ch* uri_;    // все
    Ch* base_;   // Все, кроме фрагмента
    Ch* scheme_; // Включает в себя:
    Ch* auth_;   // Включает //
    Ch* path_;   // Абсолютно, если начинается с /
    Ch* query_;  // Includes the ?
    Ch* frag_;   // Включает #

    Allocator* allocator_;      //!< Текущий распределитель. Он либо пользуется пользователем, либо равенownAllocator_.
    Allocator* ownAllocator_;   //!< Распределитель, последствия этого Ури.
};

//! GenericUri для значений (UTF-8, распределитель по умолчанию).
typedef GenericUri<Value> Uri;

RAPIDJSON_NAMESPACE_END

#if defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_URI_H_
