#include "Drops.hpp"

namespace CrossCraft
{
    void DropData::doPhysics(float dt, World* w) {
        glm::vec3 testpos = pos + vel * dt;

        pos.y += 1.5f;
        test_collide(testpos, w, dt);
        pos += vel * dt;
        pos.y -= 1.5f;

        vel.x *= 0.95f;
        vel.z *= 0.95f;
        vel.y -= 16.0f * dt;
    }

    Drops::Drops()
    {
        for (int i = 0; i < 50; i++)
        {
            setup_model(i);
        }

        drops.clear();
    }

    Drops::~Drops() {}

    auto Drops::add_face_to_mesh(std::array<float, 12> data,
                                 std::array<float, 8> uv, uint32_t lightVal,
                                 glm::vec3 mypos, uint8_t type)
        -> void
    { // Create color
        Rendering::Color c;
        c.color = lightVal;

        // Push Back Verts
        u8 idx = 0;
        for (int i = 0, tx = 0; i < 4; i++)
        {

            Rendering::Vertex vert;
            vert.color = c;
            vert.u = uv[tx++];
            vert.v = uv[tx++];
            vert.x = data[idx++] + mypos.x;
            vert.y = data[idx++] + mypos.y;
            vert.z = data[idx++] + mypos.z;

            m_verts[type].push_back(vert);
        }

        // Push Back Indices
        m_index[type].push_back(idx_counter[type]);
        m_index[type].push_back(idx_counter[type] + 1);
        m_index[type].push_back(idx_counter[type] + 2);
        m_index[type].push_back(idx_counter[type] + 2);
        m_index[type].push_back(idx_counter[type] + 3);
        m_index[type].push_back(idx_counter[type] + 0);
        idx_counter[type] += 4;
    }

    void Drops::add_drop(DropData data)
    {
        drops.push_back(data);
    }

    int Drops::try_pickup(DropData& d, Player* p) {
        int idx = -1;

        for (int i = 0; i < 9; i++) {
            if (p->itemSelections[i].type == d.type) {
                idx = i;
                break;
            }
        }

        if (idx < 0) {
            //Check empty slots

            for (int i = 0; i < 9; i++) {
                if (p->itemSelections[i].type == -1) {
                    p->itemSelections[i].type = d.type;
                    idx = i;
                    break;
                }
            }

            if (idx < 0) {
                return 0;
            }
        }

        auto count = p->itemSelections[idx].quantity;
        
        if (count >= 99)
            return 0;

        auto newSum = count + d.quantity;

        if (newSum <= 99) {
            p->itemSelections[idx].quantity = newSum;
            p->countChange = true;
            return 1;
        }
        else {
            auto excess = newSum - 99;
            auto diff = d.quantity - excess;
            d.quantity -= diff;
            p->itemSelections[idx].quantity += diff;
            p->countChange = true;
            return 2;
        }

        return 0;
    }

    void Drops::update(float dt, Player *p, World* w)
    {
        int toRemove = -1;
        start:
        for (int i = 0; i < drops.size(); i++)
        {
            auto& d = drops[i];

            d.animTime += dt;
            d.rot = glm::vec2(0, d.animTime * 30.0f);
            d.doPhysics(dt, w);

            auto diff = p->pos - d.pos;
            auto len = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

            if (len < 2.0f && toRemove < 0 && try_pickup(d, p) == 1) {
                toRemove = i;
            }
            else if (len < 4.0f) {
                d.pos += diff * dt * 2.0f;
            }
            else {
                //Merge check
                int mergeList = -1;
                for (int c = 0; c < drops.size(); c++) {
                    if (c != i) {
                        auto diff = d.pos - drops[c].pos;
                        auto newLen = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

                        if (newLen < 1.0f && drops[c].type == d.type) {
                            mergeList = c;
                            break;
                        }
                    }
                }

                if (mergeList >= 0) {
                    d.quantity += drops[mergeList].quantity;
                    drops.erase(drops.begin() + mergeList);
                    goto start;
                }
            }

            if (len < 24.0f * 24.0f) {
                d.inRange = true;
            }
            else {
                d.inRange = false;
            }
        }

        if (toRemove >= 0) {
            drops.erase(drops.begin() + toRemove);
        }
    }
    void Drops::draw()
    {
        for (auto &d : drops)
        {
            Rendering::RenderContext::get().matrix_translate(
                {d.pos.x, d.pos.y + sinf(d.animTime) * 0.125f, d.pos.z});

            Rendering::RenderContext::get().matrix_rotate({d.rot.x, d.rot.y, 0});
            Rendering::RenderContext::get().matrix_scale({0.25f, 0.25f, 0.25f});

            // Set up texture
            Rendering::TextureManager::get().bind_texture(terrain_atlas);

            if(d.inRange)
                blockMesh[d.type].draw();

            Rendering::RenderContext::get().matrix_clear();
        }
    }

    auto Drops::setup_model(uint8_t type) -> void
    {
        idx_counter[type] = 0;
        m_verts[type].clear();
        m_verts[type].shrink_to_fit();
        m_index[type].clear();
        m_index[type].shrink_to_fit();
        blockMesh[type].delete_data();

        glm::vec3 p = {-0.5f, -0.5f, -0.5f};

        if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40)
        {
            add_face_to_mesh(xFace1, getTexCoord(type, LIGHT_SIDE_X), LIGHT_SIDE_X,
                             p, type);
            add_face_to_mesh(xFace2, getTexCoord(type, LIGHT_SIDE_X), LIGHT_SIDE_X,
                             p, type);
        }
        else if (type == Block::Slab)
        {
            add_face_to_mesh(topFace, getTexCoord(type, LIGHT_TOP), LIGHT_TOP,
                             {0, -0.5f, 0}, type);
            add_face_to_mesh(bottomFace, getTexCoord(type, LIGHT_BOT), LIGHT_BOT,
                             p, type);
            add_face_to_mesh(leftFaceHalf, getTexCoord(type, LIGHT_SIDE_X),
                             LIGHT_SIDE_X, p, type);
            add_face_to_mesh(rightFaceHalf, getTexCoord(type, LIGHT_SIDE_X),
                             LIGHT_SIDE_X, p, type);
            add_face_to_mesh(frontFaceHalf, getTexCoord(type, LIGHT_SIDE_Z),
                             LIGHT_SIDE_Z, p, type);
            add_face_to_mesh(backFaceHalf, getTexCoord(type, LIGHT_SIDE_Z),
                             LIGHT_SIDE_Z, p, type);
        }
        else
        {

            add_face_to_mesh(topFace, getTexCoord(type, LIGHT_TOP), LIGHT_TOP, p,
                             type);
            add_face_to_mesh(bottomFace, getTexCoord(type, LIGHT_BOT), LIGHT_BOT, p,
                             type);
            add_face_to_mesh(leftFace, getTexCoord(type, LIGHT_SIDE_X), LIGHT_SIDE_X,
                             p, type);
            add_face_to_mesh(rightFace, getTexCoord(type, LIGHT_SIDE_X), LIGHT_SIDE_X,
                             p, type);
            add_face_to_mesh(frontFace, getTexCoord(type, LIGHT_SIDE_Z),
                             LIGHT_SIDE_Z, p, type);
            add_face_to_mesh(backFace, getTexCoord(type, LIGHT_SIDE_Z),
                             LIGHT_SIDE_Z, p, type);
        }

        blockMesh[type].add_data(m_verts[type].data(), m_verts[type].size(),
                                 m_index[type].data(), m_index[type].size());
    }

}