#include "QueryCraft/operator/likeoperator.h"

namespace query_craft {
namespace operators {

std::string like_operator::sql() const
{
    return "LIKE";
}

bool like_operator::need_bracket() const
{
    return false;
}

} // namespace operators
} // namespace query_craft