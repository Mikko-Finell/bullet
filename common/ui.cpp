#include "ui.hpp"

namespace UI {

Element::Element() {
}

Element::~Element() {
}

void Element::activate() {
    activated = true;
}

void Element::update_mousepos(const Position & pos) {
    hovering = contains(pos);
}

void Element::click(const Position & pos) {
    activated = contains(pos);
    if (activated) {
        activate();
    }
}

bool Element::contains(const Position & pos) {
    return false;
}

// Container /////////////////////////////////////////////////////////////////////////////

void Container::cleanup() {
}

Container::~Container() {
    cleanup();
}

Container::Container(Engine & eng) : engine(eng) {
}

void Container::add_element(Element * element) {
    element_ptrs.push_back(element);
}

void Container::update_mousepos(const Position & pos) {
    for (auto p : element_ptrs) {
        p->update_mousepos(pos);
    }
}

bool Container::contains(const Position & pos) {
    return background.get_screencoords().contains(pos);
}

bool Container::click(const Position & pos) {
    if (contains(pos)) {
        for (auto & e : elements) {
            e.click(pos);
        }
        for (auto eptr : element_ptrs) {
            eptr->click(pos);
        }
    }
    return contains(pos);
}

} // UI
