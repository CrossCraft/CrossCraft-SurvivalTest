#include "Mob.hpp"
#include "../Model/Steve.hpp"
#include "../Model/Zombie.hpp"
#include "../Model/Skeleton.hpp"
#include "../Model/Creeper.hpp"
#include "../Model/Spider.hpp"
#include "../Model/Pig.hpp"
#include "../Model/Sheep.hpp"

namespace CrossCraft{
    Mob* Mob::make_mob(MobType type){
        switch(type){
            case MobType::Steve: { 
                SteveData* sd = new SteveData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Steve;
                sd->size = {0.6f, 1.8f, 0.6f};
                sd->HP = 20;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = 0;

                return sd;
            }
            case MobType::Zombie: {
                ZombieData* sd = new ZombieData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Zombie;
                sd->size = {0.6f, 1.8f, 0.6f};
                sd->HP = 20;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = rand() % 3;

                return sd;
            }
            case MobType::Skeleton: { 
                SkeletonData* sd = new SkeletonData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Skeleton;
                sd->size = {0.6f, 1.8f, 0.6f};
                sd->HP = 20;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = rand() % 3;
                sd->fireTime = 0.0f;

                return sd;
            }
            case MobType::Creeper: { 
                CreeperData* sd = new CreeperData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Creeper;
                sd->size = {0.6f, 1.4625f, 0.6f};
                sd->HP = 20;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = 0;

                return sd;
            }
            case MobType::Spider: {
                SpiderData* sd = new SpiderData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Spider;
                sd->size = {1.8f, 0.9f, 1.8f};
                sd->HP = 20;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = 0;

                return sd;
            }
            case MobType::Pig: { 
                PigData* sd = new PigData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Pig;
                sd->size = {0.6f, 0.9f, 0.6f};
                sd->HP = 10;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = 0;

                return sd;
            }
            case MobType::Sheep: { 
                SheepData* sd = new SheepData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Sheep;
                sd->size = {0.6f, 0.9f, 0.6f};
                sd->HP = 8;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = 1;
                sd->hasWool = true;

                return sd;
            }

            default:
                break;
        }

        return nullptr;
    }
}