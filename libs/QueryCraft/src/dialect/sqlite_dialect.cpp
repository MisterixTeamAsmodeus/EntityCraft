#include <QueryCraft/dialect/sqlite_dialect.h>

#include <sstream>

namespace query_craft {

std::string sqlite_dialect::quote_identifier(const std::string& identifier) const
{
    std::ostringstream stream;
    stream << '"' << identifier << '"';
    return stream.str();
}

std::string sqlite_dialect::placeholder(std::size_t /*index*/) const
{
    return "?";
}

std::string sqlite_dialect::format_limit_offset(std::size_t limit, std::size_t offset) const
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

bool sqlite_dialect::supports_returning() const
{
    // SQLite поддерживает RETURNING начиная с версии 3.35.0 (март 2021).
    // Предполагаем использование версии не ниже указанной.
    return true;
}

std::string sqlite_dialect::function_name(const std::string& logical_name) const
{
    return logical_name;
}

} // namespace query_craft
