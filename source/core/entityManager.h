#pragma once
#define GLFW_INCLUDE_NONE
#include "./entities/gameObject.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <type_traits>
#include <vector>

/**
 * Central registry that owns every live gameObject_t instance,
 * updates them once per frame, resolves pair-wise collisions,
 * and then draws them.
 *
 * Usage:
 *   auto& em = entityManager_t::instance();
 *   em.add<character_t>(args…);        // spawn something
 *   em.update(dt);                     // per-frame logic
 *   em.draw();                         // per-frame render
 */
class EntityManager {
public:
    /*───── singleton access ───────────────────────────────────────────────*/
    static EntityManager *instance();

    /*───── factory helper; hides std::make_unique ─────────────────────────*/
    template <class T, class... Args>
    std::shared_ptr<T> add(Args &&...args);

    /*───── per-frame hooks ────────────────────────────────────────────────*/
    void update(float dt);
    void draw();

    /*───── rule of five: keep singleton unique ───────────────────────────*/
    EntityManager(const EntityManager &) = delete;
    EntityManager &operator=(const EntityManager &) = delete;
    EntityManager(EntityManager &&) = delete;
    EntityManager &operator=(EntityManager &&) = delete;

    /*───── accessors ─────────────────────────────────────────────────────*/
    const std::vector<std::shared_ptr<GameObject>> &getEntities() const {
        return m_entities;
    }

private:
    EntityManager() = default;
    ~EntityManager() = default;

    std::vector<std::shared_ptr<GameObject>> m_entities;
};

/*───────────────────────────── template impls ─────────────────────────────*/
template <class T, class... Args>
std::shared_ptr<T> EntityManager::add(Args &&...args) {
    static_assert(std::is_base_of_v<GameObject, T>, "T must derive from gameObject_t");

    auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
    m_entities.emplace_back(ptr);
    return ptr;
}
