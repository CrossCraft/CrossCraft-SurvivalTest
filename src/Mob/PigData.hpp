#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"
#include <glm.hpp>

namespace CrossCraft::Mob {
class PigData : public PassiveMob {
  public:
    PigData() = default;
    ~PigData() = default;
};

} // namespace CrossCraft::Mob