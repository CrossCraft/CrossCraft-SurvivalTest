#pragma once
#include "../Mob/PassiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
struct ArmorData{
  bool helmet;
  bool torso;
  bool zombie;
};

class Armor {
  public:
    Armor();
    ~Armor();

    void draw(Mob* mobdata, ArmorData& sd);

    inline static auto get() -> Armor & {
        static Armor armor;
        return armor;
    }

  private:
    uint32_t tex;

    Model arm;
    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft