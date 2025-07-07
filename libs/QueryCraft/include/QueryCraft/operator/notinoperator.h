#pragma once

#include "ioperator.h"

namespace query_craft {
namespace operators {

/// Класс, представляющий оператор "NOT IN"
class not_in_operator final : public IOperator
{
public:
    ~not_in_operator() override = default;

    std::string sql() const override;

    bool need_bracket() const override;
};

} // namespace operators
} // namespace query_craft