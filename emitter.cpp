#include "emitter.hpp"
#include "listener.hpp"

void Emitter::emit(Event event) {
    for (auto listener : listeners) {
        listener->recvevent(event);
    }
}

void Emitter::addlistener(Listener * listener) {
    listeners.insert(listener);
    listener->recvevent(Event{Event::RegisteredListener});
}

void Emitter::removelistener(Listener * listener) {
    listeners.erase(listener);
}

Emitter::~Emitter() {
    //sendevent(Event{DeregisteredListener});
}
