#pragma once

#include "helper/sfinae.hpp"
#include "typeconverter.hpp"
#include "utilities.hpp"

#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace type_converter_api {
namespace impl {

/// \brief Вспомогательная функция для резервирования памяти (если доступно)
template<typename Container,
    std::enable_if_t<sfinae::has_reserve_v<Container>, bool> = true>
void reserve_if_available(Container& container, std::size_t size, int)
{
    container.reserve(size);
}

template<typename Container>
void reserve_if_available(Container&, std::size_t, ...)
{
    // Метод reserve недоступен, ничего не делаем
}

/// \brief Вспомогательная функция для очистки контейнера (если доступно)
template<typename Container,
    std::enable_if_t<sfinae::has_clear_v<Container>, bool> = true>
void clear_if_available(Container& container, int)
{
    container.clear();
}

template<typename Container>
void clear_if_available(Container&, ...)
{
    // Метод clear недоступен, ничего не делаем
}

/// \brief Конвертация для Qt-контейнеров с оператором << (одинаковые типы элементов)
template<typename TargetContainer, typename SourceType, typename TargetType, typename CurrentContainer,
    std::enable_if_t<sfinae::has_left_shift_container_operator_v<TargetContainer, TargetType> && std::is_same<SourceType, TargetType>::value, bool> = true>
void convert_to_target_impl(TargetContainer& target, const CurrentContainer& source, int)
{
    reserve_if_available(target, source.size(), 0);

    // Прямая конвертация без изменения типа
    for(const auto& value : source) {
        target << value;
    }
}

/// \brief Конвертация для Qt-контейнеров с оператором << (разные типы элементов, числовые типы)
template<typename TargetContainer, typename SourceType, typename TargetType, typename CurrentContainer,
    std::enable_if_t<sfinae::has_left_shift_container_operator_v<TargetContainer, TargetType> && !std::is_same<SourceType, TargetType>::value && sfinae::is_numeric_v<SourceType> && sfinae::is_numeric_v<TargetType>, bool> = true>
void convert_to_target_impl(TargetContainer& target, const CurrentContainer& source, int)
{
    reserve_if_available(target, source.size(), 0);

    // Прямая конвертация для числовых типов
    for(const auto& value : source) {
        auto converted_value = static_cast<TargetType>(value);
        target << converted_value;
    }
}

/// \brief Конвертация для Qt-контейнеров с оператором << (разные типы элементов, не числовые)
template<typename TargetContainer, typename SourceType, typename TargetType, typename CurrentContainer,
    std::enable_if_t<sfinae::has_left_shift_container_operator_v<TargetContainer, TargetType> && !std::is_same<SourceType, TargetType>::value && (!sfinae::is_numeric_v<SourceType> || !sfinae::is_numeric_v<TargetType>), bool> = true>
void convert_to_target_impl(TargetContainer& target, const CurrentContainer& source, int)
{
    reserve_if_available(target, source.size(), 0);

    // Конвертация с изменением типа элементов через строку
    type_converter<TargetType> converter;
    for(const auto& value : source) {
        TargetType converted_value;
        std::string intermediate = type_converter<SourceType>().convert_to_string(value);
        converter.fill_from_string(converted_value, intermediate);
        target << converted_value;
    }
}

/// \brief Конвертация для последовательных контейнеров (одинаковые типы элементов)
template<typename TargetContainer, typename SourceType, typename TargetType, typename CurrentContainer,
    std::enable_if_t<(sfinae::has_push_back_v<TargetContainer, TargetType> || sfinae::has_insert_v<TargetContainer, TargetType> || sfinae::has_emplace_back_v<TargetContainer, TargetType>) && std::is_same<SourceType, TargetType>::value, bool> = true>
void convert_to_target_impl(TargetContainer& target, const CurrentContainer& source, int)
{
    // Очистка целевого контейнера
    clear_if_available(target, 0);

    // Резервирование памяти для оптимизации
    reserve_if_available(target, source.size(), 0);

    // Прямая конвертация без изменения типа
    for(const auto& value : source) {
        impl::insert_item(target, value, 0);
    }
}

/// \brief Конвертация для последовательных контейнеров (разные типы элементов, числовые типы)
/// Использует прямую конвертацию через static_cast для числовых типов
template<typename TargetContainer, typename SourceType, typename TargetType, typename CurrentContainer,
    std::enable_if_t<(sfinae::has_push_back_v<TargetContainer, TargetType> || sfinae::has_insert_v<TargetContainer, TargetType> || sfinae::has_emplace_back_v<TargetContainer, TargetType>) && !std::is_same<SourceType, TargetType>::value && sfinae::is_numeric_v<SourceType> && sfinae::is_numeric_v<TargetType>, bool> = true>
void convert_to_target_impl(TargetContainer& target, const CurrentContainer& source, int)
{
    // Очистка целевого контейнера
    clear_if_available(target, 0);

    // Резервирование памяти для оптимизации
    reserve_if_available(target, source.size(), 0);

    // Прямая конвертация для числовых типов (более эффективно и правильно)
    for(const auto& value : source) {
        TargetType converted_value = static_cast<TargetType>(value);
        ::type_converter_api::impl::insert_item(target, converted_value, 0);
    }
}

/// \brief Конвертация для последовательных контейнеров (разные типы элементов, не числовые)
/// Использует строковую конвертацию для нечисловых типов
template<typename TargetContainer, typename SourceType, typename TargetType, typename CurrentContainer,
    std::enable_if_t<(sfinae::has_push_back_v<TargetContainer, TargetType> || sfinae::has_insert_v<TargetContainer, TargetType> || sfinae::has_emplace_back_v<TargetContainer, TargetType>) && !std::is_same<SourceType, TargetType>::value && (!sfinae::is_numeric_v<SourceType> || !sfinae::is_numeric_v<TargetType>), bool> = true>
void convert_to_target_impl(TargetContainer& target, const CurrentContainer& source, int)
{
    // Очистка целевого контейнера
    clear_if_available(target, 0);

    // Резервирование памяти для оптимизации
    reserve_if_available(target, source.size(), 0);

    // Конвертация с изменением типа элементов через строку
    type_converter<TargetType> converter;
    for(const auto& value : source) {
        TargetType converted_value;
        std::string intermediate = type_converter<SourceType>().convert_to_string(value);
        converter.fill_from_string(converted_value, intermediate);
        impl::insert_item(target, converted_value, 0);
    }
}

/// \brief Fallback реализация с информативным сообщением об ошибке
template<typename TargetContainer, typename SourceType, typename TargetType, typename CurrentContainer>
void convert_to_target_impl(TargetContainer&, const CurrentContainer&, ...)
{
    std::stringstream message;
    message << "convert_to_target not implemented: cannot convert from "
            << typeid(CurrentContainer).name() << " (value_type: " << typeid(SourceType).name() << ") "
            << "to " << typeid(TargetContainer).name() << " (value_type: " << typeid(TargetType).name() << ")";
    throw std::runtime_error(message.str());
}

} // namespace impl

/// \brief Класс для конвертации между различными типами контейнеров
/// \tparam TargetContainer Тип целевого контейнера
/// \tparam TargetType Тип элементов целевого контейнера (по умолчанию определяется автоматически)
///
/// Этот класс предоставляет функциональность для конвертации данных из одного контейнера
/// в другой, включая поддержку конвертации типов элементов.
///
/// \example
/// \code
/// // Конвертация vector<int> в list<int>
/// std::vector<int> source = {1, 2, 3};
/// std::list<int> target;
/// container_converter<std::list<int>> converter;
/// converter.convert_to_target(target, source);
///
/// // Конвертация vector<int> в vector<long> с изменением типа
/// std::vector<int> source = {1, 2, 3};
/// std::vector<long> target;
/// container_converter<std::vector<long>> converter;
/// converter.convert_to_target(target, source);
/// \endcode
template<typename TargetContainer, typename TargetType = typename TargetContainer::value_type>
class container_converter
{
public:
    /// \brief Конвертирует данные из исходного контейнера в целевой
    /// \tparam CurrentContainer Тип исходного контейнера (по умолчанию std::vector<SourceType>)
    /// \param target Ссылка на целевой контейнер
    /// \param source Исходный контейнер для конвертации
    ///
    /// Метод автоматически определяет тип элементов исходного контейнера и выполняет
    /// конвертацию с использованием type_converter для каждого элемента, если типы различаются.
    ///
    /// Поддерживаемые методы вставки (в порядке приоритета):
    /// - push_back (для последовательных контейнеров)
    /// - insert (для ассоциативных контейнеров)
    /// - emplace_back (для контейнеров с поддержкой emplace)
    /// - Qt оператор << (если USE_TYPE_QT определен)
    ///
    /// \throws std::runtime_error если конвертация невозможна
    template<typename CurrentContainer = std::vector<TargetType>>
    void convert_to_target(TargetContainer& target, const CurrentContainer& source)
    {
        using SourceType = typename CurrentContainer::value_type;
        impl::convert_to_target_impl<TargetContainer, SourceType, TargetType, CurrentContainer>(target, source, 0);
    }

    /// \brief Конвертирует данные из исходного контейнера и возвращает новый контейнер
    /// \tparam CurrentContainer Тип исходного контейнера
    /// \param source Исходный контейнер для конвертации
    /// \return Новый целевой контейнер с конвертированными данными
    ///
    /// \example
    /// \code
    /// std::vector<int> source = {1, 2, 3};
    /// auto target = container_converter<std::list<int>>().convert(source);
    /// \endcode
    template<typename CurrentContainer>
    TargetContainer convert(const CurrentContainer& source)
    {
        TargetContainer target;
        convert_to_target(target, source);
        return target;
    }
};

} // namespace type_converter_api
