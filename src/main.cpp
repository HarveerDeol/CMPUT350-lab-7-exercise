#include <cassert>
#include <iostream>
#include <string>

#include "entity_manager.h"
#include "registry.h"

int main() {
    Registry<std::string> r;
    size_t a = r.store(std::string("a"));
    size_t b = r.store(std::string("b"));
    assert(a == 0 && b == 1 && r.size() == 2);
    r.remove(a);
    assert(!r.valid(a) && r.size() == 1);
    assert(r.store(std::string("c")) == a); 
    assert(r[a] == "c");

    EntityManager<int, float, bool> em;
    EntityID e = em.createEntity();
    em.addComponent(e, 5);
    assert(em.hasComponent<int>(e));
    assert(!em.hasComponent<float>(e));
    assert(!em.hasComponent<bool>(e));
    assert(em.getComponent<int>(e) == 5);

    std::cout << "All tests passed\n";
    return 0;
}