#include "SelectionBox.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
namespace CrossCraft {
SelectionBox::SelectionBox() {
    idx_counter[0] = 0;
    m_verts[0].clear();
    m_verts[0].shrink_to_fit();
    m_index[0].clear();
    m_index[0].shrink_to_fit();
    blockMesh[0].delete_data();

    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});

    m_index[0].push_back(0);
    m_index[0].push_back(1);
    m_index[0].push_back(2);
    m_index[0].push_back(3);
    m_index[0].push_back(0);

    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});

    m_index[0].push_back(4);
    m_index[0].push_back(5);
    m_index[0].push_back(6);
    m_index[0].push_back(7);
    m_index[0].push_back(4);

    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});

    m_index[0].push_back(8);
    m_index[0].push_back(9);
    m_index[0].push_back(10);
    m_index[0].push_back(11);
    m_index[0].push_back(8);

    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});

    m_index[0].push_back(12);
    m_index[0].push_back(13);
    m_index[0].push_back(14);
    m_index[0].push_back(15);
    m_index[0].push_back(12);

    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});

    m_index[0].push_back(16);
    m_index[0].push_back(17);
    m_index[0].push_back(18);
    m_index[0].push_back(19);
    m_index[0].push_back(16);

    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    m_verts[0].push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});

    m_index[0].push_back(20);
    m_index[0].push_back(21);
    m_index[0].push_back(22);
    m_index[0].push_back(23);
    m_index[0].push_back(20);

    blockMesh[0].add_data(m_verts[0].data(), m_verts[0].size(),
                          m_index[0].data(), m_index[0].size());

    for (int i = 1; i < 11; i++) {
        idx_counter[i] = 0;
        m_verts[i].clear();
        m_verts[i].shrink_to_fit();
        m_index[i].clear();
        m_index[i].shrink_to_fit();
        blockMesh[i].delete_data();

        auto p = glm::vec3(0, 0, 0);

        add_face_to_mesh(topFace, getTexCoord(239 + i, LIGHT_TOP), LIGHT_TOP, p,
                         i);
        add_face_to_mesh(bottomFace, getTexCoord(239 + i, LIGHT_TOP), LIGHT_TOP,
                         p, i);
        add_face_to_mesh(leftFace, getTexCoord(239 + i, LIGHT_TOP), LIGHT_TOP,
                         p, i);
        add_face_to_mesh(rightFace, getTexCoord(239 + i, LIGHT_TOP), LIGHT_TOP,
                         p, i);
        add_face_to_mesh(frontFace, getTexCoord(239 + i, LIGHT_TOP), LIGHT_TOP,
                         p, i);
        add_face_to_mesh(backFace, getTexCoord(239 + i, LIGHT_TOP), LIGHT_TOP,
                         p, i);

        blockMesh[i].add_data(m_verts[i].data(), m_verts[i].size(),
                              m_index[i].data(), m_index[i].size());
    }
};

auto SelectionBox::add_face_to_mesh(std::array<float, 12> data,
                                    std::array<float, 8> uv, uint32_t lightVal,
                                    glm::vec3 mypos, uint8_t type)
    -> void { // Create color
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

auto SelectionBox::draw_break(World *wrld) -> void {
    if (wrld->isBreaking) {

        auto ctx = &Rendering::RenderContext::get();
        ctx->matrix_clear();
        auto pos = wrld->breaking;
        ctx->matrix_translate(glm::vec3(pos.x, pos.y, pos.z));
        ctx->matrix_translate(glm::vec3(0.505f, 0.505f, 0.505f));
        ctx->matrix_scale({1.02f, 1.02f, 1.02f});
        ctx->matrix_translate(glm::vec3(-0.5f, -0.5f, -0.5f));

        if (wrld->totalTimeBreak == 0.0f)
            return;

        int i = (wrld->totalTimeBreak - wrld->timeLeftToBreak) /
                (wrld->totalTimeBreak / 10.0f);
        if (i >= 0 && i < 10) {
#if BUILD_PLAT == BUILD_PSP
            sceGuBlendFunc(GU_ADD, GU_DST_COLOR, GU_SRC_COLOR, 0, 0);
#else
            glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
#endif
            blockMesh[i + 1].draw();
#if BUILD_PLAT == BUILD_PSP
            sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
#else
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
        }

        ctx->matrix_clear();
        return;
    }
}

auto SelectionBox::update_position(World *wrld) -> void {
    position = glm::ivec3(0, 0, 0);
    auto pos = wrld->player->get_pos();
    pos.y -= (1.80f - 1.5965f);

    auto default_vec = glm::vec3(0, 0, 1);
    default_vec =
        glm::rotateX(default_vec, DEGTORAD(wrld->player->get_rot().x));
    default_vec =
        glm::rotateY(default_vec, DEGTORAD(-wrld->player->get_rot().y + 180));

    const float REACH_DISTANCE = 4.0f;
    default_vec *= REACH_DISTANCE;

    const u32 NUM_STEPS = 100;
    for (u32 i = 0; i < NUM_STEPS; i++) {
        float percentage =
            static_cast<float>(i) / static_cast<float>(NUM_STEPS);

        auto cast_pos = pos + (default_vec * percentage);

        auto ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                               static_cast<s32>(cast_pos.y),
                               static_cast<s32>(cast_pos.z));

        u32 idx = wrld->getIdx(ivec.x, ivec.y, ivec.z);

        auto blk = wrld->worldData[idx];

        if (blk == Block::Air || blk == Block::Bedrock || blk == Block::Water ||
            blk == Block::Lava)
            continue;

        if (ivec.x < 0 || ivec.x > wrld->world_size.x || ivec.y < 0 ||
            ivec.y > wrld->world_size.y || ivec.z < 0 ||
            ivec.z > wrld->world_size.z)
            return;

        if (wrld->breaking != ivec) {
            wrld->isBreaking = false;
        }

        position = ivec;
        break;
    }
}

auto SelectionBox::draw() -> void {
    auto ctx = &Rendering::RenderContext::get();

    ctx->matrix_translate(glm::vec3(position.x, position.y, position.z));
    blockMesh[0].draw_lines();

    ctx->matrix_clear();
    return;
}
} // namespace CrossCraft
