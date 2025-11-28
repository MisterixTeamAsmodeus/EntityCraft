#include <DatabaseAdapter/model/connection_entry.hpp>

namespace database_adapter {

ConnectionEntry::ConnectionEntry(std::shared_ptr<IConnection> conn)
    : _connection(std::move(conn))
    , _last_used(std::chrono::system_clock::now())
{
}

std::shared_ptr<IConnection> ConnectionEntry::connection() const
{
    return _connection;
}

std::chrono::system_clock::time_point ConnectionEntry::last_used() const
{
    return _last_used;
}

void ConnectionEntry::update_last_used()
{
    _last_used = std::chrono::system_clock::now();
}

void ConnectionEntry::set_last_used(const std::chrono::system_clock::time_point& time_point)
{
    _last_used = time_point;
}
} // namespace database_adapter