#include "QueryCraft/operator/isoperator.h"

namespace query_craft {
namespace operators {

std::string is_operator::sql() const
{
    return "IS";
}

bool is_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft