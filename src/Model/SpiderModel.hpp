#pragma once
#include <glm.hpp>
#include "../Entity/Entity.hpp"
#include "../Mob/AggressiveMob.hpp"
#include "../Mob/SpiderData.hpp"
#include "Model.hpp"

namespace CrossCraft::Model {
    using namespace Stardust_Celeste;

    class Spider {
    public:
        Spider();
        ~Spider();

        void draw(Mob::SpiderData *sd);

        inline static auto get() -> Spider & {
            static Spider spider;
            return spider;
        }

    private:
        uint32_t tex;

        Model leg;
        Model torso;
        Model head;
        Model tail;
    };
}// namespace CrossCraft::Model