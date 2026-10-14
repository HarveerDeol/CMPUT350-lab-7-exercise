#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H


#include <cstddef>  // size_t

#include "pack.h"  // Pack from prep

#include <array>
#include <cassert>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "registry.h"

using EntityID = size_t;

template <typename... ComponentTypes>
class EntityManager {
   public:

    EntityID createEntity() {
        EntityID id;
        if (!freeEntities_.empty()) {
            id = freeEntities_.back();
            freeEntities_.pop_back();
            aliveEntities_[id] = true;
            entityToComponent_[id] = emptyRow(); 
        } else {
            id = aliveEntities_.size();
            aliveEntities_.push_back(true);
            entityToComponent_.push_back(emptyRow());
        }
        return id;
    }

    bool validEntity(EntityID id) const {
        return id < aliveEntities_.size() && aliveEntities_[id];
    }

    template <typename ComponentType>
    bool hasComponent(EntityID id) const {
        constexpr size_t typeIdx = componentTypeIndex<ComponentType>();
        assert(validEntity(id));
        return entityToComponent_[id][typeIdx] != INVALID_COMPONENT_INDEX;
    }

    template <typename ComponentType>
    ComponentType& getComponent(EntityID id) {
        constexpr size_t typeIdx = componentTypeIndex<ComponentType>();
        assert(hasComponent<ComponentType>(id));  
        return std::get<typeIdx>(registries_)[entityToComponent_[id][typeIdx]];
    }

    template <typename ComponentType>
    const ComponentType& getComponent(EntityID id) const {
        constexpr size_t typeIdx = componentTypeIndex<ComponentType>();
        assert(hasComponent<ComponentType>(id));
        return std::get<typeIdx>(registries_)[entityToComponent_[id][typeIdx]];
    }

    template <typename ComponentType>
    void addComponent(EntityID id, ComponentType&& component) {
        using Stored = std::remove_cvref_t<ComponentType>;
        constexpr size_t typeIdx = componentTypeIndex<Stored>();
        assert(validEntity(id));
        assert(!hasComponent<Stored>(id));
        entityToComponent_[id][typeIdx] =
            std::get<typeIdx>(registries_).store(std::forward<ComponentType>(component));
    }

   private:
    static constexpr size_t NUM_COMPONENT_TYPES = sizeof...(ComponentTypes);
    static constexpr size_t INVALID_COMPONENT_INDEX = std::numeric_limits<size_t>::max();

    using Row = std::array<size_t, NUM_COMPONENT_TYPES>;

    static constexpr Row emptyRow() {
        Row row{};
        row.fill(INVALID_COMPONENT_INDEX);
        return row;
    }

    template <typename ComponentType>
    static constexpr size_t componentTypeIndex() {
        static_assert(Pack<ComponentTypes...>::template contains<ComponentType>,
                      "Not a component type of this EntityManager.");
        return static_cast<size_t>(Pack<ComponentTypes...>::template index<ComponentType>);
    }

    std::tuple<Registry<ComponentTypes>...> registries_;

    std::vector<Row> entityToComponent_;     
    std::vector<bool> aliveEntities_;      
    std::vector<EntityID> freeEntities_;    
};

#endif  // ENTITY_MANAGER_H