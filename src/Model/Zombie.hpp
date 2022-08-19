#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class ZombieData : public AggressiveMob {
  public:
    ZombieData() = default;
    ~ZombieData() = default;
    float animationTime;
};

class Zombie {
  public:
    Zombie();
    ~Zombie();

    void draw(ZombieData *sd);

  private:
    uint32_t tex;

    Model arm;
    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft