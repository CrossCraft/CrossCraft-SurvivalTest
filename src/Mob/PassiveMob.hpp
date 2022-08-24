#pragma once
#include "Mob.hpp"

namespace CrossCraft {
class PassiveMob : public Mob {
  public:
    float dirChangeTime;
    PassiveMob() = default;
    virtual ~PassiveMob() = default;
    void update(float dt, Player *p, World *w);
};
} // namespace CrossCraft