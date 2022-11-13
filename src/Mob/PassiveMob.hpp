#pragma once
#include "MobData.hpp"

namespace CrossCraft::Mob {
class PassiveMob : public MobData {
  public:
    float dirChangeTime;
    PassiveMob() = default;
    virtual ~PassiveMob() = default;
    void update(float dt, Player *p, World *w);
    void OnDeath(World *w, bool playerKill);
};
} // namespace CrossCraft::Mob