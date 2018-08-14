#ifndef __MAP__
#define __MAP__

#include "tile.hpp"
#include "map.hpp"
#include "common/observer.hpp"
#include <vector>
#include <array>

class Map {
    std::vector<Tile> tiles;
    gfx::SpriteManager & spritem;
    //int width = 0, height = 0;

public:
    struct {
        Signal<int, int> map_loaded;
    } signal;

    Map(gfx::SpriteManager & spritem);

    void load(const std::string & mapname);
    void create_graph();

    Tile * const get_tile(const sf::Vector2f & coord);
    std::vector<Tile*> section(const sf::FloatRect & rect);
    std::array<Tile *, DIRECTIONS> neighbors(Tile * center);
    //std::pair<int, int> size() const;
    
    Cell * get_cell(int x, int y);
};

#endif
