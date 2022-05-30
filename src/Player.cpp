#include "Player.hpp"
#include "BlockConst.hpp"
#include <Platform/Platform.hpp>
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtx/projection.hpp>

#if PSP
#include <pspctrl.h>
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Stardust_Celeste::Rendering {
extern GLFWwindow *window;
}
#endif

namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

Player::Player()
    : pos(128.f, 64.0f, 128.f), rot(0.f, 180.f), vel(0.f, 0.f, 0.f),
      cam(pos, glm::vec3(rot.x, rot.y, 0), 70.0f, 16.0f / 9.0f, 0.01f, 255.0f),
      is_falling(true),
      model(pos, {0.4, 1.8, 0.4}), itemSelections{1,  4,  45, 2, 5,
                                                  17, 18, 20, 44},
      inventorySelection{1,  4,  45, 2,  5,  17, 18, 20, 44, 48, 6,
                         37, 38, 39, 40, 12, 13, 19, 21, 22, 23, 24,
                         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
                         36, 14, 15, 16, 42, 41, 47, 46, 49},
      idx_counter(0) {
    gui_texture = Rendering::TextureManager::get().load_texture(
        "./assets/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    water_texture = Rendering::TextureManager::get().load_texture(
        "./assets/water.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    overlay_texture = Rendering::TextureManager::get().load_texture(
        "./assets/overlay.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    item_box = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{149, 1}, {182, 22}},
        Rendering::Rectangle{{0, (256.0f - 22.0f) / 256.0f},
                             {182.0f / 256.0f, 22.0f / 256.0f}});
    selector = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{148, 0}, {24, 24}},
        Rendering::Rectangle{{0, (256.0f - 22.0f - 24.0f) / 256.0f},
                             {24.0f / 256.0f, 24.0f / 256.0f}});
    crosshair = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{240 - 8, 136 - 8}, {16, 16}},
        Rendering::Rectangle{
            {(256.0f - 16.0f) / 256.0f, (256.0f - 16.0f) / 256.0f},
            {16.0f / 256.0f, 16.0f / 256.0f}});
    water = create_scopeptr<Graphics::G2D::Sprite>(
        water_texture, Rendering::Rectangle{{0, 0}, {480, 272}});
    overlay = create_scopeptr<Graphics::G2D::Sprite>(
        overlay_texture, Rendering::Rectangle{{120, 64}, {240, 168}});

    selectorIDX = 0;
    is_underwater = false;
    in_inventory = false;
    jump_icd = 0.2f;
    terrain_atlas = 0;
}

const auto playerSpeed = 4.3f;

auto Player::add_face_to_mesh(std::array<float, 12> data,
                              std::array<float, 8> uv, uint32_t lightVal,
                              glm::vec3 mypos) -> void { // Create color
    Rendering::Color c;
    c.color = lightVal;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {
        m_verts.push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            data[idx++] + mypos.x,
            data[idx++] + mypos.y,
            data[idx++] + mypos.z,
        });
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

auto Player::setup_model(uint8_t type) -> void {
    idx_counter = 0;
    m_verts.clear();
    m_verts.shrink_to_fit();
    m_index.clear();
    m_index.shrink_to_fit();
    blockMesh.delete_data();

    glm::vec3 p = {0, 0, 0};

    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40) {

        add_face_to_mesh(xFace1, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_SIDE, p);
        add_face_to_mesh(xFace2, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_SIDE, p);
    } else if (type == 44) {
        add_face_to_mesh(topFace, ChunkMesh::getTexCoord(type, LIGHT_TOP),
                         LIGHT_TOP, {0, -0.5, 0});
        add_face_to_mesh(leftFaceHalf, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_BOT, p);
        add_face_to_mesh(backFaceHalf, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_SIDE, {0, 0, 1});
        add_face_to_mesh(frontFaceHalf,
                         ChunkMesh::getTexCoord(type, LIGHT_SIDE), LIGHT_SIDE,
                         {0, 0, 1});
        add_face_to_mesh(backFaceHalf, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_SIDE, p);
    } else {

        add_face_to_mesh(topFace, ChunkMesh::getTexCoord(type, LIGHT_TOP),
                         LIGHT_TOP, p);
        add_face_to_mesh(leftFace, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_BOT, p);
        add_face_to_mesh(backFace, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_SIDE, {0, 0, 1});
        add_face_to_mesh(frontFace, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_SIDE, {0, 0, 1});
        add_face_to_mesh(backFace, ChunkMesh::getTexCoord(type, LIGHT_SIDE),
                         LIGHT_SIDE, p);
    }

    blockMesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                       idx_counter);
}

auto Player::move_forward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory) {
        p->vel.x += -sinf(DEGTORAD(-p->rot.y)) * playerSpeed;
        p->vel.z += -cosf(DEGTORAD(-p->rot.y)) * playerSpeed;
    }
}

auto Player::move_backward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory) {
        p->vel.x += sinf(DEGTORAD(-p->rot.y)) * playerSpeed;
        p->vel.z += cosf(DEGTORAD(-p->rot.y)) * playerSpeed;
    }
}

auto Player::move_left(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory) {
        p->vel.x += -sinf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
        p->vel.z += -cosf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
    }
}

auto Player::move_right(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory) {
        p->vel.x += sinf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
        p->vel.z += cosf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
    }
}

auto Player::move_up(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory) {
        if (!p->is_falling && p->jump_icd < 0.0f) {
            p->vel.y = 8.4f;
            p->is_falling = false;
            p->jump_icd = 0.2f;
        }

        if (p->is_underwater) {
            p->vel.y = 3.2f;
            p->is_falling = false;
        }
    }
}

auto Player::change_selector(std::any d) -> void {
    auto s = std::any_cast<SelData>(d);
    s.player->selectorIDX = s.selIDX;
}

auto Player::inc_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX++;

    if (p->selectorIDX > 8)
        p->selectorIDX = 0;
}

auto Player::dec_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX--;

    if (p->selectorIDX < 0)
        p->selectorIDX = 8;
}

auto Player::toggle_inv(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->in_inventory = !p->in_inventory;

#if BUILD_PC
    if (p->in_inventory)
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#else
    p->vcursor_x = 240;
    p->vcursor_y = 136;
#endif
}

auto Player::move_down(std::any d) -> void {
    // TODO: Sneak
}

auto Player::rotate(float dt) -> void {
    using namespace Utilities::Input;
    // Rotate player
    const auto rotSpeed = 500.0f;
    float cX, cY;

#ifndef PSP
    cX = get_axis("Mouse", "X");
    cY = get_axis("Mouse", "Y");
#else
    cX = get_axis("PSP", "X");
    cY = get_axis("PSP", "Y");

    if (cX <= 0.4f && cX >= -0.4f)
        cX = 0.0f;
    if (cY <= 0.4f && cY >= -0.4f)
        cY = 0.0f;

    cX * 0.5f;
    cY * 0.5f;
#endif
    if (!in_inventory) {
        rot.y += cX * rotSpeed * dt;
        rot.x += cY * rotSpeed * dt;

        if (rot.y > 360.0f) {
            rot.y -= 360.0f;
        }

        if (rot.y < 0.0f) {
            rot.y += 360.0f;
        }

        if (rot.x < -89.9f) {
            rot.x = -89.9f;
        }

        if (rot.x > 89.9f) {
            rot.x = 89.9f;
        }

        set_cursor_center();
    } else {
        vcursor_x += cX * 20.0f;
        vcursor_y += cY * 20.0f;

        if (vcursor_x < 0)
            vcursor_x = 0;
        else if (vcursor_x > 480)
            vcursor_x = 480;
        if (vcursor_y < 0)
            vcursor_y = 0;
        else if (vcursor_y > 272)
            vcursor_y = 272;
    }
}

const float GRAVITY_ACCELERATION = 28.0f;

auto test(glm::vec3 pos, World *wrld) -> bool {
    auto blk = wrld->worldData[wrld->getIdx(pos.x, pos.y, pos.z)];
    return blk != 0 && blk != 8 && blk != 6 && blk != 37 && blk != 38 &&
           blk != 39 && blk != 40;
}

void Player::test_collide(glm::vec3 testpos, World *wrld, float dt) {
    for (int x = -1; x <= 1; x++)
        for (int y = 0; y <= 2; y++)
            for (int z = -1; z <= 1; z++) {
                float xoff = x;
                float zoff = z;

                auto new_vec = glm::vec3(testpos.x + xoff, testpos.y - 1.8f + y,
                                         testpos.z + zoff);

                if (test(new_vec, wrld)) {
                    AABB test_box = AABB(new_vec, {1, 1, 1});

                    if (AABB::intersect(test_box, model)) {
                        vel.x = 0;
                        vel.z = 0;
                    }
                }
            }

    testpos = pos + vel * dt;

    bool collide_down = false;
    collide_down =
        collide_down || test({testpos.x, testpos.y - 1.8f, testpos.z}, wrld);

    if (collide_down && vel.y < 0) {
        vel.y = 0;
        is_falling = false;
    }

    if (test({testpos.x, testpos.y, testpos.z}, wrld)) {
        vel.y = 0;
        is_falling = true;
    }
}

void Player::update(float dt, World *wrld) {
    rotate(dt);
    jump_icd -= dt;

    // Update position
    if (!is_underwater)
        vel.y -= GRAVITY_ACCELERATION * dt;
    else
        vel.y -= GRAVITY_ACCELERATION * 0.3f * dt;
    is_falling = true;

    glm::vec3 testpos = pos + vel * dt;
    model.pos = testpos + glm::vec3(0.2f, 0, 0.2f);

    auto blk =
        wrld->worldData[wrld->getIdx(testpos.x, testpos.y + 0.2f, testpos.z)];
    if (blk == 8)
        is_head_water = true;
    else
        is_head_water = false;

    blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y - 1.5f, testpos.z)];
    if (blk == 8)
        is_underwater = true;
    else
        is_underwater = false;

    test_collide(testpos, wrld, dt);

    if (is_underwater) {
        vel.x *= 0.5f;
        vel.z *= 0.5f;
        vel.y *= 0.9f;
    }

    pos += vel * dt;

    // When the player stops falling, we make sure the player snaps to the top
    // of a surface
    if (!is_falling) {
        pos.y += 0.2f;
        pos.y = std::round(pos.y);
        pos.y -= 0.2f;
    }

    // Update camera
    cam.pos = pos;
    cam.rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
    cam.update();
    vel = glm::vec3(0.f, vel.y, 0.f);
}
// template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f
// * 3.14159; }

auto Player::drawBlk(uint8_t type, int x, int y) -> void {
    setup_model(type);

    Rendering::RenderContext::get().matrix_view(glm::mat4(1));
    Rendering::RenderContext::get().matrix_translate(
        {153.5f + x * 20, 8 + y * 24, -20});
    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40)
        Rendering::RenderContext::get().matrix_rotate({0.0f, 45.0f, 0});
    else
        Rendering::RenderContext::get().matrix_rotate({30.0f, 45.0f, 0});

    Rendering::RenderContext::get().matrix_scale({9.0f, 9.0f, 9.0f});

// DISABLE CULL
#if BUILD_PC
    glDisable(GL_CULL_FACE);
#else
    sceGuDisable(GU_CULL_FACE);
#endif

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);
    blockMesh.draw();

// ENABLE CULL
#if BUILD_PC
    glEnable(GL_CULL_FACE);
#else
    sceGuEnable(GU_CULL_FACE);
#endif

    Rendering::RenderContext::get().matrix_clear();
}

auto Player::draw() -> void {
    Rendering::RenderContext::get().matrix_ortho(0, 480, 0, 272, 30, -30);

    if (is_head_water) {
        water->set_position({0, 0});
        water->set_layer(1);
        water->draw();
    }

    if (!in_inventory) {
        crosshair->set_position({240 - 8, 136 - 8});
        crosshair->set_layer(-1);
        crosshair->draw();
    } else {
#if PSP
        crosshair->set_position({vcursor_x, vcursor_y});
        crosshair->set_layer(1);
        crosshair->draw();
#endif
    }

    if (in_inventory) {
        overlay->set_position({120, 64});
        overlay->set_layer(0);
        overlay->draw();
    }

    item_box->set_position({149, 1});
    item_box->set_layer(-1);
    item_box->draw();

    selector->set_position({148 + 20 * selectorIDX, 0});
    selector->set_layer(-2);
    selector->draw();

    for (int i = 0; i < 9; i++)
        drawBlk(itemSelections[i], i, 0);

    if (in_inventory) {
        for (int i = 0; i < 42; i++)
            drawBlk(inventorySelection[i], i % 9, 7 - i / 9);
    }
}
} // namespace CrossCraft