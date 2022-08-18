#pragma once
#include <glm.hpp>
#include "Model.hpp"
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;
    class CreeperData : public AggressiveMob
    {
    public:
        CreeperData() = default;
        ~CreeperData() = default;
        glm::vec2 head_rotation;
        float animationTime;
    };

    class Creeper
    {
    public:
        Creeper();
        ~Creeper();

        void draw(CreeperData *sd);

    private:
        uint32_t tex;

        Model leg;
        Model torso;
        Model head;
    };
}