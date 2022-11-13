#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include <glm.hpp>

namespace CrossCraft::Mob {
class SpiderData : public AggressiveMob {
  public:
    SpiderData() = default;
    ~SpiderData() = default;
};

} // namespace CrossCraft::Mob