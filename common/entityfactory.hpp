#ifndef entityfactory_hpp
#define entityfactory_hpp

#include "entity.hpp"
#include "entitymanager.hpp"
#include "spritefactory.hpp"
#include "animationfactory.hpp"
#include "rendersystem.hpp"
#include "util.hpp"
#include <unordered_map>

/**
 * EntityFactory
 * Holds all available entities.
 */
class EntityFactory {
    AnimationFactory & animf;
    EntityManager & entitym;
    EntitySystem & entitys;

    mutable uid_t next_id = 0;
    std::unordered_map<type_id_t, Entity> entities;

public:
    EntityFactory(AnimationFactory & af, EntityManager & em, EntitySystem & es);
    
    // create an entity of type.
    Entity * create(RenderSystem & rs, const type_id_t & type) const;

    // get a list of all available types of entities
    std::vector<type_id_t> get_all_types() const;
};

#endif
