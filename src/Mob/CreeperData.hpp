#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include <glm.hpp>

namespace CrossCraft::Mob {
class CreeperData : public AggressiveMob {
  public:
    CreeperData() = default;
    ~CreeperData() = default;
    void OnDeath(World *w, bool playerKill);
};
} // namespace CrossCraft::Mob