#include "entitymenu.hpp"
#include "tilemenu.hpp"
#include "common/engine.hpp"
#include "common/util.hpp"
#include <CASE/timer.hpp>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <memory>

class EntityEdit {
    input::Context editctx;
    input::Context uictx;
    EntityMenu menu;
    Entity entity;
    Engine & engine;

    EntityEdit & operator=(const EntityEdit &) = delete;
    EntityEdit(const EntityEdit &) = delete;

public:
    ~EntityEdit();
    EntityEdit(Engine & engine);
};

class TileEdit {
    input::Context editctx;
    input::Context uictx;
    TileMenu menu;
    Tile tile;
    input::Manager & inputm;

    TileEdit & operator=(const TileEdit &) = delete;
    TileEdit(const TileEdit &) = delete;

public:
    ~TileEdit();
    TileEdit(Engine & engine);
};

int main(int argc, char * argv[]) {
    Engine engine;
    engine.init();

    std::unique_ptr<TileEdit> tile_ed;
    std::unique_ptr<EntityEdit> entity_ed{new EntityEdit{engine}};

    auto globctx = engine.inputm.get_global_context();

    input::Event event{sf::Event::KeyPressed};
    event.set_key(sf::Keyboard::E);
    globctx->bind(event, [&](){
        tile_ed.reset(nullptr);
        entity_ed.reset(new EntityEdit{engine});
    });
    event.set_key(sf::Keyboard::T);
    globctx->bind(event, [&](){
        entity_ed.reset(nullptr);
        tile_ed.reset(new TileEdit{engine});
    });
    event.set_key(sf::Keyboard::N);
    event.set_mod(input::Mod::CTRL, true);
    globctx->bind(event, [&](){
        engine.reset();
        if (entity_ed) {
            entity_ed.reset(new EntityEdit{engine});
        }
        else {
            tile_ed.reset(new TileEdit{engine});
        }
    });

    engine.load(engine.map.filename());
    engine.run();
}

EntityEdit::~EntityEdit() {
    engine.inputm.remove_context(&editctx);
    engine.inputm.remove_context(&uictx);
    //engine.entitym.destroy(entity);
}

EntityEdit::EntityEdit(Engine & engine) 
     : menu(engine.spritef, engine.uirender, engine.entityf, 128, WINH, 1),
     engine(engine)
{
    engine.inputm.push_context(editctx);
    engine.inputm.push_context(uictx);

    auto set_type = [&](type_id_t type){
        entity = engine.entityf.get(type);
        engine.wrender.add(entity.animation.sprite);
    };
    menu.entity_selected.add_callback("set_type", set_type);
    set_type("unit4");

    using namespace input;

    Event syncbrush{sf::Event::MouseMoved};
    editctx.bind(syncbrush, [&](const Event & event){
        auto pos = Position(event.get_mousepos());
        // note: All entities are given an offset, so that when we say 
        // sprite.set_position the actual x,y result is something that makes 
        // sense like the center of a tile or the feet of a unit. That is why 
        // this is required, it's like the mouse cursor's offset.
        pos.y += CELLH / 2;

        Coordinate<CELLW, CELLH> coord{pos};
        coord = coord.to_grid();
        entity.set_cell(coord);

        return false;
    });

    Event edit_entity{sf::Event::MouseButtonPressed};
    edit_entity.set_button(sf::Mouse::Left);
    editctx.bind(edit_entity, [&](const Event & event){
        auto e = engine.entitym.create(entity.get_type());
        e->set_cell(entity.get_cell());
        return true;
    });

    //edit_entity.set_button(sf::Mouse::Right);
    //editctx.bind(edit_entity, [&](const Event & event){
        //return true;
    //});

    uictx.bind(input::Event{sf::Event::MouseMoved}, [&](const Event & event){
        auto p = Position(engine.window.mapCoordsToPixel(event.get_mousepos()));
        menu.update_mousepos(p);
        return menu.contains(p);
    });

    Event clickevnt{sf::Event::MouseButtonPressed};
    clickevnt.set_button(sf::Mouse::Left);
    uictx.bind(clickevnt, [&](const Event & event){
        auto p = Position(engine.window.mapCoordsToPixel(event.get_mousepos()));
        if (menu.contains(p)) {
            menu.try_click(p);
            return true;
        }
        else {
            return false;
        }
    });
}

TileEdit::~TileEdit() {
    inputm.remove_context(&editctx);
    inputm.remove_context(&uictx);
}

TileEdit::TileEdit(Engine & engine)
    : menu(engine.spritef, engine.uirender, engine.tilef, 128, WINH, 2),
     inputm(engine.inputm)
{
    engine.inputm.push_context(editctx);
    engine.inputm.push_context(uictx);
    tile = engine.tilef.get(1);
    menu.tile_selected.add_callback("set_tile_type", [&](tile_id_t type){
        tile = engine.tilef.get(type);
    });

    using namespace input;

    Event synctile{sf::Event::MouseMoved};
    editctx.bind(synctile, [&](const Event & event){
        auto pos = Position(event.get_mousepos());
        // note: All entities are given an offset, so that when we say 
        // sprite.set_position the actual x,y result is something that makes 
        // sense like the center of a tile or the feet of a unit. That is why 
        // this is required, it's like the mouse cursor's offset.
        pos.y += TILEH / 2;
        auto coord = coord_t(pos).to_grid();
        tile.set_coordinate(coord);

        if (engine.inputm.is_button_pressed(sf::Mouse::Left)) {
            engine.map.add_tile(tile.get_id(), coord);
            return true;
        }
        else if (engine.inputm.is_button_pressed(sf::Mouse::Right)) {
            engine.map.remove_tile(coord);
            return true;
        }
        return false;
    });

    Event edit_tile{sf::Event::MouseButtonPressed};
    edit_tile.set_button(sf::Mouse::Left);
    editctx.bind(edit_tile, [&](const Event & event){
        engine.map.add_tile(tile.get_id(), tile.get_coordinate());
        return true;
    });
    edit_tile.set_button(sf::Mouse::Right);
    editctx.bind(edit_tile, [&](const Event & event){
        engine.map.remove_tile(tile.get_coordinate());
        return true;
    });

    uictx.bind(input::Event{sf::Event::MouseMoved}, [&](const Event & event){
        auto p = Position(engine.window.mapCoordsToPixel(event.get_mousepos()));
        menu.update_mousepos(p);
        return menu.contains(p);
    });

    Event clickevnt{sf::Event::MouseButtonPressed};
    clickevnt.set_button(sf::Mouse::Left);
    uictx.bind(clickevnt, [&](const Event & event){
        auto p = Position(engine.window.mapCoordsToPixel(event.get_mousepos()));
        if (menu.contains(p)) {
            menu.try_click(p);
            return true;
        }
        else {
            return false;
        }
    });
}
