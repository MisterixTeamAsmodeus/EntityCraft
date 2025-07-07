#pragma once

#include "ioperator.h"

namespace query_craft {
namespace operators {

/// Класс, представляющий оператор "IS"
class is_operator final : public IOperator
{
public:
    ~is_operator() override = default;

    std::string sql() const override;

    bool need_bracket() const override;
};

} // namespace operators
} // namespace query_craft