#pragma once

#include "column.h"
#include "relationtype.h"
#include "table.h"

#include <cstdint>
#include <ReflectionApi/helper/templates.hpp>
#include <string>

namespace entity_craft {

/**
 * @brief Тип каскадного поведения при операциях с зависимыми сущностями
 */
enum class cascade_behavior : uint8_t
{
    /// Каскадные операции не будут применяться ко всем связанным сущностям.
    none = 1 << 0,
    /// Каскадное сохранение применяется ко всем связанным сущностям.
    persist = 1 << 1,
    /// Каскадное обновление применяется ко всем связанным сущностям.
    merge = 1 << 2,
    /// Каскадное обновление применяется ко всем связанным сущностям. А так же определяет какие сущности были удалены из связанных
    merge_orphan = 1 << 3,
    /// Каскадное удаление применяется ко всем связанным сущностям.
    remove = 1 << 4,
    /// Операция каскадного удаления, сохранения и обновления применяется ко всем связанным сущностям.
    all = 1 << 5,
};

template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
class reference_column final : public column<ClassType, PropertyType, Setter, Getter>
{
public:
    explicit reference_column(const std::string& name,
        reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
        table<ReferencePropertyType, ReferenceColumns...> reference_table,
        relation_type type,
        column_settings settings = column_settings::empty) noexcept;

    explicit reference_column(const std::string& name,
        Setter setter,
        Getter getter,
        table<ReferencePropertyType, ReferenceColumns...> reference_table,
        relation_type type,
        column_settings settings = column_settings::empty) noexcept;

    reference_column(const reference_column& other) = default;
    reference_column(reference_column&& other) noexcept = default;
    reference_column& operator=(const reference_column& other) = default;
    reference_column& operator=(reference_column&& other) noexcept = default;

    table<ReferencePropertyType, ReferenceColumns...> reference_table() const;

    relation_type type() const;

    void append_value(ClassType& obj, const ReferencePropertyType& value) const;

    /**
     * @brief Получить тип каскадного поведения
     * @return Тип каскадного поведения
     */
    cascade_behavior cascade() const;

    /**
     * @brief Установить тип каскадного поведения
     * @param behavior Тип каскадного поведения
     * @return Ссылка на текущий объект для цепочки вызовов
     */
    reference_column set_cascade(cascade_behavior behavior);

    /**
     * @brief Получить имя колонки внешнего ключа
     * @return Имя колонки внешнего ключа или пустая строка, если не установлено
     */
    std::string foreign_key_column() const;

    /**
     * @brief Получить имя колонки первичного ключа связанной таблицы
     * @return Имя колонки первичного ключа или пустая строка, если не установлено
     */
    std::string primary_key_column() const;

    void set_primary_key_column(const std::string& primary_key_column);

private:
    table<ReferencePropertyType, ReferenceColumns...> _reference_table;
    relation_type _type;
    cascade_behavior _cascade_behavior = cascade_behavior::none;
    std::string _foreign_key_column;
    std::string _primary_key_column;
};

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::reference_column(
    const std::string& name,
    const reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type,
    const column_settings settings) noexcept
    : column<ClassType, PropertyType, Setter, Getter>(name, variable, settings)
    , _reference_table(reference_table)
    , _type(type)
{
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::reference_column(
    const std::string& name,
    Setter setter,
    Getter getter,
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type,
    const column_settings settings) noexcept
    : column<ClassType, PropertyType, Setter, Getter>(name, setter, getter, settings)
    , _reference_table(reference_table)
    , _type(type)
    , _foreign_key_column(_reference_table.primary_key_column_name())
{
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
table<ReferencePropertyType, ReferenceColumns...> reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::reference_table() const
{
    return _reference_table;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
relation_type reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::type() const
{
    return _type;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
void reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::append_value(ClassType& obj, const ReferencePropertyType& value) const
{
    set_value(obj, type_converter_api::insert_item(value(obj), value));
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
cascade_behavior reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::cascade() const
{
    return _cascade_behavior;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>
reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::set_cascade(cascade_behavior behavior)
{
    _cascade_behavior = behavior;
    return *this;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
std::string reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::foreign_key_column() const
{
    return _foreign_key_column;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
std::string reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::primary_key_column() const
{
    return _primary_key_column;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
void reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::set_primary_key_column(const std::string& primary_key_column)
{
    _primary_key_column = primary_key_column;
}

template<typename ClassType, typename PropertyType,typename ReferencePropertyType, typename... ReferenceColumns>
auto make_reference_column(const std::string& name,
    const reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type,
    const column_settings settings = column_settings::empty) noexcept
{
    return reference_column<ClassType, 
    PropertyType, 
    reflection_api::helper::Setter_t<ClassType, PropertyType>, 
    reflection_api::helper::ConstGetter_t<ClassType, PropertyType>, 
    ReferencePropertyType,
    ReferenceColumns...>(name, variable, reference_table, type, settings);
}
} // namespace entity_craft