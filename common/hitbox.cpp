#include "hitbox.hpp"

Hitbox::Hitbox() {
}

Hitbox::Hitbox(int offset_x, int offset_y, int w, int h) {
    offset = sf::Vector2i{offset_x, offset_y};
    screencoords = sf::IntRect{0, 0, w, h};
}

void Hitbox::set_position(const Position & pos) {
    screencoords.left = pos.x - offset.x;
    screencoords.top = pos.y - offset.y;
}

void Hitbox::serialize(std::ostream & out) const {
    util::write(offset.x, out);
    util::write(offset.y, out);
    util::write(screencoords.left, out);
    util::write(screencoords.top, out);
    util::write(screencoords.width, out);
    util::write(screencoords.height, out);
}

void Hitbox::deserialize(std::istream & in) {
    util::read(offset.x, in);
    util::read(offset.y, in);
    util::read(screencoords.left, in);
    util::read(screencoords.top, in);
    util::read(screencoords.width, in);
    util::read(screencoords.height, in);
}
