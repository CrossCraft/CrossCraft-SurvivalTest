#pragma once
#include "../Model/Armor.hpp"
#include "../Model/Creeper.hpp"
#include "../Model/Pig.hpp"
#include "../Model/Sheep.hpp"
#include "../Model/Skeleton.hpp"
#include "../Model/Spider.hpp"
#include "../Model/Steve.hpp"
#include "../Model/Zombie.hpp"
#include "Mob.hpp"
#include <vector>

namespace CrossCraft {
using namespace Stardust_Celeste;

class Player;
class MobManager {
  public:
    MobManager();
    ~MobManager();

    void add_mob(Mob *mobData);

    void update(float dt, Player *p, World *w);
    void draw();

  private:
    std::vector<Mob *> mobs;

    ScopePtr<Creeper> creeper;
    ScopePtr<Steve> steve;
    ScopePtr<Skeleton> skeleton;
    ScopePtr<Zombie> zombie;
    ScopePtr<Pig> pig;
    ScopePtr<Sheep> sheep;
    ScopePtr<Spider> spider;
};
} // namespace CrossCraft