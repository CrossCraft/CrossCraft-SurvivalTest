#pragma once
#include "MobData.hpp"

namespace CrossCraft::Mob {
    class AggressiveMob : public MobData {
    public:
        AggressiveMob() = default;
        virtual ~AggressiveMob() = default;
        void update(float dt, Player *p, World *w);
    };
}// namespace CrossCraft::Mob