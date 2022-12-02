#pragma once
#include <glm.hpp>
#include "../Mob/ArmorData.hpp"
#include "../Mob/PassiveMob.hpp"
#include "Model.hpp"

namespace CrossCraft::Model {
    using namespace Stardust_Celeste;

    class Armor : public Singleton {
    public:
        Armor();
        ~Armor();

        void draw(Mob::MobData *mobdata, Mob::ArmorData &sd);

        inline static auto get() -> Armor & {
            static Armor armor;
            return armor;
        }

    private:
        uint32_t tex;

        Model arm;
        Model leg;
        Model torso;
        Model head;
    };
}// namespace CrossCraft::Model