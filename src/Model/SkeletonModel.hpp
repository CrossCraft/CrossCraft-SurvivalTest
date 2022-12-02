#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include "Model.hpp"

#include "../Mob/SkeletonData.hpp"

namespace CrossCraft::Model {
    using namespace Stardust_Celeste;

    class Skeleton {
    public:
        Skeleton();
        ~Skeleton();

        void draw(Mob::SkeletonData *sd);

        inline static auto get() -> Skeleton & {
            static Skeleton skeleton;
            return skeleton;
        }

    private:
        uint32_t tex;

        Model arm;
        Model leg;
        Model torso;
        Model head;
    };
}// namespace CrossCraft::Model