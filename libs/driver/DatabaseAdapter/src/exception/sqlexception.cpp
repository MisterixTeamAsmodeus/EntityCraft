#include <DatabaseAdapter/exception/sqlexception.hpp>

#include <sstream>
#include <utility>

namespace database_adapter {

sql_exception::sql_exception(std::string message, std::string last_query, int error_code)
    : _message(std::move(message))
    , _last_query(std::move(last_query))
    , _error_code(error_code)
{
    if(_error_code != 0) {
        std::ostringstream oss;
        oss << _message << " [Код ошибки: " << _error_code << "]";
        if(!_last_query.empty()) {
            oss << " [Запрос: " << _last_query << "]";
        }
        _message = oss.str();
    } else if(!_last_query.empty()) {
        std::ostringstream oss;
        oss << _message << " [Запрос: " << _last_query << "]";
        _message = oss.str();
    }
}

const char* sql_exception::what() const noexcept
{
    return _message.c_str();
}

std::string sql_exception::last_query() const
{
    return _last_query;
}

int sql_exception::error_code() const
{
    return _error_code;
}

} // namespace database_adapter