#pragma once
#include "../Model/ArmorModel.hpp"
#include "../Model/CreeperModel.hpp"
#include "../Model/PigModel.hpp"
#include "../Model/SheepModel.hpp"
#include "../Model/SkeletonModel.hpp"
#include "../Model/SpiderModel.hpp"
#include "../Model/SteveModel.hpp"
#include "../Model/ZombieModel.hpp"
#include "MobData.hpp"
#include <vector>

namespace CrossCraft {
class Player;
}

namespace CrossCraft::Mob {

class MobManager {
  public:
    MobManager();
    ~MobManager();

    void add_mob(MobData *mobData);

    void update(float dt, Player *p, World *w);
    void draw();

    std::vector<MobData *> mobs;

  private:
};
} // namespace CrossCraft::Mob