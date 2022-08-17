#pragma once
#include "Mob.hpp"

namespace CrossCraft
{
    class PassiveMob : public Mob
    {
    public:
        PassiveMob() = default;
        virtual ~PassiveMob() = default;
        void update(float dt, Player *p, World* w);
    };
}