#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class CreeperData : public AggressiveMob {
  public:
    CreeperData() = default;
    ~CreeperData() = default;
    float animationTime;
};

class Creeper {
  public:
    Creeper();
    ~Creeper();

    void draw(CreeperData *sd);

  private:
    uint32_t tex;

    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft