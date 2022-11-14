#include "ParticleManager.hpp"
#include "../../Chunk/ChunkUtil.hpp"

namespace CrossCraft {
ParticleManager::ParticleManager() { count = 0; }

auto rand_pos() -> float { return ((rand() % 16) - 8) * (1.0f / 16.0f); }

auto bind_texture_break(std::array<Rendering::Vertex, 4> &v, uint32_t type)
    -> void {
    auto uv = Chunk::getTexCoord(type, 0xFFCCCCCC);

    const float UV_SIZE = (2.0f / 16.0f) / 16.0f;
    glm::vec2 uv_offset = glm::vec2(uv[6], uv[7]);
    float offset = ((rand() % 12) + 2) * ((1.0f / 16.0f) / 16.0f);
    float offset2 = ((rand() % 12) + 2) * ((1.0f / 16.0f) / 16.0f);
    uv_offset += glm::vec2(offset, offset2);

    // x, h, w, h, w, y, x, y
    //  0 1
    //  1 1
    //  1 0
    //  0 0
    v[0].u = uv_offset.x;
    v[0].v = uv_offset.y + UV_SIZE;

    v[1].u = uv_offset.x + UV_SIZE;
    v[1].v = uv_offset.y + UV_SIZE;

    v[2].u = uv_offset.x + UV_SIZE;
    v[2].v = uv_offset.y;

    v[3].u = uv_offset.x;
    v[3].v = uv_offset.y;
}

auto bind_texture(std::array<Rendering::Vertex, 4> &v) -> void {
    uint32_t type = rand() % 8;

    const float UV_X = 1.0f / 16.0f;
    const float UV_Y = 1.0f / 16.0f;

    v[0].u = type * UV_X;
    v[0].v = 0.0f;
    v[1].u = type * UV_X;
    v[1].v = UV_Y;
    v[2].u = UV_X + type * UV_X;
    v[2].v = UV_Y;
    v[3].u = UV_X + type * UV_X;
    v[3].v = 0.0f;
}

void generate_verts() {}

void gen_mesh(Rendering::FixedMesh<Rendering::Vertex, 4, 6> &m) {
    auto cFace = frontFace;

    Rendering::Color c;
    c.color = 0xFFFFFFFF;

    for (int i = 0; i < 4; i++) {
        m.vertices[i].x = cFace[i * 3 + 0];
        m.vertices[i].y = cFace[i * 3 + 1];
        m.vertices[i].z = cFace[i * 3 + 2];
        m.vertices[i].color = c;
    }

    m.indices[0] = 0;
    m.indices[1] = 1;
    m.indices[2] = 2;
    m.indices[3] = 2;
    m.indices[4] = 3;
    m.indices[5] = 0;
}

void ParticleManager::generate_mesh(ParticleType type, glm::vec3 pos,
                                    uint8_t blkType) {
    Rendering::FixedMesh<Rendering::Vertex, 4, 6> weather_mesh;

    if (type == Particle_Break) {
        for (int i = 0; i < 4; i++) {
            gen_mesh(break_mesh[i]);
            bind_texture_break(break_mesh[i].vertices, blkType);
            break_mesh[i].setup_buffer();
        }
    } else if (type == Particle_Death) {
        for (int i = 0; i < 4; i++) {
            gen_mesh(death_mesh[i]);
            bind_texture_break(death_mesh[i].vertices, 0);
            death_mesh[i].setup_buffer();
        }
    } else {
        gen_mesh(weather_mesh);
        bind_texture(weather_mesh.vertices);
        weather_mesh.setup_buffer();
    }
}

void ParticleManager::spawn_particles(ParticleType type, glm::vec3 pos,
                                      uint8_t blkType) {
    srand(pos.x + pos.y + pos.z);

    if (type == Particle_Break || type == Particle_Death)
        generate_mesh(type, pos, blkType);

    for (int i = 0; i < 32; i++) {
        Particle particle;
        particle.position = pos;
        particle.position.x += rand_pos();
        particle.position.y += rand_pos();
        particle.position.z += rand_pos();

        particle.velocity = glm::vec3(rand() % 10, rand() % 37, rand() % 10);
        particle.velocity /= 13.0f;

        particle.pre = 0.0f;
        particle.lifetime = 1.0f;

        particle.type = type;
        particle.variant = rand() % 4;

        particles.emplace(count++, particle);
    }
}

void ParticleManager::update(double dt) {
    std::vector<u32> dead;

    for (auto &[id, p] : particles) {
        if (p.pre > 0)
            p.pre -= dt;
        else
            p.lifetime -= dt;

        if (p.lifetime < 0)
            dead.push_back(id);

        if (p.type == 1 || p.type == 2)
            p.velocity.y -= 16.0f * (float)dt;

        p.position += p.velocity * (float)dt;
    }

    for (auto id : dead)
        particles.erase(id);
}

void ParticleManager::draw(glm::vec3 rot) {

    for (auto &[id, p] : particles) {
        if (p.pre > 0)
            continue;

        GI::set_culling_mode(false, true);

        switch (p.type) {
        case Particle_Death: {
            Rendering::TextureManager::get().bind_texture(death);
            break;
        }

        case Particle_Break: {
            Rendering::TextureManager::get().bind_texture(terrain);
            break;
        }

        case Particle_Weather: {
            Rendering::TextureManager::get().bind_texture(weather);
            break;
        }
        }

        Rendering::RenderContext::get().matrix_translate(p.position);
        Rendering::RenderContext::get().matrix_scale({0.125f, 0.125f, 0.125f});
        Rendering::RenderContext::get().matrix_scale({0.75f, 0.75f, 0.75f});
        Rendering::RenderContext::get().matrix_rotate({0.0f, -rot.y, 0.0f});
        Rendering::RenderContext::get().matrix_rotate({-rot.x, 0.0f, 0.0f});

        // DRAW MESH
        switch (p.type) {
        case Particle_Death: {
            death_mesh[p.variant].bind();
            death_mesh[p.variant].draw();
            break;
        }

        case Particle_Break: {
            break_mesh[p.variant].bind();
            break_mesh[p.variant].draw();
            break;
        }

        case Particle_Weather: {
            weather_mesh.bind();
            weather_mesh.draw();
            break;
        }
        }

        Rendering::RenderContext::get().matrix_clear();

        GI::set_culling_mode(true, true);
    }
}
} // namespace CrossCraft