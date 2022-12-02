#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"
#include "../Mob/SheepData.hpp"
#include "Model.hpp"

namespace CrossCraft::Model {
    using namespace Stardust_Celeste;

    class Sheep {
    public:
        Sheep();
        ~Sheep();

        void draw(Mob::SheepData *sd);

        inline static auto get() -> Sheep & {
            static Sheep sheep;
            return sheep;
        }

    private:
        uint32_t tex, tex2;

        Model leg;
        Model torso;
        Model head;
        Model legWool;
        Model torsoWool;
        Model headWool;
    };
}// namespace CrossCraft::Model