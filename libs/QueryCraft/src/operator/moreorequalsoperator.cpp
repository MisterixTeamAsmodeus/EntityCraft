#include "QueryCraft/operator/moreorequalsoperator.h"

namespace query_craft {
namespace operators {

std::string more_or_equals_operator::sql() const
{
    return ">=";
}

bool more_or_equals_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft