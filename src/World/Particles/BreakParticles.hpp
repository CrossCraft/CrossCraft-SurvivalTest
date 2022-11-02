#pragma once
#include <Rendering/Rendering.hpp>
using namespace Stardust_Celeste;

namespace CrossCraft {

struct BreakParticle {
    glm::vec3 position;
    std::array<float, 8> uv;
    glm::vec3 velocity;
};

class BreakParticleSystem {
  public:
    BreakParticleSystem(uint32_t tex);
    ~BreakParticleSystem();

    void initialize(uint32_t type, glm::vec3 pos);
    void generate();

    void update(double dt);
    void draw(glm::vec3 rot);

  private:
    unsigned int texture;

    std::vector<BreakParticle> particles;

    Rendering::FixedMesh<Rendering::Vertex, 4, 6> mesh;
};
} // namespace CrossCraft