#include "QueryCraft/operator/inoperator.h"

namespace query_craft {
namespace operators {

std::string in_operator::sql() const
{
    return "IN";
}

bool in_operator::need_bracket() const
{
    return true;
}

} // namespace operators
} // namespace query_craft