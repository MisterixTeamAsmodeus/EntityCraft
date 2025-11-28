#pragma once

#include "typeconverter_base.hpp"

#include <array>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#if __cplusplus >= 201703L
#    include <any>
#    include <optional>
#    include <string_view>
#    include <variant>
#endif

namespace type_converter_api {

/// Специализация для std::pair
template<typename A, typename B>
class type_converter<std::pair<A, B>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::pair<A, B>& value, const std::string& str) const
    {
        // Формат: "first:second" или "first,second"
        std::size_t separator_pos = str.find(':');
        if(separator_pos == std::string::npos) {
            separator_pos = str.find(',');
        }
        if(separator_pos == std::string::npos) {
            throw std::invalid_argument("Invalid pair format: expected 'key:value' or 'key,value', got: " + str);
        }

        std::string first_str = str.substr(0, separator_pos);
        std::string second_str = str.substr(separator_pos + 1);

        type_converter<A> first_converter;
        type_converter<B> second_converter;
        first_converter.fill_from_string(value.first, first_str);
        second_converter.fill_from_string(value.second, second_str);
    }

    virtual std::string convert_to_string(const std::pair<A, B>& value) const
    {
        type_converter<A> first_converter;
        type_converter<B> second_converter;
        return first_converter.convert_to_string(value.first) + ":" + second_converter.convert_to_string(value.second);
    }
};

/// Специализация для std::vector
template<typename T>
class type_converter<std::vector<T>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::vector<T>& value, const std::string& str) const
    {
        value.clear();
        if(str.empty())
            return;

        // Формат: "item1,item2,item3" или "item1|item2|item3"
        std::string delimiter = ",";
        if(str.find('|') != std::string::npos) {
            delimiter = "|";
        }

        std::size_t start = 0;
        std::size_t end = str.find(delimiter);
        type_converter<T> converter;

        while(end != std::string::npos) {
            std::string item_str = str.substr(start, end - start);
            if(!item_str.empty()) {
                T item;
                converter.fill_from_string(item, item_str);
                value.push_back(item);
            }
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }

        std::string item_str = str.substr(start);
        if(!item_str.empty()) {
            T item;
            converter.fill_from_string(item, item_str);
            value.push_back(item);
        }
    }

    virtual std::string convert_to_string(const std::vector<T>& value) const
    {
        if(value.empty())
            return "";

        type_converter<T> converter;
        std::string result = converter.convert_to_string(value[0]);
        for(std::size_t i = 1; i < value.size(); ++i) {
            result += "," + converter.convert_to_string(value[i]);
        }
        return result;
    }
};

/// Специализация для std::array
template<typename T, std::size_t N>
class type_converter<std::array<T, N>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::array<T, N>& value, const std::string& str) const
    {
        if(str.empty()) {
            value.fill(T {});
            return;
        }

        // Формат: "item1,item2,item3"
        std::size_t start = 0;
        std::size_t end = str.find(',');
        type_converter<T> converter;
        std::size_t index = 0;

        while(end != std::string::npos && index < N) {
            std::string item_str = str.substr(start, end - start);
            if(!item_str.empty()) {
                converter.fill_from_string(value[index], item_str);
                ++index;
            }
            start = end + 1;
            end = str.find(',', start);
        }

        if(index < N) {
            std::string item_str = str.substr(start);
            if(!item_str.empty()) {
                converter.fill_from_string(value[index], item_str);
            }
        }
    }

    virtual std::string convert_to_string(const std::array<T, N>& value) const
    {
        type_converter<T> converter;
        std::string result = converter.convert_to_string(value[0]);
        for(std::size_t i = 1; i < N; ++i) {
            result += "," + converter.convert_to_string(value[i]);
        }
        return result;
    }
};

/// Специализация для std::deque
template<typename T>
class type_converter<std::deque<T>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::deque<T>& value, const std::string& str) const
    {
        value.clear();
        if(str.empty())
            return;

        std::size_t start = 0;
        std::size_t end = str.find(',');
        type_converter<T> converter;

        while(end != std::string::npos) {
            std::string item_str = str.substr(start, end - start);
            if(!item_str.empty()) {
                T item;
                converter.fill_from_string(item, item_str);
                value.push_back(item);
            }
            start = end + 1;
            end = str.find(',', start);
        }

        std::string item_str = str.substr(start);
        if(!item_str.empty()) {
            T item;
            converter.fill_from_string(item, item_str);
            value.push_back(item);
        }
    }

    virtual std::string convert_to_string(const std::deque<T>& value) const
    {
        if(value.empty())
            return "";

        type_converter<T> converter;
        std::string result = converter.convert_to_string(value[0]);
        for(std::size_t i = 1; i < value.size(); ++i) {
            result += "," + converter.convert_to_string(value[i]);
        }
        return result;
    }
};

/// Специализация для std::list
template<typename T>
class type_converter<std::list<T>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::list<T>& value, const std::string& str) const
    {
        value.clear();
        if(str.empty())
            return;

        std::size_t start = 0;
        std::size_t end = str.find(',');
        type_converter<T> converter;

        while(end != std::string::npos) {
            std::string item_str = str.substr(start, end - start);
            if(!item_str.empty()) {
                T item;
                converter.fill_from_string(item, item_str);
                value.push_back(item);
            }
            start = end + 1;
            end = str.find(',', start);
        }

        std::string item_str = str.substr(start);
        if(!item_str.empty()) {
            T item;
            converter.fill_from_string(item, item_str);
            value.push_back(item);
        }
    }

    virtual std::string convert_to_string(const std::list<T>& value) const
    {
        if(value.empty())
            return "";

        type_converter<T> converter;
        std::string result;
        bool first = true;
        for(const auto& item : value) {
            if(!first)
                result += ",";
            result += converter.convert_to_string(item);
            first = false;
        }
        return result;
    }
};

/// Специализация для std::set
template<typename T>
class type_converter<std::set<T>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::set<T>& value, const std::string& str) const
    {
        value.clear();
        if(str.empty())
            return;

        std::size_t start = 0;
        std::size_t end = str.find(',');
        type_converter<T> converter;

        while(end != std::string::npos) {
            std::string item_str = str.substr(start, end - start);
            if(!item_str.empty()) {
                T item;
                converter.fill_from_string(item, item_str);
                value.insert(item);
            }
            start = end + 1;
            end = str.find(',', start);
        }

        std::string item_str = str.substr(start);
        if(!item_str.empty()) {
            T item;
            converter.fill_from_string(item, item_str);
            value.insert(item);
        }
    }

    virtual std::string convert_to_string(const std::set<T>& value) const
    {
        if(value.empty())
            return "";

        type_converter<T> converter;
        std::string result;
        bool first = true;
        for(const auto& item : value) {
            if(!first)
                result += ",";
            result += converter.convert_to_string(item);
            first = false;
        }
        return result;
    }
};

/// Специализация для std::multiset
template<typename T>
class type_converter<std::multiset<T>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::multiset<T>& value, const std::string& str) const
    {
        value.clear();
        if(str.empty())
            return;

        std::size_t start = 0;
        std::size_t end = str.find(',');
        type_converter<T> converter;

        while(end != std::string::npos) {
            std::string item_str = str.substr(start, end - start);
            if(!item_str.empty()) {
                T item;
                converter.fill_from_string(item, item_str);
                value.insert(item);
            }
            start = end + 1;
            end = str.find(',', start);
        }

        std::string item_str = str.substr(start);
        if(!item_str.empty()) {
            T item;
            converter.fill_from_string(item, item_str);
            value.insert(item);
        }
    }

    virtual std::string convert_to_string(const std::multiset<T>& value) const
    {
        if(value.empty())
            return "";

        type_converter<T> converter;
        std::string result;
        bool first = true;
        for(const auto& item : value) {
            if(!first)
                result += ",";
            result += converter.convert_to_string(item);
            first = false;
        }
        return result;
    }
};

/// Специализация для std::map
template<typename K, typename V>
class type_converter<std::map<K, V>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::map<K, V>& value, const std::string& str) const
    {
        value.clear();
        if(str.empty())
            return;

        // Формат: "key1:value1,key2:value2"
        std::size_t start = 0;
        type_converter<K> key_converter;
        type_converter<V> value_converter;

        while(start < str.length()) {
            std::size_t colon_pos = str.find(':', start);
            if(colon_pos == std::string::npos) {
                throw std::invalid_argument("Invalid map format: expected 'key:value', got: " + str.substr(start));
            }

            std::string key_str = str.substr(start, colon_pos - start);
            std::size_t comma_pos = str.find(',', colon_pos + 1);
            std::string value_str;
            if(comma_pos == std::string::npos) {
                value_str = str.substr(colon_pos + 1);
                start = str.length();
            } else {
                value_str = str.substr(colon_pos + 1, comma_pos - colon_pos - 1);
                start = comma_pos + 1;
            }

            K key;
            V val;
            key_converter.fill_from_string(key, key_str);
            value_converter.fill_from_string(val, value_str);
            value[key] = val;
        }
    }

    virtual std::string convert_to_string(const std::map<K, V>& value) const
    {
        if(value.empty())
            return "";

        type_converter<K> key_converter;
        type_converter<V> value_converter;
        std::string result;
        bool first = true;
        for(const auto& pair : value) {
            if(!first)
                result += ",";
            result += key_converter.convert_to_string(pair.first) + ":" + value_converter.convert_to_string(pair.second);
            first = false;
        }
        return result;
    }
};

/// Специализация для std::unordered_map
template<typename K, typename V>
class type_converter<std::unordered_map<K, V>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::unordered_map<K, V>& value, const std::string& str) const
    {
        value.clear();
        if(str.empty())
            return;

        // Формат: "key1:value1,key2:value2"
        std::size_t start = 0;
        type_converter<K> key_converter;
        type_converter<V> value_converter;

        while(start < str.length()) {
            std::size_t colon_pos = str.find(':', start);
            if(colon_pos == std::string::npos) {
                throw std::invalid_argument("Invalid unordered_map format: expected 'key:value', got: " + str.substr(start));
            }

            std::string key_str = str.substr(start, colon_pos - start);
            std::size_t comma_pos = str.find(',', colon_pos + 1);
            std::string value_str;
            if(comma_pos == std::string::npos) {
                value_str = str.substr(colon_pos + 1);
                start = str.length();
            } else {
                value_str = str.substr(colon_pos + 1, comma_pos - colon_pos - 1);
                start = comma_pos + 1;
            }

            K key;
            V val;
            key_converter.fill_from_string(key, key_str);
            value_converter.fill_from_string(val, value_str);
            value[key] = val;
        }
    }

    virtual std::string convert_to_string(const std::unordered_map<K, V>& value) const
    {
        if(value.empty())
            return "";

        type_converter<K> key_converter;
        type_converter<V> value_converter;
        std::string result;
        bool first = true;
        for(const auto& pair : value) {
            if(!first)
                result += ",";
            result += key_converter.convert_to_string(pair.first) + ":" + value_converter.convert_to_string(pair.second);
            first = false;
        }
        return result;
    }
};

#if __cplusplus >= 201703L

/// Специализация для std::optional
template<typename T>
class type_converter<std::optional<T>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::optional<T>& value, const std::string& str) const
    {
        // Пустая строка или "null" означает отсутствие значения
        if(str.empty() || str == "null" || str == "nullopt" || str == "none") {
            value = std::nullopt;
            return;
        }

        // Иначе создаем значение и заполняем его
        T temp_value {};
        type_converter<T> converter;
        converter.fill_from_string(temp_value, str);
        value = temp_value;
    }

    virtual std::string convert_to_string(const std::optional<T>& value) const
    {
        if(!value.has_value()) {
            return "null";
        }

        type_converter<T> converter;
        return converter.convert_to_string(*value);
    }
};

/// Специализация для std::string_view
template<>
class type_converter<std::string_view>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::string_view& value, const std::string& str) const
    {
        // string_view не может быть изменен после создания, поэтому используем временную строку
        // В реальности это ограничение - string_view должен указывать на существующую строку
        // Для fill_from_string это проблематично, но мы можем выбросить исключение
        // или использовать статическую строку (не рекомендуется)
        throw std::runtime_error("fill_from_string for std::string_view is not supported: "
                                 "string_view cannot be modified after construction. "
                                 "Use std::string instead.");
    }

    virtual std::string convert_to_string(const std::string_view& value) const
    {
        return std::string(value);
    }
};

/// Специализация для std::variant (поддержка произвольного количества типов)
/// Использует рекурсивный подход для обработки всех типов в variant
template<typename... Types>
class type_converter<std::variant<Types...>>
{
private:
    template<std::size_t Index>
    static void try_fill_at_index(std::variant<Types...>& value, const std::string& value_str)
    {
        using TypeAt = std::variant_alternative_t<Index, std::variant<Types...>>;
        TypeAt temp_value {};
        type_converter<TypeAt> converter;
        converter.fill_from_string(temp_value, value_str);
        value = temp_value;
    }

    template<std::size_t... Indices>
    static void fill_by_index_impl(std::variant<Types...>& value, std::size_t index,
        const std::string& value_str, std::index_sequence<Indices...>)
    {
        ((index == Indices ? (try_fill_at_index<Indices>(value, value_str), true) : false) || ...);
    }

    template<std::size_t Index>
    static bool try_fill_auto(std::variant<Types...>& value, const std::string& str)
    {
        using TypeAt = std::variant_alternative_t<Index, std::variant<Types...>>;
        try {
            TypeAt temp_value {};
            type_converter<TypeAt> converter;
            converter.fill_from_string(temp_value, str);
            value = temp_value;
            return true;
        } catch(...) {
            return false;
        }
    }

    template<std::size_t... Indices>
    static bool try_fill_auto_impl(std::variant<Types...>& value, const std::string& str,
        std::index_sequence<Indices...>)
    {
        return (try_fill_auto<Indices>(value, str) || ...);
    }

public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::variant<Types...>& value, const std::string& str) const
    {
        // Формат: "type_index:value" или просто "value" (пробуем все типы)
        std::size_t colon_pos = str.find(':');

        if(colon_pos != std::string::npos) {
            // Явно указан индекс типа
            std::string index_str = str.substr(0, colon_pos);
            std::string value_str = str.substr(colon_pos + 1);

            std::size_t index = static_cast<std::size_t>(std::stoull(index_str));
            if(index >= sizeof...(Types)) {
                throw std::invalid_argument("Invalid variant index: " + index_str + " (expected 0 to " + std::to_string(sizeof...(Types) - 1) + ")");
            }

            fill_by_index_impl(value, index, value_str, std::make_index_sequence<sizeof...(Types)> {});
        } else {
            // Пробуем все типы по очереди
            if(!try_fill_auto_impl(value, str, std::make_index_sequence<sizeof...(Types)> {})) {
                throw std::runtime_error("Failed to parse variant value from string: " + str);
            }
        }
    }

    virtual std::string convert_to_string(const std::variant<Types...>& value) const
    {
        return std::visit([&](const auto& val) -> std::string {
            type_converter<std::decay_t<decltype(val)>> converter;
            std::string result = std::to_string(value.index()) + ":";
            result += converter.convert_to_string(val);
            return result;
        },
            value);
    }
};

#endif // __cplusplus >= 201703L

} // namespace type_converter_api
