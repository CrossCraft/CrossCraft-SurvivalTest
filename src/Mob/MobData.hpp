#pragma once
#include "../Entity/Entity.hpp"

namespace CrossCraft {
class Player;
}

namespace CrossCraft::Mob {
enum class MobType {
    Steve = 0x00,
    Zombie = 0x01,
    Skeleton = 0x02,
    Creeper = 0x03,
    Spider = 0x04,
    Pig = 0x05,
    Sheep = 0x06
};

class MobData : public Entity {
  public:
    MobData() = default;
    virtual ~MobData() = default;

    MobType mobType;
    glm::vec2 head_rotation;
    bool inRange;
    bool isAnimating;
    float animationTime;

    int8_t HP;
    bool isAlive;
    bool despawned;
    float deathTime;
    float hitCD;

    uint32_t Score;

    uint8_t armorVal;

    static MobData *make_mob(MobType type);

    virtual void OnDeath(World *w, bool playerKill);
    virtual void update(float dt, Player *p, World *w) = 0;
    void OnHit(World *w, int damage, glm::vec3 from, bool player);
};
} // namespace CrossCraft::Mob