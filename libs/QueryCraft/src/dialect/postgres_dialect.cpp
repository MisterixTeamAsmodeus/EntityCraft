#include <QueryCraft/dialect/postgres_dialect.h>

#include <sstream>

namespace query_craft {

std::string postgres_dialect::quote_identifier(const std::string& identifier) const
{
    // Оптимизация: используем резервирование памяти и конкатенацию
    // вместо std::ostringstream для лучшей производительности
    std::string result;
    result.reserve(identifier.size() + 2);
    result += '"';
    result += identifier;
    result += '"';
    return result;
}

std::string postgres_dialect::placeholder(std::size_t index) const
{
    // Оптимизация: используем std::to_string напрямую
    return "$" + std::to_string(index);
}

std::string postgres_dialect::format_limit_offset(std::size_t limit, std::size_t offset) const
{
    std::ostringstream stream;

    if(limit > 0) {
        stream << " LIMIT " << limit;
    }

    if(offset > 0) {
        stream << " OFFSET " << offset;
    }

    return stream.str();
}

bool postgres_dialect::supports_returning() const
{
    return true;
}

std::string postgres_dialect::function_name(const std::string& logical_name) const
{
    return logical_name;
}

} // namespace query_craft
