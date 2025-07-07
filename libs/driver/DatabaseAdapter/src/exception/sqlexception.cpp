#include <DatabaseAdapter/exception/sqlexception.h>

#include <utility>

namespace database_adapter {
sql_exception::sql_exception(std::string message)
    : _message(std::move(message))
{
}

sql_exception::sql_exception(std::string message, std::string last_query)
    : _message(std::move(message))
    , _last_query(std::move(last_query))
{
}

const char* sql_exception::what() const noexcept
{
    return _message.c_str();
}

std::string sql_exception::last_query() const
{
    return _last_query;
}
} // namespace DatabaseAdapter