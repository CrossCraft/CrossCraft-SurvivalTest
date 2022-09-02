#pragma once
#include <Rendering/Rendering.hpp>
using namespace Stardust_Celeste;

namespace CrossCraft {

struct WeatherParticle {
    glm::vec3 position;
    std::array<float, 8> uv;
    glm::vec3 velocity;
};

class WeatherParticleSystem {
  public:
    WeatherParticleSystem(uint32_t tex);
    ~WeatherParticleSystem();

    void initialize(uint32_t type, glm::vec3 pos);
    void generate();

    void update(double dt);
    void draw(glm::vec3 rot);

  private:
    float timer = 0.0f;
    unsigned int texture;

    uint16_t idx_counter;
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;

    std::vector<WeatherParticle> particles;

    Rendering::Mesh mesh;
};
} // namespace CrossCraft