#include "map.hpp"
#include "entity.hpp"
#include "common/input.hpp"
#include "common/animation.hpp"
#include <iostream>

void init(sf::RenderWindow & window, input::Manager & inputm,
          gfx::SpriteManager & spritem, gfx::AnimationManager & anim, Map & map);

int main() {
    sf::RenderWindow window;
    input::Manager inputm{window};
    gfx::SpriteManager spritem;
    gfx::AnimationManager anim{spritem};
    Map map{spritem};
    init(window, inputm, spritem, anim, map);

    auto view = window.getView();
    view.zoom(0.5f);
    window.setView(view);
    map.load("tmp.bulletmap");

    Entity entity;
    entity.animation = anim.get("test");
    entity.animation.set_sequence("idle-down");
    entity.animation.sprite.set_origin(-23, -38);
    entity.set_cell(map.get_cell(0, 0));

    while (window.isOpen()) {
        inputm.poll_sfevents();

        entity.update(16);

        window.clear(sf::Color::White);
        spritem.draw(window);
        window.display();
    }
}

void init(sf::RenderWindow & window, input::Manager & inputm,
          gfx::SpriteManager & spritem, gfx::AnimationManager & anim, Map & map)
{
    window.create(sf::VideoMode{WINW, WINH}, "Bullet Broodwar");
    window.setFramerateLimit(60);
    //window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    util::center_window(window);

    inputm.create_action("quit", [&](){ window.close(); });

    static input::Context gctx;
    inputm.push_context(gctx);
    gctx.bind(input::Event{sf::Event::Closed}, "quit");

    input::Event keyp{sf::Event::KeyPressed};
    keyp.set_key(sf::Keyboard::Q);
    gctx.bind(keyp, "quit");

    spritem.texture.loadFromFile("../sprites/sprites.png");

    map.signal.map_loaded.add_callback([&](int w, int h){
        const sf::Vector2f v(w * 0.5f, h * 0.5f);
        auto u = util::to_pixel(v);
        auto view = window.getView();
        view.setCenter(u.x, u.y);
        window.setView(view);
    });

    auto scroll = [&](const input::Event & event){
        if (inputm.is_button_pressed(sf::Mouse::Middle)) {
            auto view = window.getView();
            view.move(sf::Vector2f(event.get_mousedt()));
            window.setView(view);
            return true;
        }
        return false;
    };
    gctx.bind(input::Event{sf::Event::MouseMoved}, scroll);

    static gfx::Sprite hlsprite{&spritem};
    hlsprite.set_spritecoord({128, 128});
    hlsprite.set_layer(2);
    hlsprite.set_visible(false);

    // highlight tile from mouse movement
    auto hl = [&](const input::Event & event) -> bool {
        sf::Vector2i mousepos = event.get_mousepos();
        sf::Vector2f coordpos = window.mapPixelToCoords(sf::Vector2i(mousepos));
        sf::Vector2f logicpos = util::to_grid(coordpos);
        if (auto tile = map.get_tile(logicpos); tile != nullptr) {
            hlsprite.set_visible(true);
            hlsprite.set_position(tile->position());
        }
        else {
            hlsprite.set_visible(false);
        }
        return false;
    };

    static input::Context tilectx;
    tilectx.bind(input::Event{sf::Event::MouseMoved}, hl);
    inputm.push_context(tilectx);
}
