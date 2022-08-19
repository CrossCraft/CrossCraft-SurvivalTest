#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class SheepData : public PassiveMob {
  public:
    SheepData() = default;
    ~SheepData() = default;
    float animationTime;
    bool hasWool;
};

class Sheep {
  public:
    Sheep();
    ~Sheep();

    void draw(SheepData *sd);

  private:
    uint32_t tex, tex2;

    Model leg;
    Model torso;
    Model head;
    Model legWool;
    Model torsoWool;
    Model headWool;
};
} // namespace CrossCraft