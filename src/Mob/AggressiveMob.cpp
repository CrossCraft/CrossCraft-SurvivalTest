#include "AggressiveMob.hpp"
#include "../Player/Player.hpp"
#include <gtx/vector_angle.hpp>
namespace CrossCraft {
void AggressiveMob::update(float dt, Player *p, World *w) {
    is_falling = true;

    auto blk = w->worldData[w->getIdx(pos.x, pos.y - 1.2f, pos.z)];
    bool water_cutoff = false;
    if (blk == 8 || blk == 10)
        water_cutoff = true;
    else
        water_cutoff = false;

    test_collide(w, dt);

    auto vel2 = vel;
    if (water_cutoff) {
        vel.x *= 0.5f;
        vel.z *= 0.5f;
        vel.y *= 0.5f;
    }
    pos += vel * dt;
    vel = vel2;

    auto ppos = glm::vec2{p->pos.x, p->pos.z};
    auto cpos = glm::vec2{pos.x, pos.z};

    auto diff = ppos - cpos;
    auto len = sqrtf(diff.x * diff.x + diff.y * diff.y);

    auto yDiff = p->pos.y - pos.y;

    if (water_cutoff) {
        vel.y = 4.8f;
    }

    auto vSpeed = 2.3f;
    if (mobType == MobType::Skeleton) {
        vSpeed = 2.0f;
    } else if (mobType == MobType::Creeper) {
        vSpeed = 1.8f;
    }

    if (len < 24.0f && len > 1.0f) {
        diff /= len;
        vel.x = diff.x * 0.5f * vSpeed;
        vel.z = diff.y * 0.5f * vSpeed;

        auto defaultPoint = glm::vec2(0.0f, 1.0f);
        auto refVec = diff;

        auto angle = glm::orientedAngle(defaultPoint, refVec);
        rot.y = 90.0f - angle / 3.14159f * 180.0f;

        if ((int)yDiff > 0 && !is_falling && yDiff < 7.0f && !water_cutoff) {
            vel.y = 6.9f;
        }
    }

    vel.y -= 16.0f * dt;
}
} // namespace CrossCraft