#include "QueryCraft/conditiongroup.h"

#include "QueryCraft/operator/isnotoperator.h"
#include "QueryCraft/operator/isoperator.h"
#include "QueryCraft/operator/likeoperator.h"

namespace query_craft {

condition_group::condition::column::column(std::string name, const settings settings)
    : _name(std::move(name))
    , _columnSettings(settings)
{
}

bool condition_group::condition::column::operator==(const column& rhs) const
{
    return _name == rhs._name
        && _fullName == rhs._fullName
        && _alias == rhs._alias
        && _columnSettings == rhs._columnSettings;
}

bool condition_group::condition::column::operator!=(const column& rhs) const
{
    return !(*this == rhs);
}

std::string condition_group::condition::column::name() const
{
    return _name;
}

std::string condition_group::condition::column::full_name() const
{
    return _fullName;
}

void condition_group::condition::column::set_full_name(const std::string& fullName)
{
    _fullName = fullName;
}

std::string condition_group::condition::column::alias() const
{
    return _alias;
}

void condition_group::condition::column::set_alias(const std::string& alias)
{
    _alias = alias;
}

void condition_group::condition::column::add_settings(const settings settings)
{
    _columnSettings = settings | _columnSettings;
}

bool condition_group::condition::column::has_settings(const settings settings) const
{
    return (_columnSettings | settings) == _columnSettings;
}

condition_group::condition condition_group::condition::column::is_null() const
{
    return create_condition(std::move(std::make_shared<operators::is_operator>()), { null_value() });
}

condition_group::condition condition_group::condition::column::not_null() const
{
    return create_condition(std::move(std::make_shared<operators::is_not_operator>()), { null_value() });
}

condition_group::condition condition_group::condition::column::like(const std::string& pattern) const
{
    return create_condition(std::move(std::make_shared<operators::like_operator>()), { pattern });
}

condition_group::condition condition_group::condition::column::equals(const column& value) const
{
    return create_condition(std::make_shared<operators::equals_operator>(), { value.full_name() }, false);
}

condition_group::condition condition_group::condition::column::not_equals(const column& value) const
{
    return create_condition(std::make_shared<operators::not_equals_operator>(), { value.full_name() }, false);
}

condition_group::condition condition_group::condition::column::less(const column& value) const
{
    return create_condition(std::make_shared<operators::less_operator>(), { value.full_name() }, false);
}

condition_group::condition condition_group::condition::column::less_or_equals(const column& value) const
{
    return create_condition(std::make_shared<operators::less_or_equals_operator>(), { value.full_name() }, false);
}

condition_group::condition condition_group::condition::column::more(const column& value) const
{
    return create_condition(std::make_shared<operators::more_operator>(), { value.full_name() }, false);
}

condition_group::condition condition_group::condition::column::more_or_equals(const column& value) const
{
    return create_condition(std::make_shared<operators::more_or_equals_operator>(), { value.full_name() }, false);
}

bool condition_group::condition::column::is_valid() const
{
    return !_name.empty() || !_fullName.empty() || !_alias.empty() || _columnSettings != settings::none;
}

condition_group::condition condition_group::condition::column::create_condition(std::shared_ptr<operators::IOperator>&& conditionOperator, std::vector<std::string>&& values, bool need_forging) const
{
    condition condition;

    condition._condition_operator = std::move(conditionOperator);
    condition._column = *this;
    condition._values = std::move(values);
    condition._need_forging = need_forging;

    return std::move(condition);
}

condition_group condition_group::condition::operator&&(const condition& rhd) const
{
    condition_group group;

    std::get<0>(group._node) = logical_operator::and_;

    group._left = std::make_shared<condition_group>();
    std::get<1>(group._left->_node) = *this;

    group._right = std::make_shared<condition_group>();
    std::get<1>(group._right->_node) = rhd;

    return group;
}

condition_group condition_group::condition::operator&&(const condition_group& rhd) const
{
    condition_group group;

    std::get<0>(group._node) = logical_operator::and_;

    group._left = std::make_shared<condition_group>();
    std::get<1>(group._left->_node) = *this;

    group._right = std::make_shared<condition_group>(rhd);

    return group;
}

condition_group condition_group::condition::operator||(const condition& rhd) const
{
    condition_group group;

    std::get<0>(group._node) = logical_operator::or_;

    group._left = std::make_shared<condition_group>();
    std::get<1>(group._left->_node) = *this;

    group._right = std::make_shared<condition_group>();
    std::get<1>(group._right->_node) = rhd;

    return group;
}

condition_group condition_group::condition::operator||(const condition_group& rhd) const
{
    condition_group group;

    std::get<0>(group._node) = logical_operator::or_;

    group._left = std::make_shared<condition_group>();
    std::get<1>(group._left->_node) = *this;

    group._right = std::make_shared<condition_group>(rhd);

    return group;
}

std::string condition_group::condition::unwrap(const condion_view_type view_type) const
{
    if(_values.empty())
        return "";

    std::stringstream stream;

    switch(view_type) {
        case condion_view_type::name: {
            stream << "\"" << _column.name() << "\"";
            break;
        }
        case condion_view_type::alias: {
            stream << _column.alias();
            break;
        }
        case condion_view_type::full_name: {
            stream << _column.full_name();
            break;
        }
    }

    stream << " " << _condition_operator->sql() << " ";

    if(_condition_operator->need_bracket())
        stream << "(";

    std::for_each(_values.begin(), _values.end(), [&stream, this](const auto& value) {
        if(value != column::null_value() && _need_forging)
            stream << "'";

        stream << value;

        if(value != column::null_value() && _need_forging)
            stream << "'";

        if(_condition_operator->need_bracket())
            stream << ", ";
    });

    if(_condition_operator->need_bracket()) {
        stream.seekp(-2, std::stringstream::cur);
        stream << ")";
    }

    return stream.str();
}

condition_group::condition::column condition_group::condition::condition_column() const
{
    return _column;
}

std::shared_ptr<operators::IOperator> condition_group::condition::condition_operator() const
{
    return _condition_operator;
}

std::vector<std::string> condition_group::condition::values() const
{
    return _values;
}

bool condition_group::condition::is_valid() const
{
    return _condition_operator != nullptr || !_values.empty() || _column.is_valid();
}

condition_group::condition_group(const condition& condition)
{
    std::get<1>(_node) = condition;
}

condition_group condition_group::operator&&(const condition_group& rhd) const
{
    condition_group group;

    std::get<0>(group._node) = logical_operator::and_;

    group._left = std::make_shared<condition_group>(*this);

    group._right = std::make_shared<condition_group>(rhd);

    return group;
}

condition_group condition_group::operator&&(const condition& rhd) const
{
    condition_group group;

    std::get<0>(group._node) = logical_operator::and_;

    group._left = std::make_shared<condition_group>(*this);

    group._right = std::make_shared<condition_group>();
    std::get<1>(group._right->_node) = rhd;

    return group;
}

condition_group condition_group::operator||(const condition_group& rhd) const
{
    condition_group group;

    std::get<0>(group._node) = logical_operator::or_;

    group._left = std::make_shared<condition_group>(*this);

    group._right = std::make_shared<condition_group>(rhd);

    return group;
}

condition_group condition_group::operator||(const condition& rhd) const
{
    condition_group group;

    std::get<0>(group._node) = logical_operator::or_;

    group._left = std::make_shared<condition_group>(*this);

    group._right = std::make_shared<condition_group>();
    std::get<1>(group._right->_node) = rhd;

    return group;
}

std::string condition_group::unwrap(const condion_view_type view_type, const bool compressed) const
{
    std::stringstream stream;
    unwrap_tree(this, stream, view_type, compressed);

    return stream.str();
}

bool condition_group::is_valid() const
{
    return !is_sheet() || std::get<1>(_node).is_valid();
}

bool condition_group::is_sheet() const
{
    return _left == _right && _left == nullptr;
}

void condition_group::unwrap_tree(const condition_group* node, std::stringstream& stream, const condion_view_type view_type, const bool compressed)
{
    if(node->is_sheet()) {
        stream << std::get<1>(node->_node).unwrap(view_type);
        return;
    }

    stream << "(";

    if(node->_left != nullptr)
        unwrap_tree(node->_left.get(), stream, view_type, compressed);

    if(!compressed)
        stream << "\n";

    switch(std::get<0>(node->_node)) {
        case logical_operator::and_: {
            stream << " AND ";
            break;
        }
        case logical_operator::or_: {
            stream << " OR ";
            break;
        }
    }

    if(node->_right != nullptr)
        unwrap_tree(node->_right.get(), stream, view_type, compressed);

    stream << ")";
}

column_settings operator|(column_settings a, column_settings b)
{
    return static_cast<column_settings>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

column_settings primary_key()
{
    return column_settings::primary_key | column_settings::not_null;
}

column_settings not_null()
{
    return column_settings::not_null;
}

} // namespace query_craft
