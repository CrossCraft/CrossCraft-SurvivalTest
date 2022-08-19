#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class SpiderData : public AggressiveMob {
  public:
    SpiderData() = default;
    ~SpiderData() = default;
};

class Spider {
  public:
    Spider();
    ~Spider();

    void draw(SpiderData *sd);

  private:
    uint32_t tex;

    Model leg;
    Model torso;
    Model head;
    Model tail;
};
} // namespace CrossCraft