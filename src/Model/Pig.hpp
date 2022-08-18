#pragma once
#include <glm.hpp>
#include "Model.hpp"
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;
    class PigData : public PassiveMob
    {
    public:
        PigData() = default;
        ~PigData() = default;
        glm::vec2 head_rotation;
        float animationTime;
    };

    class Pig
    {
    public:
        Pig();
        ~Pig();

        void draw(PigData *sd);

    private:
        uint32_t tex;

        Model leg;
        Model torso;
        Model head;
    };
}