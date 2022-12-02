#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"

namespace CrossCraft::Mob {
    class PigData : public PassiveMob {
    public:
        PigData() = default;
        ~PigData() = default;
    };

}// namespace CrossCraft::Mob