#include <DatabaseAdapter/exception/opendatabaseexception.hpp>

#include <sstream>
#include <utility>

namespace database_adapter {

open_database_exception::open_database_exception(std::string message, int error_code)
    : _message(std::move(message))
    , _error_code(error_code)
{
    if(_error_code != 0) {
        std::ostringstream oss;
        oss << _message << " [Код ошибки: " << _error_code << "]";
        _message = oss.str();
    }
}

const char* open_database_exception::what() const noexcept
{
    return _message.c_str();
}

int open_database_exception::error_code() const
{
    return _error_code;
}

} // namespace database_adapter