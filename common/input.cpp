#include "input.hpp"
#include "sfml.hpp"
#include "stl.hpp"

namespace input {

hash_t Event::compute_hash() const {
    hash_t _hash = 1000000 * type;
    switch (type) {
        case sf::Event::KeyPressed:
            _hash += key;
            break;
        case sf::Event::KeyReleased:
            _hash += key + 100000;
            break;
        case sf::Event::MouseButtonPressed:
            _hash += button + 200000;
            break;
        case sf::Event::MouseButtonReleased:
            _hash += button + 300000;
            break;
        default:
            break;
    }
    _hash += mod[Mod::CTRL] * 10000;
    _hash += mod[Mod::SHIFT] * 20000;
    _hash += mod[Mod::ALT] * 30000;
    return _hash;
}

Event::Event() {
}

Event::Event(int t) {
    set_type(t);
}

Event::Event(const sf::Event & sfevent) {
    set_type(sfevent.type);
    bool checkmod = false;
    switch (type) {
        case sf::Event::KeyPressed:
            key = sfevent.key.code;
            checkmod = true;
            break;
        case sf::Event::KeyReleased:
            key = sfevent.key.code;
            checkmod = true;
            break;
        case sf::Event::MouseButtonPressed:
            button = sfevent.mouseButton.button;
            break;
        case sf::Event::MouseButtonReleased:
            button = sfevent.mouseButton.button;
            break;
        case sf::Event::MouseWheelScrolled:
            if (sfevent.mouseWheelScroll.delta > 0) {
                scroll = 1;
            }
            else {
                scroll = -1;
            }
            break;
        default:
            break;
    }
    if (checkmod) {
        mod[Mod::CTRL] = sfevent.key.control;
        mod[Mod::SHIFT] = sfevent.key.shift;
        mod[Mod::ALT] = sfevent.key.alt;
    }
    hash = compute_hash();
}

void Event::set_type(int t) {
    type = t;
    hash = compute_hash();
}

void Event::set_key(int k) {
    key = k;
    hash = compute_hash();
}

void Event::set_button(int b) {
    button = b;
    hash = compute_hash();
}

void Event::set_mod(Mod m, bool b) {
    mod.set(m, b);
    hash = compute_hash();
}

bool Event::operator==(const Event & other) const {
    return this->get_hash() == other.get_hash();
}

hash_t Event::get_hash() const {
    return hash;
}

const Position & Event::get_mousepos_screen() const { 
    return mousepos_screen;
}

const Position Event::get_mousepos_pixel() const { 
    return mousepos_pixel;
}

const Coordinate & Event::get_mousepos_logic() const {
    return mousepos_logic;
}

sf::Vector2i Event::get_mousedt() const {
    return mousedt;
}

int Event::get_scroll() const {
    return scroll;
}

void Event::set_mousepos_screen(const Position & p) {
    mousepos_screen = p;
}

void Event::set_mousepos_pixel(const Position & p) {
    mousepos_pixel = p;
}

void Event::set_mousepos_logic(const Coordinate & c) {
    mousepos_logic = c;
}

void Event::set_mousedt(const sf::Vector2i & v) {
    mousedt = v;
}

// Manager //////////////////////////////////////////////////////////////////////

Manager::~Manager() {
    for (Context * ctx : contexts) {
        if (ctx != nullptr) {
            ctx->set_manager(nullptr);
        }
    }
}

Manager::Manager(sf::RenderWindow & w) : globctx(std::make_unique<input::Context>())
{
    contexts.push_back(globctx.get());
    globctx->set_manager(this);
    set_window(w);
}

void Manager::set_window(sf::RenderWindow & win) {
    sfwin = &win;
}

void Manager::process_event(const sf::Event & sfevent) {
    switch (sfevent.type) {
        case sf::Event::KeyPressed:
            key_down[sfevent.key.code] = true;
            break;
        case sf::Event::KeyReleased:
            key_down[sfevent.key.code] = false;
            break;
        case sf::Event::MouseButtonPressed:
            button_down[sfevent.mouseButton.button] = true;
            break;
        case sf::Event::MouseButtonReleased:
            button_down[sfevent.mouseButton.button] = false;
            break;
        case sf::Event::MouseMoved:
            mouse_dt.x = sfevent.mouseMove.x - mouse_pos.x;
            mouse_dt.y = sfevent.mouseMove.y - mouse_pos.y;
            mouse_pos.x = sfevent.mouseMove.x;
            mouse_pos.y = sfevent.mouseMove.y;
        default:
            break;
    }

    assert(sfwin);
    Event arg{sfevent};

    arg.set_mousedt(mouse_dt);
    arg.set_mousepos_screen(mouse_pos);

    Position pos{sfwin->mapPixelToCoords(sf::Vector2i(mouse_pos))};
    arg.set_mousepos_pixel(pos);
    // note: All entities are given an offset, so that when we say 
    // sprite.set_position the actual x,y result is something that makes 
    // sense like the center of a tile or the feet of a unit. That is why 
    // this is required, it's like the mouse cursor's offset.
    pos.y += config::cellh / 2;
    arg.set_mousepos_logic(pos);

    // contexts can be created inside event propagation loop, so
    // add new contexts from previous update first
    for (auto ctx : context_queue) {
        contexts.push_front(ctx);
        ctx->set_manager(this);
    }
    context_queue.clear();

    auto itr = contexts.begin();
    while (itr != contexts.end()) {
        Context * context = *itr;

        // Contexts can be removed in response to an event, in that case
        // it is replaced by nullptr, so we erase any such entries in the list
        if (context == nullptr) {
            itr = contexts.erase(itr);
        }
        else {
            // context signals that event was consumed by returning true
            if (context->execute(arg)) {
                break;
            }
            ++itr;
        }
    }
}

void Manager::poll_sfevents() {
    assert(sfwin);
    sf::Event sfevent;
    while (sfwin->pollEvent(sfevent)) {
        process_event(sfevent);
    }
}

void Manager::push_context(Context * context) {
    context_queue.push_back(context);
}

void Manager::push_context(Context & context) {
    push_context(&context);
}

// context can be removed during stack traversal, so we just replace
// it with nullptr since we don't manage context lifetime here.
void Manager::remove_context(Context * context) {
    for (auto & c : contexts) {
        if (c == context) {
            c = nullptr;
        }
    }
}

void Manager::pop_context() {
    if (contexts.empty() == false) {
        remove_context(contexts.front());
    }
}

Context * Manager::get_global_context() {
    assert(contexts.empty() == false);
    return globctx.get();
}

void Manager::create_action(const std::string & name, const Callback & callback)
{
    name_to_callback[name] = callback;
}

void 
Manager::create_action(const std::string & n, const std::function<void()> & fn){
    create_action(n, [fn](const Event &){ fn(); return true; });
}

std::optional<Callback> Manager::get_action(const std::string & name) {
    if (auto itr = name_to_callback.find(name); itr != name_to_callback.end()) {
        return (*itr).second;
    }
    return {};
    //return std::nullopt;
}

sf::Vector2f Manager::get_mousepos() {
    auto pos = sfwin->mapPixelToCoords(sf::Mouse::getPosition(*sfwin));
    return pos;
}

// Context //////////////////////////////////////////////////////////////////////

Context::~Context() {
    if (manager != nullptr) {
        remove_manager();
    }
}

void Context::set_manager(Manager * m) {
    manager = m;
}

void Context::remove_manager() {
    assert(manager != nullptr);
    manager->remove_context(this);
    manager = nullptr;
}

// iterate our callbacks, until event is consumed.
// return true if event was consumed otherwise false
bool Context::execute(const Event & arg) {
    auto itr = event_to_callback.find(arg);
    if (itr != event_to_callback.end()) {
        auto action = itr->second;
        if (action(arg)) { // actions can pass event through by return false
            return true;
        }
    }
    auto itr1 = event_to_name.find(arg);
    if (itr1 != event_to_name.end()) {
        auto & name = itr1->second;
        return execute(name);
    }
    return false;
}

// execute the first callback that is bound to name.
bool Context::execute(const std::string & name) {
    assert(manager);
    // named callbacks do not check events so we just pass dummy event as param
    if (auto itr = name_to_callback.find(name); itr != name_to_callback.end()) {
        auto & callback = itr->second;
        if (callback(Event{})) {
            return true;
        }
    }
    if (auto action = manager->get_action(name)) {
        // dereference optional
        if ((*action)(Event{})) {
            return true;
        }
    }
    return false;
}

void Context::bind(const Event & event, const Callback & callback) {
    event_to_callback[event] = callback;
}

void Context::bind(const Event & event, const std::function<void()> & fn) {
    bind(event, [fn](const Event &){ fn(); return true; });
}

void Context::bind(const Event & event, const std::string & name) {
    event_to_name[event] = name;
}

void Context::bind(const std::string & name, const Callback & callback) {
    name_to_callback[name] = callback;
}

void Context::bind(const std::string & name, const std::function<void()> & fn) {
    name_to_callback[name] = [fn](const Event &){ fn(); return true; };
}

bool Context::is_button_pressed(sf::Mouse::Button button) {
    if (manager == nullptr) {
        throw std::logic_error{"Use context.is_button_pressed with no manager."};
    }
    else {
        return manager->is_button_pressed(button);
    }
}

bool Context::is_key_pressed(sf::Keyboard::Key key) {
    if (manager == nullptr) {
        throw std::logic_error{"Use context.is_key_pressed with no manager."};
    }
    else {
        return manager->is_key_pressed(key);
    }
}

} // input
