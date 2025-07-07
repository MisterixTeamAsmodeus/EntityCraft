#include "QueryCraft/operator/moreoperator.h"

namespace query_craft {
namespace operators {

std::string more_operator::sql() const
{
    return ">";
}

bool more_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft