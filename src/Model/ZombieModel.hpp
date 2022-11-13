#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include "../Mob/ZombieData.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft::Model {
using namespace Stardust_Celeste;

class Zombie {
  public:
    Zombie();
    ~Zombie();

    void draw(Mob::ZombieData *sd);

    inline static auto get() -> Zombie & {
        static Zombie zombie;
        return zombie;
    }

  private:
    uint32_t tex;

    Model arm;
    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft::Model