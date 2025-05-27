#define GLFW_INCLUDE_NONE
#include "entityManager.h"

EntityManager *EntityManager::instance() {
    static EntityManager s;  // Meyers singleton
    return &s;
}

/*────────────────────────────   update   ────────────────────────────────*/
void EntityManager::update(float dt) {
    /* 1. integrate / animate */
    for (auto &e: m_entities) {
        e->update(dt);
    }

    /* 2. naive O(N²) narrow-phase collision */
    const std::size_t n = m_entities.size();
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            GameObject &a = *m_entities[i];
            GameObject &b = *m_entities[j];

            if (!a.hasCollision() && !b.hasCollision() ||
                (!a.hitbox.intersects(b.hitbox) && !b.hitbox.intersects(a.hitbox))) {
                continue;
            }

            if (a.shouldMoveOnCollision()) {
                a.resolveCollision(&b);
            }
            if (b.shouldMoveOnCollision()) {
                b.resolveCollision(&a);
            }
        }
    }
}

/*─────────────────────────────   draw   ─────────────────────────────────*/
void EntityManager::draw() {
    for (auto &e: m_entities) {
        e->draw();
    }
}
