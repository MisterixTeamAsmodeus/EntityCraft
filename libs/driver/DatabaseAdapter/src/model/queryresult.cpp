#include "DatabaseAdapter/model/queryresult.h"

namespace database_adapter {
namespace models {

query_result::query_result(const std::list<result_row>& result)
    : _result(result)
{
}

void query_result::add_row(const result_row& value)
{
    _result.emplace_back(value);
}

std::list<query_result::result_row> query_result::data() const
{
    return _result;
}

std::list<query_result::result_row>& query_result::operator()()
{
    return _result;
}

bool query_result::empty() const
{
    return _result.empty();
}
} // namespace models
} // namespace database_adapter
