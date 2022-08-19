#pragma once
#include "../Mob/PassiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class ArmorData : public PassiveMob {
  public:
    ArmorData() = default;
    ~ArmorData() = default;
};

class Armor {
  public:
    Armor();
    ~Armor();

    void draw(ArmorData *sd);

  private:
    uint32_t tex;

    Model arm;
    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft