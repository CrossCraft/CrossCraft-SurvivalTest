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

    virtual void OnHit(World* w, int damage, glm::vec3 from, bool player);
  protected:
    bool is_falling;
    void test_collide(World *wrld, float dt);
};
} // namespace CrossCraft