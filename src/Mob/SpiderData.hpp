#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"

namespace CrossCraft::Mob {
    class SpiderData : public AggressiveMob {
    public:
        SpiderData() = default;
        ~SpiderData() = default;
    };

}// namespace CrossCraft::Mob