#include "SkeletonData.hpp"
#include "../Entity/Arrow.hpp"

namespace CrossCraft::Mob {
void SkeletonData::update(float dt, Player *p, World *w) {

    AggressiveMob::update(dt, p, w);

    if (!isAlive)
        return;

    auto diff = p->pos - pos;
    auto len = sqrtf(diff.x * diff.x + diff.z * diff.z);

    if (len < 8.0f) {
        fireTime -= dt;

        if (fireTime < 0) {
            fireTime = 1.5f;

            // Spawn arrow
            ArrowData aData;
            aData.inRange = false;
            aData.pos = pos + glm::vec3(diff.x / 8.0f, 0.0f, diff.y / 8.0f);
            aData.pos.y -= (1.80f - 1.5965f);
            aData.rot = {-rot.x, -rot.y - 90.0f};
            aData.size = {0.1f, 0.0f, 0.1f};
            aData.lifeTime = 15.0f;
            aData.playerArrow = false;

            aData.vel = glm::normalize(diff) * 16.0f;
            w->arrow->add_arrow(aData);
        }
    }
}

void SkeletonData::OnDeath(World *w, bool playerKill) {
    MobData::OnDeath(w, playerKill);

    for (int i = 0; i < 6 + rand() % 3; i++) {
        ArrowData aData;
        aData.inRange = false;
        aData.pos = pos;
        aData.pos.y -= 1.8f;
        aData.rot = {rand() % 180 - 90, rand() % 360};
        aData.size = {0.1f, 0.0f, 0.1f};
        aData.lifeTime = 15.0f;
        aData.playerArrow = true;

        aData.vel = glm::normalize(
                        glm::vec3(rand() % 5 - 2, rand() % 3, rand() % 5 - 2)) *
                    16.0f;
        w->arrow->add_arrow(aData);
    }
}
} // namespace CrossCraft::Mob