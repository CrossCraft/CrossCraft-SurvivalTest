#pragma once
#include "Mob.hpp"
#include <vector>
#include "../Model/Steve.hpp"
#include "../Model/Zombie.hpp"
#include "../Model/Skeleton.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;

    class Player;
    class MobManager
    {
    public:
        MobManager();
        ~MobManager();

        void add_mob(Mob *mobData);

        void update(float dt, Player *p, World *w);
        void draw();

    private:
        std::vector<Mob *> mobs;

        ScopePtr<Steve> steve;
        ScopePtr<Skeleton> skeleton;
        ScopePtr<Zombie> zombie;
    };
}