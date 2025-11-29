#include "SqliteAdapter/sqliteerrorcode.hpp"

namespace database_adapter {
namespace sqlite {

sqlite_error_code to_sqlite_error_code(int code)
{
    // SQLite возвращает расширенные коды ошибок, но базовый код находится в младших 8 битах
    const int base_code = code & 0xFF;
    
    // Проверяем, является ли это известным кодом ошибки
    switch(base_code) {
        case SQLITE_OK:
            return sqlite_error_code::OK;
        case SQLITE_ERROR:
            return sqlite_error_code::ERROR;
        case SQLITE_INTERNAL:
            return sqlite_error_code::INTERNAL;
        case SQLITE_PERM:
            return sqlite_error_code::PERM;
        case SQLITE_ABORT:
            return sqlite_error_code::ABORT;
        case SQLITE_BUSY:
            return sqlite_error_code::BUSY;
        case SQLITE_LOCKED:
            return sqlite_error_code::LOCKED;
        case SQLITE_NOMEM:
            return sqlite_error_code::NOMEM;
        case SQLITE_READONLY:
            return sqlite_error_code::READONLY;
        case SQLITE_INTERRUPT:
            return sqlite_error_code::INTERRUPT;
        case SQLITE_IOERR:
            return sqlite_error_code::IOERR;
        case SQLITE_CORRUPT:
            return sqlite_error_code::CORRUPT;
        case SQLITE_NOTFOUND:
            return sqlite_error_code::NOTFOUND;
        case SQLITE_FULL:
            return sqlite_error_code::FULL;
        case SQLITE_CANTOPEN:
            return sqlite_error_code::CANTOPEN;
        case SQLITE_PROTOCOL:
            return sqlite_error_code::PROTOCOL;
        case SQLITE_EMPTY:
            return sqlite_error_code::EMPTY;
        case SQLITE_SCHEMA:
            return sqlite_error_code::SCHEMA;
        case SQLITE_TOOBIG:
            return sqlite_error_code::TOOBIG;
        case SQLITE_CONSTRAINT:
            return sqlite_error_code::CONSTRAINT;
        case SQLITE_MISMATCH:
            return sqlite_error_code::MISMATCH;
        case SQLITE_MISUSE:
            return sqlite_error_code::MISUSE;
        case SQLITE_NOLFS:
            return sqlite_error_code::NOLFS;
        case SQLITE_AUTH:
            return sqlite_error_code::AUTH;
        case SQLITE_FORMAT:
            return sqlite_error_code::FORMAT;
        case SQLITE_RANGE:
            return sqlite_error_code::RANGE;
        case SQLITE_NOTADB:
            return sqlite_error_code::NOTADB;
        case SQLITE_NOTICE:
            return sqlite_error_code::NOTICE;
        case SQLITE_WARNING:
            return sqlite_error_code::WARNING;
        case SQLITE_ROW:
            return sqlite_error_code::ROW;
        case SQLITE_DONE:
            return sqlite_error_code::DONE;
        default:
            // Для неизвестных кодов возвращаем ERROR как общий код ошибки
            return sqlite_error_code::ERROR;
    }
}

int to_int(sqlite_error_code code)
{
    return static_cast<int>(code);
}

bool is_success(sqlite_error_code code)
{
    return code == sqlite_error_code::OK ||
           code == sqlite_error_code::ROW ||
           code == sqlite_error_code::DONE;
}

bool is_critical_error(sqlite_error_code code)
{
    return code == sqlite_error_code::CORRUPT ||
           code == sqlite_error_code::NOTADB ||
           code == sqlite_error_code::INTERNAL ||
           code == sqlite_error_code::NOMEM;
}

} // namespace sqlite
} // namespace database_adapter

