#pragma once
#include <glm.hpp>
#include "../Mob/PassiveMob.hpp"
#include "../Mob/SteveData.hpp"
#include "Model.hpp"

namespace CrossCraft::Model {
    using namespace Stardust_Celeste;

    class Steve {
    public:
        Steve();
        ~Steve();

        void draw(Mob::SteveData *sd);

        inline static auto get() -> Steve & {
            static Steve steve;
            return steve;
        }

    private:
        uint32_t tex;

        Model arm;
        Model leg;
        Model torso;
        Model head;
    };
}// namespace CrossCraft::Model