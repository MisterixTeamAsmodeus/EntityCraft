#include "QueryCraft/operator/lessorequalsoperator.h"

namespace query_craft {
namespace operators {

std::string less_or_equals_operator::sql() const
{
    return "<=";
}

bool less_or_equals_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft