#include "QueryCraft/operator/lessoperator.h"

namespace query_craft {
namespace operators {

std::string less_operator::sql() const
{
    return "<";
}

bool less_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft