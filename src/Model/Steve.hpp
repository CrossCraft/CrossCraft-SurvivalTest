#pragma once
#include "../Mob/PassiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class SteveData : public PassiveMob {
  public:
    SteveData() = default;
    ~SteveData() = default;
    float animationTime;
};

class Steve {
  public:
    Steve();
    ~Steve();

    void draw(SteveData *sd);

  private:
    uint32_t tex;

    Model arm;
    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft