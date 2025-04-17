#pragma once

#include "sfinae.h"

#include <stdexcept>
#include <vector>

namespace EntityCraft {
namespace Impl {
template<typename Container, typename Type,
    std::enable_if_t<SFINAE::has_emplace_back_v<Container, Type>, bool> = true>
void insertInRelationProperty(Container& relation_property, const std::vector<Type>& result, int)
{
    for(const auto& value : result) {
        relation_property.emplace_back(value);
    }
}

template<typename Container, typename Type>
void insertInRelationProperty(Container&, const std::vector<Type>&, ...)
{
    throw std::runtime_error("insertInRelationProperty not implemented");
}
} // namespace Impl

template<typename Container, typename Type>
class RelationInserter
{
public:
    void insertInRelationProperty(Container& relation_property, const std::vector<Type>& result)
    {
        Impl::insertInRelationProperty(relation_property, result, 0);
    }
};
} // namespace EntityCraft
