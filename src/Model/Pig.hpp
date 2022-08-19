#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class PigData : public PassiveMob {
  public:
    PigData() = default;
    ~PigData() = default;
    float animationTime;
};

class Pig {
  public:
    Pig();
    ~Pig();

    void draw(PigData *sd);

  private:
    uint32_t tex;

    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft