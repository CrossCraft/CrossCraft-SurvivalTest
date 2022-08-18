#pragma once
#include <glm.hpp>
#include "Model.hpp"
#include "../Entity/Entity.hpp"
#include "../Mob/PassiveMob.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;
    class SpiderData : public PassiveMob
    {
    public:
        SpiderData() = default;
        ~SpiderData() = default;
        glm::vec2 head_rotation;
        float animationTime;
    };

    class Spider
    {
    public:
        Spider();
        ~Spider();

        void draw(SpiderData *sd);

    private:
        uint32_t tex;

        Model leg;
        Model torso;
        Model head;
        Model tail;
    };
}