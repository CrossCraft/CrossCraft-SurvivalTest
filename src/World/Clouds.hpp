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
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;
    Rendering::Mesh mesh;

#ifdef PSP
    uint16_t idx_counter2;
    std::vector<Rendering::Vertex> m_verts2;
    std::vector<uint16_t> m_index2;
    Rendering::Mesh mesh2;
#endif
};
} // namespace CrossCraft