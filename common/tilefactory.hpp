#ifndef tilefactory_hpp
#define tilefactory_hpp

#include "tile.hpp"
#include "tilemanager.hpp"
#include "coordinate.hpp"
#include "util.hpp"
#include "spritefactory.hpp"
#include "rendersystem.hpp"
#include <unordered_map>

/**
 * TileFactory
 * Produces tiles from ids.
 * Important note: Tile id are generated automatically so
 * adding new tiles may break existing maps.
 * TODO consider some way to fix this
 */
class TileFactory {
    std::unordered_map<tile_id_t, Tile> tiles;
    std::unordered_map<tile_id_t, Sprite> sprites;
    TileManager & tilem;
    SpriteFactory & spritef;

public:
    TileFactory(TileManager & tm, SpriteFactory & sf);
    Tile * create(RenderSystem & rs, tile_id_t) const;
    std::vector<tile_id_t> get_all() const;
};

#endif
