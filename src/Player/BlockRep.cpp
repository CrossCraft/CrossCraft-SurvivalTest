#include "BlockRep.hpp"

namespace CrossCraft {

BlockRep::BlockRep() {
    isSwinging = false;
    swingTime = 0.0f;

    for (int i = 0; i < 50; i++) {
        setup_model(i);
    }

    idx_counter[50] = 0;
    blockMesh[50].vertices.clear();
    blockMesh[50].vertices.shrink_to_fit();
    blockMesh[50].indices.clear();
    blockMesh[50].indices.shrink_to_fit();
    blockMesh[50].delete_data();

    glm::vec3 p = {0, 0, 0};

    Rendering::Color c;
    c.color = LIGHT_TOP;

    // THE HAND

    // Right
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{44.0f / 64.0f, 20.0f / 32.0f, c, 0.0f, 0.25f, 0.25f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{44.0f / 64.0f, 1.0f, c, 0.0f, 0.75f, 0.25f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{40.0f / 64.0f, 1.0f, c, 0.25f, 0.75f, 0.25f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{40.0f / 64.0f, 20.0f / 32.0f, c, 0.25f, 0.0f, 0.25f});

    // Front
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{48.0f / 64.0f, 20.0f / 32.0f, c, 0.0f, 0.0f, 0.0f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{48.0f / 64.0f, 1.0f, c, 0.0f, 0.75f, 0.0f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{44.0f / 64.0f, 1.0f, c, 0.0f, 0.75f, 0.25f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{44.0f / 64.0f, 20.0f / 32.0f, c, 0.0f, 0.0f, 0.25f});

    // Left
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{52.0f / 64.0f, 20.0f / 32.0f, c, 0.25f, 0.0f, 0.0f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{52.0f / 64.0f, 1.0f, c, 0.25f, 0.75f, 0.0f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{48.0f / 64.0f, 1.0f, c, 0.0f, 0.75f, 0.0f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{48.0f / 64.0f, 20.0f / 32.0f, c, 0.0f, 0.0f, 0.0f});

    // Back
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{56.0f / 64.0f, 20.0f / 32.0f, c, 0.25f, 0.0f, 0.25f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{56.0f / 64.0f, 1.0f, c, 0.25f, 0.75f, 0.25f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{52.0f / 64.0f, 1.0f, c, 0.25f, 0.75f, 0.0f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{52.0f / 64.0f, 20.0f / 32.0f, c, 0.25f, 0.0f, 0.0f});

    // Far end
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{44.0f / 64.0f, 16.0f / 32.0f, c, 0.0f, 0.75f, 0.0f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{44.0f / 64.0f, 20.0f / 32.0f, c, 0.0f, 0.75f, 0.25f});
    blockMesh[50].vertices.push_back(Rendering::Vertex{
        48.0f / 64.0f, 20.0f / 32.0f, c, 0.25f, 0.75f, 0.25f});
    blockMesh[50].vertices.push_back(
        Rendering::Vertex{48.0f / 64.0f, 16.0f / 32.0f, c, 0.25f, 0.75f, 0.0f});

    blockMesh[50].indices.push_back(0);
    blockMesh[50].indices.push_back(1);
    blockMesh[50].indices.push_back(2);
    blockMesh[50].indices.push_back(2);
    blockMesh[50].indices.push_back(3);
    blockMesh[50].indices.push_back(0);

    blockMesh[50].indices.push_back(4);
    blockMesh[50].indices.push_back(5);
    blockMesh[50].indices.push_back(6);
    blockMesh[50].indices.push_back(6);
    blockMesh[50].indices.push_back(7);
    blockMesh[50].indices.push_back(4);

    blockMesh[50].indices.push_back(8);
    blockMesh[50].indices.push_back(9);
    blockMesh[50].indices.push_back(10);
    blockMesh[50].indices.push_back(10);
    blockMesh[50].indices.push_back(11);
    blockMesh[50].indices.push_back(8);

    blockMesh[50].indices.push_back(12);
    blockMesh[50].indices.push_back(13);
    blockMesh[50].indices.push_back(14);
    blockMesh[50].indices.push_back(14);
    blockMesh[50].indices.push_back(15);
    blockMesh[50].indices.push_back(12);

    blockMesh[50].indices.push_back(16);
    blockMesh[50].indices.push_back(17);
    blockMesh[50].indices.push_back(18);
    blockMesh[50].indices.push_back(18);
    blockMesh[50].indices.push_back(19);
    blockMesh[50].indices.push_back(16);

    blockMesh[50].setup_buffer();
}
BlockRep::~BlockRep() {}

auto BlockRep::add_face_to_mesh(std::array<float, 12> data,
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

auto BlockRep::setup_model(uint8_t type) -> void {
    idx_counter[type] = 0;
    blockMesh[type].delete_data();

    glm::vec3 p = {0, 0, 0};

    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40) {
        add_face_to_mesh(xFace1, Chunk::getTexCoord(type, LIGHT_SIDE_X),
                         LIGHT_SIDE_X, p, type);
        add_face_to_mesh(xFace2, Chunk::getTexCoord(type, LIGHT_SIDE_X),
                         LIGHT_SIDE_X, p, type);
    } else if (type == Block::Slab) {
        add_face_to_mesh(topFace, Chunk::getTexCoord(type, LIGHT_TOP),
                         LIGHT_TOP, {0, -0.5f, 0}, type);
        add_face_to_mesh(leftFaceHalf, Chunk::getTexCoord(type, LIGHT_SIDE_X),
                         LIGHT_BOT, p, type);
        add_face_to_mesh(frontFaceHalf, Chunk::getTexCoord(type, LIGHT_SIDE_Z),
                         LIGHT_SIDE_Z, p, type);
    } else {

        add_face_to_mesh(topFace, Chunk::getTexCoord(type, LIGHT_TOP),
                         LIGHT_TOP, p, type);
        add_face_to_mesh(leftFace, Chunk::getTexCoord(type, LIGHT_SIDE_X),
                         LIGHT_BOT, p, type);
        add_face_to_mesh(frontFace, Chunk::getTexCoord(type, LIGHT_SIDE_Z),
                         LIGHT_SIDE_Z, p, type);
    }

    blockMesh[type].setup_buffer();
}

auto BlockRep::update(float dt) -> void {
    swingTime -= dt;
    if (swingTime < 0) {
        isSwinging = false;
    }
}

auto BlockRep::trigger_swing() -> void {
    if (!isSwinging) {
        isSwinging = true;
        swingTime = 0.25f;
    }
}

#if BUILD_PLAT == BUILD_3DS
#define SCREEN_W 400
#define SCREEN_H 240
#else
#define SCREEN_W 480
#define SCREEN_H 272
#endif
#define SCREEN_CENTER (SCREEN_W / 2)

auto BlockRep::drawBlk(int8_t type, int x, int y, int y_offset, float scale)
    -> void {
    if (type < 0)
        return;
    Rendering::RenderContext::get().matrix_view(glm::mat4(1));

    Rendering::RenderContext::get().matrix_translate(
        {SCREEN_CENTER - 87.5f + x * 20, (8 + y * 24) + y_offset, -20});

    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40)
        Rendering::RenderContext::get().matrix_rotate({0.0f, 45.0f, 0});
    else
        Rendering::RenderContext::get().matrix_rotate({30.0f, 45.0f, 0});
    Rendering::RenderContext::get().matrix_scale({scale, scale, scale});

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);

    GI::set_culling_mode(false, true);

    blockMesh[type].draw();

    GI::set_culling_mode(true, true);

    Rendering::RenderContext::get().matrix_clear();
}

auto BlockRep::drawBlkHand(int8_t type, World *wrld, double cube_bob) -> void {
    if (type < 0)
        type = 50;

    auto ctx = &Rendering::RenderContext::get();

    if (type != 50) {
        ctx->matrix_view(glm::mat4(1.0f));
        ctx->matrix_translate(glm::vec3(0.280f, -0.7225f + cube_bob, -0.725f));
        if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40 ||
            type == 44) {
            ctx->matrix_translate({0.0f, 0.175f, 0.0f});
        }
        ctx->matrix_rotate({0, 45.0f, 0});
        ctx->matrix_scale({0.40f, 0.40f, 0.40f});
    } else {
        ctx->matrix_view(glm::mat4(1.0f));
        ctx->matrix_translate(glm::vec3(0.75f, -1.0f + cube_bob, -0.6f));
        ctx->matrix_rotate({-60.0f, 45.0f, 0});
        ctx->matrix_scale({1.0f, 1.25f, 1.0f});
    }

    if (isSwinging) {
        auto timeIn = 0.25f - swingTime;
        auto sval = sinf(timeIn * 4.0f * 3.14159f);
        ctx->matrix_translate(
            glm::vec3(sval * 0.1f * 0.75f, sval * 0.15f * 0.75f, 0.0f));
        ctx->matrix_rotate(
            glm::vec3(-sval * 0.1f * 90.0f, sval * 0.15f * 30.0f, 0.0f));
    }

    // DISABLE CULL
#if BUILD_PC || BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_3DS
    glClear(GL_DEPTH_BUFFER_BIT);
#else
    sceGuClear(GU_DEPTH_BUFFER_BIT);
#endif

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);

    bool on_shaded_block;

    int idxl = wrld->getIdxl((int)wrld->player->pos.x, (int)wrld->player->pos.y,
                             (int)wrld->player->pos.z);
    if ((wrld->lightData[idxl] >> ((int)wrld->player->pos.y % 16)) & 1)
        on_shaded_block = false;
    else
        on_shaded_block = true;

    // manipulate the model on the fly to tint when under a shadow
    if (on_shaded_block) {
        for (auto &vert : blockMesh[type].vertices) {
            vert.color.rgba.r *= 0.6;
            vert.color.rgba.g *= 0.6;
            vert.color.rgba.b *= 0.6;
        }
        blockMesh[type].delete_data();
        blockMesh[type].setup_buffer();
    }

    if (type == 50)
        GI::set_culling_mode(false, false);
    else
        GI::set_culling_mode(false, true);

    if (type == 50)
        Rendering::TextureManager::get().bind_texture(player_tex);

    blockMesh[type].draw();

    GI::set_culling_mode(true, true);

    // revert back
    if (on_shaded_block) {
        for (auto &vert : blockMesh[type].vertices) {
            vert.color.rgba.r /= 0.6;
            vert.color.rgba.g /= 0.6;
            vert.color.rgba.b /= 0.6;
        }
        blockMesh[type].delete_data();
        blockMesh[type].setup_buffer();
    }

    ctx->matrix_clear();
}

} // namespace CrossCraft
