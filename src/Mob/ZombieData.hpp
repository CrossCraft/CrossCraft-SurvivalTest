#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"

namespace CrossCraft::Mob {
    class ZombieData : public AggressiveMob {
    public:
        ZombieData() = default;
        ~ZombieData() = default;
    };

}// namespace CrossCraft::Mob