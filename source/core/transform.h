#ifndef INK_TRANSFORM_H
#define INK_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct transform_t {
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::quat m_rotation;
};

#endif