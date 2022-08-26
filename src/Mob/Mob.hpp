#pragma once
#include "../Entity/Entity.hpp"

namespace CrossCraft {
enum class MobType {
    Steve = 0x00,
    Zombie = 0x01,
    Skeleton = 0x02,
    Creeper = 0x03,
    Spider = 0x04,
    Pig = 0x05,
    Sheep = 0x06
};

class Player;

class Mob : public Entity {
  public:
    Mob() = default;
    virtual ~Mob() = default;

    MobType mobType;
    glm::vec2 head_rotation;
    bool inRange;
    bool isAnimating;
    float animationTime;

    int8_t HP;
    bool isAlive;
    bool despawned;

    static Mob* make_mob(MobType type);

    virtual void update(float dt, Player *p, World *w) = 0;
};
} // namespace CrossCraft