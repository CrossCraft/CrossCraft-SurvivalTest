#include "MobManager.hpp"

namespace CrossCraft
{

    MobManager::MobManager()
    {
        steve = create_scopeptr<Steve>();
        skeleton = create_scopeptr<Skeleton>();
        zombie = create_scopeptr<Zombie>();
        creeper = create_scopeptr<Creeper>();
        pig = create_scopeptr<Pig>();
        sheep = create_scopeptr<Sheep>();

        mobs.clear();
    }
    MobManager::~MobManager()
    {
        for (auto m : mobs)
        {
            delete m;
        }

        mobs.clear();
    }

    void MobManager::add_mob(Mob *mobData)
    {
        mobs.push_back(mobData);
    }

    void MobManager::update(float dt, Player *p, World *w)
    {
        //for (auto m : mobs)
        //{
        //    m->update(dt, p, w);
        //}
    }
    void MobManager::draw()
    {
        for (auto m : mobs)
        {
            switch (m->mobType)
            {
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

            default:
                break;
            }
        }
    }
}