#pragma once

#include "ioperator.h"

namespace query_craft {
namespace operators {

/// Класс, представляющий оператор "<="
class less_or_equals_operator final : public IOperator
{
public:
    ~less_or_equals_operator() override = default;

    std::string sql() const override;

    bool need_bracket() const override;
};

} // namespace operators
} // namespace query_craft