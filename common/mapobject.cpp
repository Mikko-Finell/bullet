#include "mapobject.hpp"
#include "util.hpp"
#include <cassert>

namespace map {

void MapObject::serialize(std::ostream & out) const {
    const sf::Vector2f c(coord);
    const int layer = get_layer();
    const bool blocked = is_blocked();

    write(c.x, out);
    write(c.y, out);
    write(layer, out);
    write(blocked, out);

    out << sprite;
}

void MapObject::deserialize(std::istream & in) {
    sf::Vector2f _coord;
    int layer;
    bool blocked;

    read(_coord.x, in);
    read(_coord.y, in);
    read(layer, in);
    read(blocked, in);

    in >> sprite;
    
    set_coordinate(_coord);
    set_layer(layer);
    set_blocked(blocked);
}

const gfx::Sprite & MapObject::get_sprite() const {
    return sprite;
}

MapObject::~MapObject() {
}

MapObject::MapObject(gfx::SpriteManager & spritem) {

    sprite = gfx::Sprite{&spritem};
    sprite.set_origin(-TILEW/2, -TILEH/2);
    sprite.set_size(SPRIW, SPRIH);

    set_layer(z);
    set_blocked(false);
}
            
void MapObject::set_coordinate(const sf::Vector2f & c) {
    coord = c;
    auto pos = util::to_pixel(coord);
    sprite.set_position(pos);
}

void MapObject::set_sprite(const sf::Vector2i & c) {
    sprite.set_spritecoord(c);
}

void MapObject::set_blocked(bool b) {
    blocked = b;
}

void MapObject::move(const sf::Vector2f & offset) {
    set_coordinate(coord + offset);
}

void MapObject::set_layer(int layer) {
    z = layer;
    sprite.set_layer(z);
}

sf::Vector2f MapObject::coordinate() const {
    return coord;
}

sf::Vector2f MapObject::position() const {
    auto pos = util::to_pixel(coord);
    pos.x -= TILEW / 2;
    pos.y -= TILEH / 2;
    return pos;
}

int MapObject::get_layer() const {
    return z;
}

bool MapObject::is_blocked() const {
    return blocked;
}

} // map

