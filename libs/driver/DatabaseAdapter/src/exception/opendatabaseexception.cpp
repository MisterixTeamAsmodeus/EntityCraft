#include <DatabaseAdapter/exception/opendatabaseexception.h>

namespace database_adapter {
open_database_exception::open_database_exception(std::string message)
    : _message(std::move(message))
{
}

const char* open_database_exception::what() const noexcept
{
    return _message.c_str();
}
} // namespace DatabaseAdapter