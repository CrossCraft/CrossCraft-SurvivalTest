#include "MobManager.hpp"

namespace CrossCraft
{

    MobManager::MobManager()
    {
        steve = create_scopeptr<Steve>();
        zombie = create_scopeptr<Zombie>();

        mobs.clear();
    }
    MobManager::~MobManager()
    {
        for (auto m : mobs) {
            delete m;
        }

        mobs.clear();
    }

    void MobManager::add_mob(Mob *mobData)
    {
        mobs.push_back(mobData);
    }

    void MobManager::update(float dt, Player *p, World* w)
    {
        for (auto m : mobs) {
            m->update(dt, p, w);
        }
    }
    void MobManager::draw()
    {
        for (auto m : mobs) {
            switch (m->mobType) {
            case MobType::Steve:
                steve->draw((SteveData*)m);
                break;

            case MobType::Zombie:
                zombie->draw((ZombieData*)m);
                break;

            default:
                break;
            }
        }
    }
}