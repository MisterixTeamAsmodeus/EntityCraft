#include "QueryCraft/joincolumn.h"

std::ostream& query_craft::operator<<(std::ostream& os, const join_column& obj)
{
    switch(obj.join_type) {
        case join_column::type::inner: {
            os << " INNER ";
            break;
        }
        case join_column::type::outer: {
            os << " OUTER ";
            break;
        }
        case join_column::type::left: {
            os << " LEFT ";
            break;
        }
        case join_column::type::right: {
            os << " RIGHT ";
            break;
        }
        case join_column::type::cross: {
            os << " CROSS ";
            break;
        }
    }

    os << "JOIN " << obj.joined_table.table_name() << " ON " << obj.condition.unwrap(condion_view_type::full_name);

    return os;
}