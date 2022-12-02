#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include "../Mob/CreeperData.hpp"
#include "Model.hpp"

namespace CrossCraft::Model {

    class Creeper {
    public:
        Creeper();
        ~Creeper();

        void draw(Mob::CreeperData *sd);

        inline static auto get() -> Creeper & {
            static Creeper creeper;
            return creeper;
        }

    private:
        uint32_t tex;

        Model leg;
        Model torso;
        Model head;
    };
}// namespace CrossCraft::Model