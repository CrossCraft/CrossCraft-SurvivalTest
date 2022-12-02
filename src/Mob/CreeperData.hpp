#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"

namespace CrossCraft::Mob {
    class CreeperData : public AggressiveMob {
    public:
        CreeperData() = default;
        ~CreeperData() = default;
        void OnDeath(World *w, bool playerKill);
    };
}// namespace CrossCraft::Mob