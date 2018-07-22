#include <SFML/Graphics.hpp>
#include "helper.hpp"
#include "tile.hpp"

Tile::Tile() {}
Tile::Tile(const sf::Vector2f & coord) : coordinate(coord) {}

Tile Tile::centered_at(const sf::Vector2f & pos) {
    Tile tile;
    tile.center_at(pos);
    return tile;
}

void Tile::center_at(const sf::Vector2f & pos) {
    auto v = pos;
    v.x -= HALFW;
    auto w = snap_to_grid(v);
    coordinate = pixel_to_logic(w);
}

bool Tile::operator==(const Tile & t) const {
    return t.coordinate == coordinate;
}

bool Tile::operator<(const Tile & t) const {
    if (t.coordinate.y == coordinate.y) {
	return t.coordinate.x > coordinate.x;
    }
    else {
	return t.coordinate.y > coordinate.y;
    }
}

void Tile::draw(std::vector<sf::Vertex> & vertices) const {
    static float sprite_x = 0;
    static float sprite_y = 0;
    static float sprite_w = TILEW;
    static float sprite_h = TILEH * 2;

    auto pixel_pos = logic_to_pixel(coordinate);
    auto offset = sprite_h / 4;

    auto topleft = pixel_pos + sf::Vector2f{0, -offset};
    auto topright = pixel_pos + sf::Vector2f{TILEW, -offset};
    auto botright = pixel_pos + sf::Vector2f{TILEW, sprite_h - offset};
    auto botleft = pixel_pos + sf::Vector2f{0, sprite_h - offset};

    // sprite coords
    sf::Vector2f spr_xy{sprite_x, sprite_y};
    auto c_topleft = spr_xy + sf::Vector2f{0, 0};
    auto c_topright = spr_xy + sf::Vector2f{sprite_w, 0};
    auto c_botright = spr_xy + sf::Vector2f{sprite_w, sprite_h};
    auto c_botleft = spr_xy + sf::Vector2f{0, sprite_h};

    vertices.push_back(sf::Vertex{topleft, c_topleft});
    vertices.push_back(sf::Vertex{topright, c_topright});
    vertices.push_back(sf::Vertex{botright, c_botright});
    vertices.push_back(sf::Vertex{botleft, c_botleft});
}
