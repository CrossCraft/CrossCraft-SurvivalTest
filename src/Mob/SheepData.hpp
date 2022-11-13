#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"
#include <glm.hpp>

namespace CrossCraft::Mob {
class SheepData : public PassiveMob {
  public:
    SheepData() = default;
    ~SheepData() = default;
    bool hasWool;
};

} // namespace CrossCraft::Mob