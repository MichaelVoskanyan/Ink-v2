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
class entityManager_t {
public:
    /*───── singleton access ───────────────────────────────────────────────*/
    static entityManager_t *instance();

    /*───── factory helper; hides std::make_unique ─────────────────────────*/
    template <class T, class... Args>
    std::shared_ptr<T> add(Args &&...args);

    /*───── per-frame hooks ────────────────────────────────────────────────*/
    void update(float dt);
    void draw();

    /*───── rule of five: keep singleton unique ───────────────────────────*/
    entityManager_t(const entityManager_t &) = delete;
    entityManager_t &operator=(const entityManager_t &) = delete;
    entityManager_t(entityManager_t &&) = delete;
    entityManager_t &operator=(entityManager_t &&) = delete;

    /*───── accessors ─────────────────────────────────────────────────────*/
    const std::vector<std::shared_ptr<gameObject_t>> &getEntities() const {
        return m_entities;
    }

private:
    entityManager_t() = default;
    ~entityManager_t() = default;

    std::vector<std::shared_ptr<gameObject_t>> m_entities;
};

/*───────────────────────────── template impls ─────────────────────────────*/
template <class T, class... Args>
std::shared_ptr<T> entityManager_t::add(Args &&...args) {
    static_assert(std::is_base_of_v<gameObject_t, T>, "T must derive from gameObject_t");

    auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
    m_entities.emplace_back(ptr);
    return ptr;
}
