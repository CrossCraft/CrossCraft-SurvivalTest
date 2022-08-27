#pragma once
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class SkeletonData : public AggressiveMob {
  public:
    SkeletonData() = default;
    ~SkeletonData() = default;

    void update(float dt, Player* p, World* w);
    float fireTime;
};

class Skeleton {
  public:
    Skeleton();
    ~Skeleton();

    void draw(SkeletonData *sd);

  private:
    uint32_t tex;

    Model arm;
    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft