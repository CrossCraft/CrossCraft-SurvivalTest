#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"

namespace CrossCraft::Mob {
    class SheepData : public PassiveMob {
    public:
        SheepData() = default;
        ~SheepData() = default;
        bool hasWool;
    };

}// namespace CrossCraft::Mob