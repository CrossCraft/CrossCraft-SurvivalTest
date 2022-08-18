#pragma once
#include <glm.hpp>
#include "Model.hpp"
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;
    class SkeletonData : public AggressiveMob
    {
    public:
        SkeletonData() = default;
        ~SkeletonData() = default;
        glm::vec2 head_rotation;
        float animationTime;
    };

    class Skeleton
    {
    public:
        Skeleton();
        ~Skeleton();

        void draw(SkeletonData *sd);

    private:
        uint32_t tex;

        Model arm;
        Model leg;
        Model torso;
        Model head;
    };
}