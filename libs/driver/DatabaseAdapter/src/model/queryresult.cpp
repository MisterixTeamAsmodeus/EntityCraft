#include "DatabaseAdapter/model/queryresult.h"

namespace database_adapter {

query_result::query_result(const std::list<row>& result)
    : _result(result)
{
}

void query_result::add(const row& value)
{
    _result.emplace_back(value);
}

std::list<query_result::row> query_result::data() const
{
    return _result;
}

std::list<query_result::row>& query_result::mutable_data()
{
    return _result;
}

bool query_result::empty() const
{
    return _result.empty();
}

} // namespace database_adapter
