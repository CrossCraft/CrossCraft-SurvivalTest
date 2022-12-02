#pragma once
#include <Rendering/Rendering.hpp>
using namespace Stardust_Celeste;

namespace CrossCraft {

    class Clouds {
    public:
        Clouds();
        ~Clouds();

        void update(double dt);
        void draw();

        bool draw_sky;

    private:
        void generate();

        unsigned int texture;

        float scroll;

        uint16_t idx_counter;
        Rendering::Mesh<Rendering::Vertex> mesh;

#if BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_3DS
        uint16_t idx_counter2;
        Rendering::Mesh<Rendering::Vertex> mesh2;
#endif
    };
}// namespace CrossCraft