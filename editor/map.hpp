#ifndef map_bpp
#define map_bpp

#include "tile.hpp"
#include "common/observer.hpp"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

class Map : public Observer {
    const std::string extension = ".bulletmap";
    const std::string mapdir = "../maps/";
    std::string name = "tmp";
    std::vector<Tile> tiles;
    RenderSystem & render;

public:
    struct {
        Signal<float,float> map_loaded;
    } signal;

    inline std::string filename() const {
        return mapdir + name + extension;
    }

    Map(RenderSystem & rs);

    void undo();
    void create(const Tile & tile);
    void remove(const sf::Vector2f & coord);

    void on_new(const std::string & s);
    void on_save(const std::string & s);
    void on_load(const std::string & s);
    void on_setname(const std::string & s);
};

#endif
