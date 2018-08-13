#include "ui.hpp"
#include "common/input.hpp"
#include <iostream>

static input::Manager inputm;
static float zoomfactor = 1.0f;

UI::UI(sf::RenderWindow & w) : window(&w) {
    inputm.set_window(w);

    using namespace input;
    auto gctx = new Context;
    inputm.push_context(gctx);
    gctx->bind("quit", [&](){ w.close(); });
    Event event{sf::Event::Closed};
    gctx->bind(event, "quit");
    event.set_type(sf::Event::KeyPressed);
    event.set_key(sf::Keyboard::Q);
    gctx->bind(event, "quit");
    auto editctx = new Context;
    inputm.push_context(editctx);
    event.set_mod(Mod::CTRL, true);
    event.set_key(sf::Keyboard::Z);
    editctx->bind(event, [&](){ signal.undo(); });
    event.set_key(sf::Keyboard::S);
    editctx->bind(event, [&](){
        signal.save("");
    });
    event.set_key(sf::Keyboard::L);
    editctx->bind(event, [&](){
        signal.load("");
    });
    event.set_key(sf::Keyboard::N);
    editctx->bind(event, [&](){ signal.newmap(""); });
    auto zoom = [&](float factor){
        auto v = window->getView();
        zoomfactor = factor;
        v.zoom(zoomfactor);
        window->setView(v);
    };
    Event z{sf::Event::KeyPressed};
    z.set_key(sf::Keyboard::PageUp);
    editctx->bind(z, std::bind(zoom, 2.0f));
    z.set_key(sf::Keyboard::PageDown);
    editctx->bind(z, std::bind(zoom, 0.5f));

    z.set_key(sf::Keyboard::G);
    editctx->bind(z, [&](){ signal.toggle_snap(); });
}

sf::Vector2f UI::mouse_pos() {
    auto sfvec = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    return sf::Vector2f{sfvec.x, sfvec.y};
}

bool UI::is_mouse_pressed() {
    return mouse_pressed;
}

void UI::process_input() {
    sf::Event sfevent;
    current_mouse_dt = sf::Vector2f{0,0};

    bool moved = false;
    while (window->pollEvent(sfevent)) {
	switch (sfevent.type) {
	    case sf::Event::MouseButtonPressed:
		continue;
	    case sf::Event::MouseButtonReleased:
		continue;
	    case sf::Event::MouseMoved:
		current_mouse_dt.x += prev_mouse_pos.x - sfevent.mouseMove.x;
		current_mouse_dt.y += prev_mouse_pos.y - sfevent.mouseMove.y;
		prev_mouse_pos.x = sfevent.mouseMove.x;
		prev_mouse_pos.y = sfevent.mouseMove.y;
                moved = true;
		continue;
	default:
                inputm.process_event(sfevent);
		continue;
	}
    }

    auto screen_mouse_pos = sf::Mouse::getPosition();
    auto wpos = window->getPosition();
    auto wsize = window->getSize();
    sf::IntRect wrect(wpos, sf::Vector2i(wsize));

    if (wrect.contains(screen_mouse_pos)) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
            // do nothing
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            signal.paint();
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            signal.erase();
        }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
        auto view = window->getView();
        view.move(zoomfactor * current_mouse_dt);
        window->setView(view);
    }
    // update listeners on current mouse position
    if (moved) {
        signal.update_mousepos(mouse_pos());
    }
}
