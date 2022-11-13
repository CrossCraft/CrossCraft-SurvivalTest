#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include <glm.hpp>

namespace CrossCraft::Mob {
class SkeletonData : public AggressiveMob {
  public:
    SkeletonData() = default;
    ~SkeletonData() = default;

    void update(float dt, Player *p, World *w);
    void OnDeath(World *w, bool playerKill);
    float fireTime;
};

} // namespace CrossCraft::Mob