#include "DeathParticles.hpp"
#include "../../Chunk/ChunkUtil.hpp"

namespace CrossCraft {

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
DeathParticleSystem::DeathParticleSystem(uint32_t tex) : texture(tex) { idx_counter = 0; }
DeathParticleSystem::~DeathParticleSystem() {
    m_verts.clear();
    m_index.clear();
}

auto rand_pos2() -> float { return ((rand() % 16) - 8) * (1.0f / 16.0f); }
float timer2 = 0.0f;
auto bind_texture(DeathParticle &particle, uint32_t type) -> void {
    type = rand() % 8;

    const float UV_X = 1.0f / 16.0f;
    const float UV_Y = 1.0f / 16.0f;

    particle.uv[0] = type * UV_X;
    particle.uv[1] = 0.0f;
    particle.uv[2] = type * UV_X;
    particle.uv[3] = UV_Y;
    particle.uv[4] = UV_X + type * UV_X;
    particle.uv[5] = UV_Y;
    particle.uv[6] = UV_X + type * UV_X;
    particle.uv[7] = 0.0f;
}

void DeathParticleSystem::initialize(uint32_t type, glm::vec3 pos) {
    particles.clear();
    srand(pos.x + pos.y + pos.z);
    for (int i = 0; i < 32; i++) {

        DeathParticle particle;
        particle.position = pos;
        particle.position.x += rand_pos2();
        particle.position.y += rand_pos2();
        particle.position.z += rand_pos2();
        particle.velocity = { 0, 0, 0 };

        particle.velocity.y = ((float)(rand() % 14) / 14.0f) + 1.0f;

        bind_texture(particle, type);

        particles.push_back(particle);
    }

    timer2 = 0.0f;
}

void DeathParticleSystem::generate() {
    mesh.delete_data();
    idx_counter = 0;
    m_verts.clear();
    m_index.clear();

    if (particles.size() <= 0)
        return;

    auto cFace = frontFace;

    Rendering::Color c;
    c.color = 0xFFFFFFFF;

    m_verts.push_back(Rendering::Vertex{
        particles[0].uv[0],
        particles[0].uv[1],
        c,
        cFace[0] * 2.0f,
        cFace[1] * 2.0f,
        cFace[2] * 2.0f,
    });

    m_verts.push_back(Rendering::Vertex{
        particles[0].uv[2],
        particles[0].uv[3],
        c,
        cFace[3] * 2.0f,
        cFace[4] * 2.0f,
        cFace[5] * 2.0f,
    });

    m_verts.push_back(Rendering::Vertex{
        particles[0].uv[4],
        particles[0].uv[5],
        c,
        cFace[6] * 2.0f,
        cFace[7] * 2.0f,
        cFace[8] * 2.0f,
    });

    m_verts.push_back(Rendering::Vertex{
        particles[0].uv[6],
        particles[0].uv[7],
        c,
        cFace[9] * 2.0f,
        cFace[10] * 2.0f,
        cFace[11] * 2.0f,
    });

    m_index.push_back(idx_counter);
    m_index.push_back(idx_counter + 1);
    m_index.push_back(idx_counter + 2);
    m_index.push_back(idx_counter + 2);
    m_index.push_back(idx_counter + 3);
    m_index.push_back(idx_counter + 0);
    idx_counter += 4;

    mesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                  m_index.size());
}

void DeathParticleSystem::update(double dt) {

    timer2 += dt;

    if (timer2 < 0.75f) {
        for (auto &p : particles) {
            p.position += p.velocity * (float)dt;
        }

        generate();
    }
}
void DeathParticleSystem::draw(glm::vec3 rot) {
    if (timer2 < 1.0f) {
        Rendering::TextureManager::get().bind_texture(texture);
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        glDisable(GL_CULL_FACE);
#else
        sceGuDisable(GU_CULL_FACE);
#endif

        for (auto &p : particles) {

            Rendering::RenderContext::get().matrix_translate(p.position);
            Rendering::RenderContext::get().matrix_scale(
                {0.125f, 0.125f, 0.125f});
            Rendering::RenderContext::get().matrix_scale({0.75f, 0.75f, 0.75f});
            Rendering::RenderContext::get().matrix_rotate({0.0f, -rot.y, 0.0f});
            Rendering::RenderContext::get().matrix_rotate({-rot.x, 0.0f, 0.0f});

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
