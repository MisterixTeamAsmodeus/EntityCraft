#include "QueryCraft/operator/isnotoperator.h"

namespace query_craft {
namespace operators {

std::string is_not_operator::sql() const
{
    return "IS NOT";
}

bool is_not_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft