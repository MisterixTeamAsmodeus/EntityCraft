#include "DatabaseAdapter/model/queryresult.hpp"

#include <stdexcept>

namespace database_adapter {

query_result::query_result(const std::vector<row>& result)
    : _result(result)
{
}

void query_result::add(const row& value)
{
    _result.emplace_back(value);
}

std::vector<query_result::row> query_result::data() const
{
    return _result;
}

std::vector<query_result::row>& query_result::mutable_data()
{
    return _result;
}

bool query_result::empty() const
{
    return _result.empty();
}

size_t query_result::size() const
{
    return _result.size();
}

std::vector<query_result::row>::const_iterator query_result::cbegin() const
{
    return _result.cbegin();
}

std::vector<query_result::row>::const_iterator query_result::cend() const
{
    return _result.cend();
}

std::vector<query_result::row>::iterator query_result::begin()
{
    return _result.begin();
}

std::vector<query_result::row>::iterator query_result::end()
{
    return _result.end();
}

const query_result::row& query_result::at(size_t index) const
{
    return _result.at(index);
}

query_result::row& query_result::at(size_t index)
{
    return _result.at(index);
}

} // namespace database_adapter
