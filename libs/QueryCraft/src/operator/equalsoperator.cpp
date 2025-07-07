#include "QueryCraft/operator/equalsoperator.h"

namespace query_craft {
namespace operators {

std::string equals_operator::sql() const
{
    return "=";
}

bool equals_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft