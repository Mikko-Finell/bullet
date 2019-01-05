#include "entitymanager.hpp"
#include <iostream>

// TODO
// entitysystem is currently never used in this class, remove reference?
// animationsystem is currently never used in this class, remove reference?
EntityManager::EntityManager(EntityFactory & ef, EntitySystem & es,
                             RenderSystem & rs, AnimationSystem & as)
    : entityf(ef), entitys(es), render(rs), anims(as)
{
}

Entity * EntityManager::create(const type_id_t & type) {
    entities.emplace_back(entityf.get(type));
    auto entity = &entities.back();
    render.add(entity->animation.sprite);
    std::cout << "EntityManager::create " << entity->info() << std::endl;
    return entity;
}

std::vector<Entity*> EntityManager::create(const type_id_t& type, std::size_t n)
{
    std::vector<Entity *> vec{n};
    assert(vec.size() == n);
    std::generate(vec.begin(), vec.end(), [&](){ return create(type); });
    return vec;
}

// TODO should it error if uid not in manager?
Entity * EntityManager::get(const uid_t & uid) {
    auto cmp = [uid](const Entity & entity){ return entity.get_uid() == uid; };
    auto itr = std::find_if(entities.begin(), entities.end(), cmp);
    if (itr != entities.end()) {
        return &(*itr);
    }
    return nullptr;
}

std::vector<Entity *> EntityManager::get_all() {
    std::vector<Entity *> vec;
    for (auto & entity : entities) {
        vec.push_back(&entity);
    }
    return vec;
}

void EntityManager::destroy(const uid_t & id) {
    auto cmp = [id](const Entity & entity){ return entity.get_uid() == id; };
    auto itr = std::find_if(entities.begin(), entities.end(), cmp);
    if (itr != entities.end()) {
        entities.erase(itr);
    }
}

void EntityManager::clear() {
    entities.clear();
}

void EntityManager::serialize(std::ostream & out) const {
    util::write(entities.size(), out);
    for (auto & entity : entities) {
        util::serialize_std_string(entity.get_type(), out);
        entity.cell.serialize(out);

        util::serialize_std_string(entity.animation.get_current_sequence(), out);
    }
}

void EntityManager::deserialize(std::istream & in) {
    entities.clear();
    decltype(entities.size()) entity_count;
    util::read(entity_count, in);
    for (auto i = 0; i < entity_count; i++) {
        type_id_t type = util::deserialize_std_string(in);
        auto entity = create(type);
        entity->cell.deserialize(in);

        auto sequence = util::deserialize_std_string(in);
        entity->animation.set_sequence(sequence);

        entity->set_cell(entity->cell); // to update sprite pos
    }
}
