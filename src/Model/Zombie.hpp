#pragma once
#include <glm.hpp>
#include "Model.hpp"
#include "../Entity/Entity.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;
    //TODO: Add Mob Data
    struct ZombieData : public Entity
    {
        glm::vec2 head_rotation;
        float animationTime;
    };

    class Zombie
    {
    public:
        Zombie();
        ~Zombie();

        void draw(ZombieData sd);

    private:
        uint32_t tex;

        Model arm;
        Model leg;
        Model torso;
        Model head;
    };
}