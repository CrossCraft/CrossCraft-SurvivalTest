#pragma once
#include <glm.hpp>
#include "Model.hpp"
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;
    class ZombieData : public AggressiveMob
    {
    public:
        ZombieData() = default;
        ~ZombieData() = default;
        glm::vec2 head_rotation;
        float animationTime;
    };

    class Zombie
    {
    public:
        Zombie();
        ~Zombie();

        void draw(ZombieData* sd);

    private:
        uint32_t tex;

        Model arm;
        Model leg;
        Model torso;
        Model head;
    };
}