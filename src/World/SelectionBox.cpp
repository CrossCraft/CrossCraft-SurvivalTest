#include "SelectionBox.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
namespace CrossCraft {
SelectionBox::SelectionBox() {
    idx_counter[0] = 0;
    blockMesh[0].vertices.clear();
    blockMesh[0].vertices.shrink_to_fit();
    blockMesh[0].indices.clear();
    blockMesh[0].indices.shrink_to_fit();
    blockMesh[0].delete_data();

    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});

    blockMesh[0].indices.push_back(0);
    blockMesh[0].indices.push_back(1);
    blockMesh[0].indices.push_back(2);
    blockMesh[0].indices.push_back(3);
    blockMesh[0].indices.push_back(0);

    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});

    blockMesh[0].indices.push_back(4);
    blockMesh[0].indices.push_back(5);
    blockMesh[0].indices.push_back(6);
    blockMesh[0].indices.push_back(7);
    blockMesh[0].indices.push_back(4);

    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});

    blockMesh[0].indices.push_back(8);
    blockMesh[0].indices.push_back(9);
    blockMesh[0].indices.push_back(10);
    blockMesh[0].indices.push_back(11);
    blockMesh[0].indices.push_back(8);

    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});

    blockMesh[0].indices.push_back(12);
    blockMesh[0].indices.push_back(13);
    blockMesh[0].indices.push_back(14);
    blockMesh[0].indices.push_back(15);
    blockMesh[0].indices.push_back(12);

    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});

    blockMesh[0].indices.push_back(16);
    blockMesh[0].indices.push_back(17);
    blockMesh[0].indices.push_back(18);
    blockMesh[0].indices.push_back(19);
    blockMesh[0].indices.push_back(16);

    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    blockMesh[0].vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});

    blockMesh[0].indices.push_back(20);
    blockMesh[0].indices.push_back(21);
    blockMesh[0].indices.push_back(22);
    blockMesh[0].indices.push_back(23);
    blockMesh[0].indices.push_back(20);

    blockMesh[0].setup_buffer();

    for (int i = 1; i < 11; i++) {
        idx_counter[i] = 0;
        blockMesh[i].vertices.clear();
        blockMesh[i].vertices.shrink_to_fit();
        blockMesh[i].indices.clear();
        blockMesh[i].indices.shrink_to_fit();
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

        blockMesh[i].setup_buffer();
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

        blockMesh[type].vertices.push_back(vert);
    }

    // Push Back Indices
    blockMesh[type].indices.push_back(idx_counter[type]);
    blockMesh[type].indices.push_back(idx_counter[type] + 1);
    blockMesh[type].indices.push_back(idx_counter[type] + 2);
    blockMesh[type].indices.push_back(idx_counter[type] + 2);
    blockMesh[type].indices.push_back(idx_counter[type] + 3);
    blockMesh[type].indices.push_back(idx_counter[type] + 0);
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

            GI::blend_func(GI_DST_COLOR, GI_SRC_COLOR);

            blockMesh[i + 1].draw();

            GI::blend_func(GI_SRC_ALPHA, GI_ONE_MINUS_SRC_ALPHA);
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
    blockMesh[0].draw(Rendering::PrimType::PRIM_TYPE_LINE);

    ctx->matrix_clear();
    return;
}
} // namespace CrossCraft
