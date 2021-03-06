#include "tileedit.hpp"
#include "entityedit.hpp"
#include "common/engine.hpp"
#include <CASE/timer.hpp>

int main(int argc, char * argv[]) {
    Engine engine;

    std::unique_ptr<TileEdit> tileptr{nullptr};
    std::unique_ptr<EntityEdit> entityptr{nullptr};

    input::Event event{sf::Event::KeyPressed};
    event.set_key(sf::Keyboard::T);
    auto globctx = engine.inputm.get_global_context();
    globctx->bind(event, [&](){
        if (tileptr == nullptr) {
            tileptr.reset(new TileEdit{engine});
            entityptr.reset(nullptr);
        }
    });

    event.set_key(sf::Keyboard::E);
    globctx->bind(event, [&](){
        if (entityptr == nullptr) {
            tileptr.reset(nullptr);
            entityptr.reset(new EntityEdit{engine});
        }
    });

    event.set_key(sf::Keyboard::N);
    event.set_mod(input::Mod::CTRL, true);
    globctx->bind(event, [&](){
        engine.tilem.clear();
        engine.entitym.clear();
        engine.camera.focus_at({0, 0});
    });

    std::unordered_map<Coordinate, Entity *, Coordinate::Hash> coord_entityptr_map;
    tileptr.reset(new TileEdit{engine});

    engine.load();
    engine.run();
    engine.save();
}
