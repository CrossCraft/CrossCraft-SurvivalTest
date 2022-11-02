#include "WeatherParticles.hpp"
#include "../../Chunk/ChunkUtil.hpp"

namespace CrossCraft {

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
WeatherParticleSystem::WeatherParticleSystem(uint32_t tex) : texture(tex) {}
WeatherParticleSystem::~WeatherParticleSystem() {}

auto bind_texture(WeatherParticle &particle, uint32_t type) -> void {
    // x, h, w, h, w, y, x, y
    //  0 1
    //  1 1
    //  1 0
    //  0 0
    particle.uv[0] = 0;
    particle.uv[1] = 1;
    particle.uv[2] = 1;
    particle.uv[3] = 1;
    particle.uv[4] = 1;
    particle.uv[5] = 0;
    particle.uv[6] = 0;
    particle.uv[7] = 0;
}

void WeatherParticleSystem::initialize(uint32_t type, glm::vec3 pos) {
    particles.clear();
    srand(pos.x + pos.y + pos.z);

    for (int x = -3; x <= 3; x++) {
        for (int y = 0; y <= 2; y++) {
            for (int z = -3; z <= 3; z++) {
                WeatherParticle particle;
                particle.position = pos + glm::vec3(x, y * 4, z);
                particle.velocity = {0, -1, 0};

                bind_texture(particle, type);

                particles.push_back(particle);
            }
        }
    }
    timer = 0.0f;
}

void WeatherParticleSystem::generate() {
    mesh.delete_data();

    if (particles.size() <= 0)
        return;

    auto cFace = frontFace;

    Rendering::Color c;
    c.color = 0xFFFFFFFF;

    mesh.vertices[0] = Rendering::Vertex{
        particles[0].uv[0], particles[0].uv[1], c, cFace[0], cFace[1], cFace[2],
    };

    mesh.vertices[1] = Rendering::Vertex{
        particles[0].uv[2], particles[0].uv[3], c, cFace[3], cFace[4], cFace[5],
    };

    mesh.vertices[2] = Rendering::Vertex{
        particles[0].uv[4], particles[0].uv[5], c, cFace[6], cFace[7], cFace[8],
    };

    mesh.vertices[3] = Rendering::Vertex{
        particles[0].uv[6], particles[0].uv[7], c,
        cFace[9],           cFace[10],          cFace[11],
    };

    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 2;
    mesh.indices[3] = 2;
    mesh.indices[4] = 3;
    mesh.indices[5] = 0;

    mesh.setup_buffer();
}

void WeatherParticleSystem::update(double dt) {

    timer += dt;

    if (timer < 1.0f) {
        for (auto &p : particles) {
            p.velocity.y -= 16.0f * (float)dt;
            p.position += p.velocity * (float)dt;
        }

        generate();
    }
}
void WeatherParticleSystem::draw(glm::vec3 rot) {
    if (timer < 1.0f) {
        Rendering::TextureManager::get().bind_texture(texture);
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        glDisable(GL_CULL_FACE);
#else
        sceGuDisable(GU_CULL_FACE);
#endif

        for (auto &p : particles) {

            Rendering::RenderContext::get().matrix_translate(p.position);
            Rendering::RenderContext::get().matrix_rotate({0, -rot.y, 0});
            Rendering::RenderContext::get().matrix_scale({1.0f, 4.0f, 1.0f});
            mesh.bind();
            mesh.draw();

            Rendering::RenderContext::get().matrix_clear();
        }

#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        glEnable(GL_CULL_FACE);
#else
        sceGuEnable(GU_CULL_FACE);
#endif
    }
}

} // namespace CrossCraft
