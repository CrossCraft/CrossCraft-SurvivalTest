#include "../Model/CreeperModel.hpp"
#include "../Model/PigModel.hpp"
#include "../Model/SheepModel.hpp"
#include "../Model/SkeletonModel.hpp"
#include "../Model/SpiderModel.hpp"
#include "../Model/SteveModel.hpp"
#include "../Model/ZombieModel.hpp"
#include "../World/World.hpp"
#include "MobData.hpp"

namespace CrossCraft {
    void Entity::OnHit(World *w, int damage, glm::vec3 from, bool player) {}
}// namespace CrossCraft

namespace CrossCraft::Mob {
    MobData *MobData::make_mob(MobType type) {
        switch (type) {
            case MobType::Steve: {
                SteveData *sd = new SteveData();
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
                ZombieData *sd = new ZombieData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Zombie;
                sd->size = {0.6f, 1.8f, 0.6f};
                sd->HP = 20;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = rand() % 3;
                sd->Score = 80;

                return sd;
            }
            case MobType::Skeleton: {
                SkeletonData *sd = new SkeletonData();
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
                sd->Score = 120;

                return sd;
            }
            case MobType::Creeper: {
                CreeperData *sd = new CreeperData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Creeper;
                sd->size = {0.6f, 1.4625f, 0.6f};
                sd->HP = 20;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = 0;
                sd->Score = 200;

                return sd;
            }
            case MobType::Spider: {
                SpiderData *sd = new SpiderData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Spider;
                sd->size = {1.8f, 0.9f, 1.8f};
                sd->HP = 20;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = 0;
                sd->Score = 105;

                return sd;
            }
            case MobType::Pig: {
                PigData *sd = new PigData();
                sd->head_rotation = {30.0f, 120.0f};
                sd->pos = {128.0f, 50.0f, 128.0f};
                sd->rot = {0.0f, 180.0f};
                sd->mobType = MobType::Pig;
                sd->size = {0.6f, 0.9f, 0.6f};
                sd->HP = 10;
                sd->isAlive = true;
                sd->despawned = false;
                sd->armorVal = 0;
                sd->Score = 10;

                return sd;
            }
            case MobType::Sheep: {
                SheepData *sd = new SheepData();
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
                sd->Score = 10;

                return sd;
            }

            default:
                break;
        }

        return nullptr;
    }

    void MobData::OnDeath(World *w, bool playerKill) {
        if (playerKill) {
            w->player->score += Score;
        }
    }

    void MobData::OnHit(World *w, int damage, glm::vec3 from, bool player) {
        if (!isAlive)
            return;

        if (hitCD > 0)
            return;

        HP -= damage;
        if (HP < 0) {
            isAlive = false;
            OnDeath(w, player);
        }
        hitCD = 0.3f;
        vel.x += from.x * 2.5f;
        vel.z += from.z * 2.5f;
    }

}// namespace CrossCraft::Mob