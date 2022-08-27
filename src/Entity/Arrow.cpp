#include "Arrow.hpp"
#include "../ResourcePackManager.hpp"

namespace CrossCraft {
void ArrowData::doPhysics(float dt, World *w) {
    auto testpos = pos + vel * dt;

    auto castPos = glm::ivec3(testpos.x, testpos.y, testpos.z);
    auto idx = w->getIdx(castPos.x, castPos.y, castPos.z);
    if (idx > 0) {
        auto blk = w->worldData[idx];

        if (blk == Block::Air || blk == Block::Water ||
            blk == Block::Still_Lava || blk == Block::Still_Water ||
            blk == Block::Lava) {
            pos += vel * dt;
        } else {
            vel = {0, 0, 0};
        }
    }

    if (vel.x == 0 || vel.z == 0 || vel.y == 0) {
        vel.x = 0;
        vel.z = 0;
    }

    vel.y -= 9.0f * dt;

    if (vel.x == 0 || vel.z == 0)
        vel.y = 0;
}

Arrow::Arrow() {
    texture = ResourcePackManager::get().load_texture(
        "./assets/minecraft/textures/item/arrows.png", SC_TEX_FILTER_NEAREST,
        SC_TEX_FILTER_NEAREST, true);
    arrows.clear();

    Rendering::Color c;
    c.color = LIGHT_TOP;

    idx_counter = 0;

    m_verts.push_back(
        Rendering::Vertex{0.5f, 0.0f, c, xFace1[0], xFace1[1], xFace1[2]});
    m_verts.push_back(Rendering::Vertex{0.5f, 5.0f / 32.0f, c, xFace1[3],
                                        xFace1[4], xFace1[5]});
    m_verts.push_back(Rendering::Vertex{0.0f, 5.0f / 32.0f, c, xFace1[6],
                                        xFace1[7], xFace1[8]});
    m_verts.push_back(
        Rendering::Vertex{0.0f, 0.0f, c, xFace1[9], xFace1[10], xFace1[11]});

    m_verts.push_back(
        Rendering::Vertex{0.5f, 0.0f, c, xFace2[0], xFace2[1], xFace2[2]});
    m_verts.push_back(Rendering::Vertex{0.5f, 5.0f / 32.0f, c, xFace2[3],
                                        xFace2[4], xFace2[5]});
    m_verts.push_back(Rendering::Vertex{0.0f, 5.0f / 32.0f, c, xFace2[6],
                                        xFace2[7], xFace2[8]});
    m_verts.push_back(
        Rendering::Vertex{0.0f, 0.0f, c, xFace2[9], xFace2[10], xFace2[11]});

    m_verts2.push_back(Rendering::Vertex{0.5f, 10.0f / 32.0f, c, xFace1[0],
                                         xFace1[1], xFace1[2]});
    m_verts2.push_back(Rendering::Vertex{0.5f, 15.0f / 32.0f, c, xFace1[3],
                                         xFace1[4], xFace1[5]});
    m_verts2.push_back(Rendering::Vertex{0.0f, 15.0f / 32.0f, c, xFace1[6],
                                         xFace1[7], xFace1[8]});
    m_verts2.push_back(Rendering::Vertex{0.0f, 10.0f / 32.0f, c, xFace1[9],
                                         xFace1[10], xFace1[11]});

    m_verts2.push_back(Rendering::Vertex{0.5f, 10.0f / 32.0f, c, xFace2[0],
                                         xFace2[1], xFace2[2]});
    m_verts2.push_back(Rendering::Vertex{0.5f, 15.0f / 32.0f, c, xFace2[3],
                                         xFace2[4], xFace2[5]});
    m_verts2.push_back(Rendering::Vertex{0.0f, 15.0f / 32.0f, c, xFace2[6],
                                         xFace2[7], xFace2[8]});
    m_verts2.push_back(Rendering::Vertex{0.0f, 10.0f / 32.0f, c, xFace2[9],
                                         xFace2[10], xFace2[11]});

    // Push Back Indices
    m_index.push_back(0);
    m_index.push_back(1);
    m_index.push_back(2);
    m_index.push_back(2);
    m_index.push_back(3);
    m_index.push_back(0);
    idx_counter += 4;

    m_index.push_back(idx_counter + 0);
    m_index.push_back(idx_counter + 1);
    m_index.push_back(idx_counter + 2);
    m_index.push_back(idx_counter + 2);
    m_index.push_back(idx_counter + 3);
    m_index.push_back(idx_counter + 0);
    idx_counter += 4;

    blockMesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                       m_index.size());

    blockMesh2.add_data(m_verts2.data(), m_verts2.size(), m_index.data(),
                        m_index.size());
}
Arrow::~Arrow() {}

void Arrow::add_arrow(ArrowData data) { arrows.push_back(data); }

void Arrow::update(float dt, Player *p, World *w) {
    int toRemove = -1;
    for (int i = 0; i < arrows.size(); i++) {
        auto &d = arrows[i];
        d.doPhysics(dt, w);
        d.lifeTime -= dt;

        if (d.lifeTime < 0) {
            toRemove = i;
        }

        auto diff = p->pos - d.pos;
        auto len = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

        if (len < 2.0f && toRemove < 0 && d.lifeTime <= 9.5f && d.playerArrow && d.vel.x == 0 && d.vel.y == 0 && d.vel.z == 0) {
            p->arrows++;
            toRemove = i;
        } else if (len < 4.0f && d.playerArrow) {
            d.pos += diff * dt * 3.0f;
        }

        if (len < 24.0f * 24.0f) {
            d.inRange = true;
        } else {
            d.inRange = false;
        }
    }

    if (toRemove >= 0) {
        arrows.erase(arrows.begin() + toRemove);
    }
}
void Arrow::draw() {
    for (auto &a : arrows) {

        // Set up texture
        Rendering::TextureManager::get().bind_texture(texture);

#ifndef PSP
        glDisable(GL_CULL_FACE);
#else
        sceGuDisable(GU_CULL_FACE);
#endif
        Rendering::RenderContext::get().matrix_translate(
            {a.pos.x, a.pos.y, a.pos.z});

        Rendering::RenderContext::get().matrix_rotate(
            glm::vec3(270.0f, 0.0f, 180.0f - a.rot.y));
        Rendering::RenderContext::get().matrix_scale(
            glm::vec3(5.0f / 16.0f, 1.0f, 5.0f / 16.0f));
        Rendering::RenderContext::get().matrix_scale(
            glm::vec3(0.5f, 1.0f, 0.5f));
        Rendering::RenderContext::get().matrix_translate(
            glm::vec3(-0.5f, -0.5f, -0.5f));

        if (a.inRange)
            if (a.playerArrow)
                blockMesh.draw();
            else
                blockMesh2.draw();

#ifndef PSP
        glEnable(GL_CULL_FACE);
#else
        sceGuEnable(GU_CULL_FACE);
#endif
        Rendering::RenderContext::get().matrix_clear();
    }
}
} // namespace CrossCraft
