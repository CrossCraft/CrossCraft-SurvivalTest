#include "TNT.hpp"

namespace CrossCraft {

void TNTData::doPhysics(float dt, World *w) {
    pos.y += 1.8f;
    test_collide(w, dt);
    pos += vel * dt;
    pos.y -= 1.8f;

    vel.x *= 0.95f;
    vel.z *= 0.95f;
    vel.y -= 16.0f * dt;
}

TNT::TNT() {
    tnt_list.clear();

    idx_counter = 0;
    m_verts.clear();
    m_verts.shrink_to_fit();
    m_index.clear();
    m_index.shrink_to_fit();
    blockMesh.delete_data();

    glm::vec3 p = {0, 0, 0};
    add_face_to_mesh(topFace, getTexCoord(Block::TNT, LIGHT_TOP), LIGHT_TOP, p,
                     0);
    add_face_to_mesh(bottomFace, getTexCoord(Block::TNT, LIGHT_BOT), LIGHT_BOT,
                     p, 0);
    add_face_to_mesh(leftFace, getTexCoord(Block::TNT, LIGHT_SIDE_X),
                     LIGHT_SIDE_X, p, 0);
    add_face_to_mesh(rightFace, getTexCoord(Block::TNT, LIGHT_SIDE_X),
                     LIGHT_SIDE_X, p, 0);
    add_face_to_mesh(frontFace, getTexCoord(Block::TNT, LIGHT_SIDE_Z),
                     LIGHT_SIDE_Z, p, 0);
    add_face_to_mesh(backFace, getTexCoord(Block::TNT, LIGHT_SIDE_Z),
                     LIGHT_SIDE_Z, p, 0);
    blockMesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                       m_index.size());
}
TNT::~TNT() { tnt_list.clear(); }

void TNT::add_TNT(TNTData data) { tnt_list.push_back(data); }

void TNT::update(float dt, Player *p, World *w) {
    int toRemove = -1;

    for (int i = 0; i < tnt_list.size(); i++) {
        auto &t = tnt_list[i];
        t.fuse -= dt;

        if (t.fuse < 0 && toRemove == -1) {
            toRemove = i;
            w->explode(t.pos);
            break;
        }

        t.doPhysics(dt, w);

        auto ppos = p->pos;
        ppos.y -= 1.8f;

        auto diff = ppos - t.pos;
        auto len = sqrtf(diff.x * diff.x + diff.z * diff.z);

        if (len < 24.0f) {
            t.inRange = true;
        } else {
            t.inRange = false;
        }
    }

    if (toRemove != -1) {
        tnt_list.erase(tnt_list.begin() + toRemove);
    }
}

void TNT::draw() {
    for (auto &d : tnt_list) {
        Rendering::RenderContext::get().matrix_clear();
        Rendering::RenderContext::get().matrix_translate(
            {d.pos.x, d.pos.y, d.pos.z});

        // Set up texture
        Rendering::TextureManager::get().bind_texture(terrain_atlas);

        blockMesh.draw();
        Rendering::RenderContext::get().matrix_clear();
    }
}

auto TNT::add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                           uint32_t lightVal, glm::vec3 mypos, uint8_t type)
    -> void {
    Rendering::Color c;

    c.color = lightVal;

    // Push Back Verts
    u8 idx = 0;
    for (int i = 0, tx = 0; i < 4; i++) {

        Rendering::Vertex vert;
        vert.color = c;
        vert.u = uv[tx++];
        vert.v = uv[tx++];
        vert.x = data[idx++] + mypos.x;
        vert.y = data[idx++] + mypos.y;
        vert.z = data[idx++] + mypos.z;

        m_verts.push_back(vert);
    }

    // Push Back Indices
    m_index.push_back(idx_counter);
    m_index.push_back(idx_counter + 1);
    m_index.push_back(idx_counter + 2);
    m_index.push_back(idx_counter + 2);
    m_index.push_back(idx_counter + 3);
    m_index.push_back(idx_counter + 0);
    idx_counter += 4;
}

} // namespace CrossCraft