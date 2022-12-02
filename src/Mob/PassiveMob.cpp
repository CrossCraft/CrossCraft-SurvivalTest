#include "PassiveMob.hpp"
#include <gtx/vector_angle.hpp>
#include "../Player/Player.hpp"

namespace CrossCraft::Mob {
    void PassiveMob::update(float dt, Player *p, World *w) {
        if (!isAlive)
            return;

        is_falling = true;
        dirChangeTime -= dt;

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

        if (water_cutoff) {
            vel.y = 4.8f;
        }

        if (dirChangeTime < 0.0f) {
            dirChangeTime = 5.0f;
            vel.x = (float) (rand() % 256 - 128) / 256.0f * 2.2f;
            vel.z = (float) (rand() % 256 - 128) / 256.0f * 2.2f;
        }

        auto defaultPoint = glm::vec2(0.0f, 1.0f);
        auto refVec = glm::vec2{(float) vel.x, (float) vel.z};

        auto angle = glm::orientedAngle(defaultPoint, refVec);
        rot.y = 90.0f - angle / 3.14159f * 180.0f;

        if (dirChangeTime < 2.0f && dirChangeTime > 1.5f && !is_falling) {
            vel.y = 5.4f;
        }

        vel.y -= 16.0f * dt;
    }

    void PassiveMob::OnDeath(World *w, bool playerKill) {
        MobData::OnDeath(w, playerKill);

        DropData d;
        d.inRange = false;
        d.animTime = 0.0f;
        d.pos = pos;
        d.size = {0.25f, 0.25f, 0.25f};
        d.vel = {0.0f, 2.0f, 0.0f};
        d.type = Block::Mushroom1;
        d.quantity = rand() % 2 + 1;

        w->drops->add_drop(d);
    }

}// namespace CrossCraft::Mob