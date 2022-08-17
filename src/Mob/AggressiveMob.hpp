#pragma once
#include "Mob.hpp"

namespace CrossCraft
{
    class AggressiveMob : public Mob
    {
    public:
        AggressiveMob() = default;
        virtual ~AggressiveMob() = default;
        void update(Player *p);
    };
}