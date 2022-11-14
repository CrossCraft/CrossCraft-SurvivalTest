#pragma once
#include <Rendering/Rendering.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;

struct Particle {
    glm::vec3 position, velocity;
    float pre, lifetime;
    u8 type, variant;
};

enum ParticleType {
    Particle_Death = 0,
    Particle_Break = 1,
    Particle_Weather = 2,
};

class ParticleManager final : public Singleton {

  public:
    ParticleManager();

    inline static auto get() -> ParticleManager & {
        static ParticleManager partMan;
        return partMan;
    }

    void spawn_particles(ParticleType type, glm::vec3 position,
                         uint8_t blkType);

    void update(double dt);
    void draw(glm::vec3 rot);

    void generate_mesh(ParticleType type, glm::vec3 pos, uint8_t blkType);

    u32 terrain, weather, death;

  private:
    std::unordered_map<u32, Particle> particles;

    u32 count;

    Rendering::FixedMesh<Rendering::Vertex, 4, 6> break_mesh[4];
    Rendering::FixedMesh<Rendering::Vertex, 4, 6> death_mesh[4];
    Rendering::FixedMesh<Rendering::Vertex, 4, 6> weather_mesh;
};

} // namespace CrossCraft