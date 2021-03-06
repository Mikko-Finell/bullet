#include "entitycursor.hpp"

EntityCursor::~EntityCursor() {
}

EntityCursor::EntityCursor(Engine & engine) : engine(engine) {
    Position center{config::winw/2, config::winh/2};
    Coordinate coord = center.to_logic<>(); //.to_grid();

    cursor = engine.entityf.create(engine.wrender, "unit4");
    cursor.set_coordinate(Coordinate{coord}.to_grid());
}

void EntityCursor::set_entity_type(const Entity::Type & type) {
    const auto coord = cursor.get_coordinate();

    cursor = engine.entityf.create(engine.wrender, type);
    cursor.set_coordinate(coord);
}

void EntityCursor::update_mousepos(const Position & pos) {
    // note: All entities are given an offset, so that when we say 
    // sprite.set_position the actual x,y result is something that makes 
    // sense like the center of a tile or the feet of a unit. That is why 
    // this is required, it's like the mouse cursor's offset.
    const auto y = pos.y + config::cellh / 2;

    Position p{pos.x, y};
    auto coord = Coordinate(p).to_grid();
    cursor.set_coordinate(coord);
}

Entity::Type EntityCursor::get_type() const {
    return cursor.get_type();
}

Coordinate EntityCursor::get_coordinate() const {
    return cursor.get_coordinate();
}
