#include "ParticleManager.hpp"
#include "../../Chunk/ChunkUtil.hpp"

namespace CrossCraft {
ParticleManager::ParticleManager() {
    count = 0;
    raining = false;
    generate_mesh(Particle_Weather, 0);
}

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

auto bind_texture(std::array<Rendering::Vertex, 4> &v, int i) -> void {
    uint32_t type = i;

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

auto bind_texture_weather(std::array<Rendering::Vertex, 4> &v) -> void {
    v[0].u = 0.0f;
    v[0].v = 0.0f;

    v[1].u = 1.0f;
    v[1].v = 0.0f;

    v[2].u = 1.0f;
    v[2].v = 1.0f;

    v[3].u = 0.0f;
    v[3].v = 1.0f;
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

void ParticleManager::generate_mesh(ParticleType type, uint8_t blkType) {

    if (type == Particle_Break) {
        for (int i = 0; i < 4; i++) {
            gen_mesh(break_mesh[i]);
            bind_texture_break(break_mesh[i].vertices, blkType);
            break_mesh[i].setup_buffer();
        }
    } else if (type == Particle_Death) {
        for (int i = 0; i < 8; i++) {
            gen_mesh(death_mesh[i]);
            bind_texture(death_mesh[i].vertices, i);
            death_mesh[i].setup_buffer();
        }
    } else {
        gen_mesh(weather_mesh);
        bind_texture_weather(weather_mesh.vertices);
        weather_mesh.setup_buffer();
    }
}

void ParticleManager::spawn_particles(ParticleType type, glm::vec3 pos,
                                      uint8_t blkType) {
    srand(pos.x + pos.y + pos.z);

    if (type == Particle_Break || type == Particle_Death)
        generate_mesh(type, blkType);

    for (int i = 0; i < 32; i++) {
        Particle particle;
        particle.position = pos;
        if (type != Particle_Weather) {
            particle.position.x += rand_pos();
            particle.position.y += rand_pos();
            particle.position.z += rand_pos();
        }

        if (type == Particle_Break) {
            particle.velocity =
                glm::vec3(rand() % 10, rand() % 37, rand() % 10);
            particle.velocity /= 13.0f;
        } else if (type == Particle_Death) {
            particle.velocity = {0, rand() % 20, 0};
            particle.velocity /= 13.0f;
        } else {
            particle.velocity = {0, -10.0f, 0};
        }

        if (particle.velocity.y < -84.0f) {
            particle.velocity.y = -84.0f;
        }

        particle.pre = 0.0f;
        if (type == Particle_Death) {
            particle.lifetime = 2.0f + 0.5f * static_cast<float>(rand() % 3);
        } else if (type == Particle_Weather) {
            particle.lifetime = 4.0f;
        } else {
            particle.lifetime = 1.0f;
        }

        particle.type = type;
        if (type == Particle_Death) {
            particle.variant = 0;
        } else {
            particle.variant = rand() % 4;
        }

        particles.emplace(count++, particle);

        if (type == Particle_Weather) {
            break;
        }
    }
}

void ParticleManager::update(glm::vec3 pos, double dt) {
    std::vector<u32> dead;

    if (raining) {
        raintimer += dt;
        if (raintimer > 0.4f) {
            raintimer = 0.0f;

            for (int x = -6; x <= 6; x++) {
                for (int z = -6; z <= 6; z++) {
                    auto npos = pos + glm::vec3(static_cast<float>(x), 4.0f,
                                                static_cast<float>(z));
                    spawn_particles(Particle_Weather, npos, 0);
                }
            }
        }
    }

    for (auto &[id, p] : particles) {
        if (p.pre > 0)
            p.pre -= dt;
        else
            p.lifetime -= dt;

        if (p.lifetime < 0)
            dead.push_back(id);

        if (p.type == Particle_Death) {
            p.variant = (p.lifetime) / 0.2f;
            if (p.variant >= 8)
                p.variant = 7;
        }

        if (p.type == Particle_Break || p.type == Particle_Weather)
            p.velocity.y -= 16.0f * (float)dt;

        p.position += p.velocity * (float)dt;
    }

    for (auto id : dead)
        particles.erase(id);
}

void ParticleManager::draw(glm::vec3 rot) {
    GI::set_culling_mode(false, true);

    for (auto &[id, p] : particles) {
        if (p.pre > 0)
            continue;

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
        if (p.type != Particle_Weather) {
            Rendering::RenderContext::get().matrix_scale(
                {0.125f, 0.125f, 0.125f});
            Rendering::RenderContext::get().matrix_scale({0.75f, 0.75f, 0.75f});
            Rendering::RenderContext::get().matrix_rotate({0.0f, -rot.y, 0.0f});
            Rendering::RenderContext::get().matrix_rotate({-rot.x, 0.0f, 0.0f});
        } else {
            Rendering::RenderContext::get().matrix_scale({2, 8, 2});
            Rendering::RenderContext::get().matrix_scale({0.75f, 0.75f, 0.75f});
            Rendering::RenderContext::get().matrix_rotate({0.0f, -rot.y, 0.0f});
        }

        // DRAW MESH
        switch (p.type) {
        case Particle_Death: {
            if (p.variant < 8) {
                death_mesh[p.variant].bind();
                death_mesh[p.variant].draw();
            }
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
    }

    GI::set_culling_mode(true, true);
}
} // namespace CrossCraft