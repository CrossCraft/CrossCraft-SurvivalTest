#pragma once
#include <cstdint>
#include <glm.hpp>

namespace CrossCraft {
class World;
struct Entity {
    glm::vec3 pos;
    glm::vec2 rot;
    glm::vec3 vel;
    glm::vec3 size;

  protected:
    bool is_falling;
    void test_collide(glm::vec3 testpos, World *wrld, float dt);
};
} // namespace CrossCraft