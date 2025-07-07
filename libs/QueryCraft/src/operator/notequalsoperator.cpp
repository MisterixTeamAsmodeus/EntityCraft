#include "QueryCraft/operator/notequalsoperator.h"

namespace query_craft {
namespace operators {

std::string not_equals_operator::sql() const
{
    return "<>";
}

bool not_equals_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft