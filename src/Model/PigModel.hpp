#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"
#include "../Mob/PigData.hpp"
#include "Model.hpp"

namespace CrossCraft::Model {
    using namespace Stardust_Celeste;

    class Pig {
    public:
        Pig();
        ~Pig();

        inline static auto get() -> Pig & {
            static Pig pig;
            return pig;
        }

        void draw(Mob::PigData *sd);

    private:
        uint32_t tex;

        Model leg;
        Model torso;
        Model head;
    };
}// namespace CrossCraft::Model