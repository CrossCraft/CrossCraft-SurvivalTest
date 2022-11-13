#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include <glm.hpp>

namespace CrossCraft::Mob {
class ZombieData : public AggressiveMob {
  public:
    ZombieData() = default;
    ~ZombieData() = default;
};

} // namespace CrossCraft::Mob