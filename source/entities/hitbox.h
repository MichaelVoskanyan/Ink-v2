#pragma once
#include <glm/glm.hpp>

class hitbox_t {
public:
  hitbox_t();
  hitbox_t(const glm::vec2 &size, const glm::vec3 &startPos);

  // Update the hitbox position based on entity position (ignoring z)
  void updatePosition(const glm::vec3 &entityPos);

  // Check if this hitbox intersects with another
  bool intersects(const hitbox_t &other) const;

  // Get collision resolution vector (how much to move to resolve collision)
  glm::vec2 getCollisionResolution(const hitbox_t &other) const;

  // Getters
  const glm::vec2 &getPosition() const { return position; }
  const glm::vec2 &getSize() const { return size; }

  // Setters
  void setSize(const glm::vec2 &newSize) { size = newSize; }
  void setActive(bool active) { isActive = active; }
  bool isActive = true; // We can make this settable to disable hitboxes for
                        // certain entities

private:
  glm::vec2 position; // Center position
  glm::vec2 size;     // Width and height
};