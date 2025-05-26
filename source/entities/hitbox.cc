#include "hitbox.h"
#include <algorithm>
#include <iostream>

hitbox_t::hitbox_t() {}

hitbox_t::hitbox_t(const glm::vec2 &size, glm::vec3 &position)
    : size(size), position(position) {
  std::cout << "[hitbox] Created with size: (" << size.x << ", " << size.y
            << ")" << std::endl;
  std::cout << "[hitbox] Created with position: (" << position.x << ", "
            << position.y << ")" << std::endl;
}

void hitbox_t::updatePosition(const glm::vec3 &entityPos) {
  position = entityPos;
  std::cout << "[hitbox] Position updated to: (" << position.x << ", "
            << position.y << ")" << std::endl;
}

bool hitbox_t::intersects(const hitbox_t &other) const {
  if (!isActive || !other.isActive)
    return false;

  // AABB intersection test
  float left1 = position.x - size.x / 2;
  float right1 = position.x + size.x / 2;
  float top1 = position.y + size.y / 2;
  float bottom1 = position.y - size.y / 2;

  float left2 = other.position.x - other.size.x / 2;
  float right2 = other.position.x + other.size.x / 2;
  float top2 = other.position.y + other.size.y / 2;
  float bottom2 = other.position.y - other.size.y / 2;

  const bool isIntersecting =
      !(left1 > right2 || right1 < left2 || top1 < bottom2 || bottom1 > top2);

  std::cout << "[hitbox] Collision check:" << std::endl;
  std::cout << "  Box 1: pos=(" << position.x << "," << position.y << ") size=("
            << size.x << "," << size.y << ")" << std::endl;
  std::cout << "  Box 2: pos=(" << other.position.x << "," << other.position.y
            << ") size=(" << other.size.x << "," << other.size.y << ")"
            << std::endl;
  std::cout << "  Intersecting: " << (isIntersecting ? "YES" : "NO")
            << std::endl;

  return isIntersecting;
}

glm::vec2 hitbox_t::getCollisionResolution(const hitbox_t &other) const {

  // Calculate overlap in each direction
  float left1 = position.x - size.x / 2;
  float right1 = position.x + size.x / 2;
  float top1 = position.y + size.y / 2;
  float bottom1 = position.y - size.y / 2;

  float left2 = other.position.x - other.size.x / 2;
  float right2 = other.position.x + other.size.x / 2;
  float top2 = other.position.y + other.size.y / 2;
  float bottom2 = other.position.y - other.size.y / 2;

  // Calculate penetration depths
  float overlapLeft = right1 - left2;
  float overlapRight = right2 - left1;
  float overlapTop = top2 - bottom1;
  float overlapBottom = top1 - bottom2;

  // Find minimum penetration
  float minOverlap =
      std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});

  glm::vec2 resolution;
  // Return resolution vector based on minimum penetration
  if (minOverlap == overlapLeft) {
    resolution = glm::vec2(-overlapLeft, 0.0f);
  } else if (minOverlap == overlapRight) {
    resolution = glm::vec2(overlapRight, 0.0f);
  } else if (minOverlap == overlapTop) {
    resolution = glm::vec2(0.0f, overlapTop);
  } else { // overlapBottom
    resolution = glm::vec2(0.0f, -overlapBottom);
  }

  std::cout << "[hitbox] Resolution vector: (" << resolution.x << ", "
            << resolution.y << ")" << std::endl;
  return resolution;
}