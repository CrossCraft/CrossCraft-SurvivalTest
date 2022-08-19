#include "MobManager.hpp"
#include "../Player/Player.hpp"

namespace CrossCraft {

MobManager::MobManager() {
    steve = create_scopeptr<Steve>();
    skeleton = create_scopeptr<Skeleton>();
    zombie = create_scopeptr<Zombie>();
    creeper = create_scopeptr<Creeper>();
    pig = create_scopeptr<Pig>();
    sheep = create_scopeptr<Sheep>();
    armor = create_scopeptr<Armor>();
    spider = create_scopeptr<Spider>();

    mobs.clear();
}
MobManager::~MobManager() {
    for (auto m : mobs) {
        delete m;
    }

    mobs.clear();
}

void MobManager::add_mob(Mob *mobData) { mobs.push_back(mobData); }

void MobManager::update(float dt, Player *p, World *w) {
    for (auto m : mobs) {
        auto diff = p->pos - m->pos;
        auto len = sqrtf(diff.x * diff.x + diff.z * diff.z);

        if (len > 24.0f)
            m->inRange = false;
        else
            m->inRange = true;

        if (m->vel.x != 0 && m->vel.z != 0 && !m->isAnimating) {
            m->isAnimating = true;
            m->animationTime = 3.0f;
        }
        m->animationTime -= dt;

        if (m->animationTime < 0)
            m->isAnimating = false;

        m->update(dt, p, w);
    }
}
void MobManager::draw() {
    for (auto m : mobs) {
        if (!m->inRange)
            continue;

        switch (m->mobType) {
        case MobType::Steve:
            steve->draw((SteveData *)m);
            break;

        case MobType::Skeleton:
            skeleton->draw((SkeletonData *)m);
            break;

        case MobType::Zombie:
            zombie->draw((ZombieData *)m);
            break;

        case MobType::Creeper:
            creeper->draw((CreeperData *)m);
            break;

        case MobType::Pig:
            pig->draw((PigData *)m);
            break;

        case MobType::Sheep:
            sheep->draw((SheepData *)m);
            break;

        case MobType::Armor:
            armor->draw((ArmorData *)m);
            break;

        case MobType::Spider:
            spider->draw((SpiderData *)m);
            break;

        default:
            break;
        }
    }
}
} // namespace CrossCraft