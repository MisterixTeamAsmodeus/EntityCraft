#include "QueryCraft/operator/notinoperator.h"

namespace query_craft {
namespace operators {

std::string not_in_operator::sql() const
{
    return "NOT IN";
}

bool not_in_operator::need_bracket() const
{
    return true;
}

} // namespace operators
} // namespace query_craft