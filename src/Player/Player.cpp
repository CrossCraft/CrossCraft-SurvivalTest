#include "Player.hpp"
#include "../BlockConst.hpp"
#include "../Chunk/ChunkUtil.hpp"
#include "../ResourcePackManager.hpp"
#include <Platform/Platform.hpp>
#include <Rendering/ShaderManager.hpp>
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/projection.hpp>
#include <gtx/rotate_vector.hpp>

#if PSP
#include <malloc.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <psputility.h>
#include <string.h>

namespace Stardust_Celeste::Rendering {
extern char list[0x100000] __attribute__((aligned(64)));
}
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PLAT == BUILD_3DS
#define SCREEN_W 400
#define SCREEN_H 240
#else
#define SCREEN_W 480
#define SCREEN_H 272
#endif
#define SCREEN_CENTER (SCREEN_W / 2)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace GI {
extern GLFWwindow *window;
}
#endif

#if PSP
u32 totalRamFree() {
    u32 size = 0;
    const u32 BLOCK_SIZE = 1024 * 512;

    while (true) {
        size += BLOCK_SIZE;
        u8 *ram = (u8 *)malloc(size);

        if (ram == NULL) {
            size -= BLOCK_SIZE;
            break;
        }

        free(ram);
    }

    return size;
}
#endif

namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

#if BUILD_PC
extern void character_callback(GLFWwindow *window, unsigned int codepoint);
#endif
extern std::string chat_text;
extern Player *player_ptr;

Player::Player()
    : itemSelections{SlotInfo{-1, 0},
                     SlotInfo{-1, 0},
                     SlotInfo{-1, 0},
                     SlotInfo{-1, 0},
                     SlotInfo{-1, 0},
                     SlotInfo{-1, 0},
                     SlotInfo{Block::Mushroom2, 10},
                     SlotInfo{Block::Mushroom1, 10},
                     SlotInfo{Block::TNT, 10}},
      cam(pos, glm::vec3(rot.x, rot.y, 0), 70.0f, 16.0f / 9.0f, 0.1f, 255.0f),
      model(pos, {0.6, 1.8, 0.6}) {
    gui_texture = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/gui/gui.png", SC_TEX_FILTER_NEAREST,
        SC_TEX_FILTER_NEAREST, false, true);
    icons_texture = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/gui/icons.png", SC_TEX_FILTER_NEAREST,
        SC_TEX_FILTER_NEAREST, true, true);
    water_texture = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/water.png", SC_TEX_FILTER_NEAREST,
        SC_TEX_FILTER_NEAREST, false, true);
    font_texture = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/default.png", SC_TEX_FILTER_NEAREST,
        SC_TEX_FILTER_NEAREST, false, false);

    in_tab = false;
    player_ptr = this;
    in_chat = false;
    in_chat_delta = true;
    chat_text = "";
    in_pause = false;
    hasDir = false;
    is_firing = false;
    fallDamaging = false;

    size = {0.6f, 1.8f, 0.6f};

    HP = 20;
    air = 10.0f;
    arrows = 20;
    score = 0;
    hitCD = 0.0f;

    sound_icd = 0;

    background_rectangle = create_scopeptr<Rendering::Primitive::Rectangle>(
        Rendering::Rectangle{{126, 55}, {226, 167}},
        Rendering::Color{0, 0, 0, 128}, 2);

    death_rectangle = create_scopeptr<Rendering::Primitive::Rectangle>(
        Rendering::Rectangle{{0, 0}, {480, 272}},
        Rendering::Color{127, 0, 0, 128}, 2);

    auto posX = SCREEN_CENTER - 91;

    item_box = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{posX, 1}, {182, 22}},
        Rendering::Rectangle{{0, (256.0f - 22.0f) / 256.0f},
                             {182.0f / 256.0f, 22.0f / 256.0f}});
    item_box->set_position({posX, 5});
    item_box->set_layer(-1);

    selector = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{posX - 1, 0}, {24, 24}},
        Rendering::Rectangle{{0, (256.0f - 22.0f - 24.0f) / 256.0f},
                             {24.0f / 256.0f, 24.0f / 256.0f}});

    selector->set_position({posX - 1, 4});
    selector->set_layer(-2);

    crosshair = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture,
        Rendering::Rectangle{{SCREEN_CENTER - 8, SCREEN_H / 2 - 8}, {16, 16}},
        Rendering::Rectangle{
            {(256.0f - 16.0f) / 256.0f, (256.0f - 16.0f) / 256.0f},
            {16.0f / 256.0f, 16.0f / 256.0f}});
    crosshair->set_layer(-1);

    heartBG = create_scopeptr<Graphics::G2D::Sprite>(
        icons_texture, Rendering::Rectangle{{posX - 1, 28}, {9, 9}},
        Rendering::Rectangle{{16.0f / 256.0f, (256.0f - 9.0f) / 256.0f},
                             {9.0f / 256.0f, 9.0f / 256.0f}});
    heartBG->set_layer(-1);

    heartFull = create_scopeptr<Graphics::G2D::Sprite>(
        icons_texture, Rendering::Rectangle{{posX, 29}, {7, 7}},
        Rendering::Rectangle{{53.0f / 256.0f, (256.0f - 8.0f) / 256.0f},
                             {7.0f / 256.0f, 7.0f / 256.0f}});
    heartFull->set_layer(-2);

    airContainer = create_scopeptr<Graphics::G2D::Sprite>(
        icons_texture, Rendering::Rectangle{{posX - 1, 38}, {9, 9}},
        Rendering::Rectangle{{16.0f / 256.0f, (256.0f - 27.0f) / 256.0f},
                             {9.0f / 256.0f, 9.0f / 256.0f}});
    airContainer->set_layer(-2);

    airContainerPop = create_scopeptr<Graphics::G2D::Sprite>(
        icons_texture, Rendering::Rectangle{{posX - 1, 38}, {9, 9}},
        Rendering::Rectangle{
            {(16.0f + 9.0f) / 256.0f, (256.0f - 27.0f) / 256.0f},
            {9.0f / 256.0f, 9.0f / 256.0f}});
    airContainerPop->set_layer(-2);

    heartHalf = create_scopeptr<Graphics::G2D::Sprite>(
        icons_texture, Rendering::Rectangle{{posX - 2, 29}, {7, 7}},
        Rendering::Rectangle{{60.0f / 256.0f, (256.0f - 8.0f) / 256.0f},
                             {7.0f / 256.0f, 7.0f / 256.0f}});
    heartHalf->set_layer(-2);

    water = create_scopeptr<Graphics::G2D::Sprite>(
        water_texture, Rendering::Rectangle{{0, 0}, {SCREEN_W, SCREEN_H}});
    water->set_position({0, 0});
    water->set_layer(1);

    playerHUD = create_scopeptr<UserInterface>();

    projmat = glm::perspective(DEGTORAD(70.0f), 16.0f / 9.0f, 0.1f, 255.0f);

    selectorIDX = 0;
    is_underwater = false;
    in_inventory = false;
    in_inv_delta = true;
    jump_icd = 0.2f;
    terrain_atlas = 0;
    view_timer = 0.0f;

    on_ground = false;
    jumping = false;

    in_cursor_x = 0;
    in_cursor_y = 0;

    chat = create_scopeptr<Chat>();
    blockRep = create_scopeptr<BlockRep>();
    blockRep->player_tex = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/char.png", SC_TEX_FILTER_NEAREST,
        SC_TEX_FILTER_NEAREST, false, false);
    pauseMenu = create_scopeptr<PauseMenu>();

    fps_count = 0;
    fps_timer = 0.0f;
    fps_display = 0;

    pos = {0, 0, 0};
    rot = {0, 0};
    vel = {0, 0, 0};
    acc = {0, 0, 0};
    heartTime = 0.0f;

#if BUILD_PC
    glfwSetCharCallback(GI::window, character_callback);
#endif
}

auto Player::respawn(glm::vec3 sp) -> void {
    spawnPoint = sp;
    pos = {sp.x + 0.5f, sp.y + 1, sp.z + 0.5f};
    pos.y += 1.8f;
    spawnPoint = pos;

    cam.pos = pos;
    model.pos = pos;

    HP = 20;
    arrows = 20;
    air = 10.0f;
    score = 0;
    isAlive = true;

    for (int i = 0; i < 9; i++) {
        itemSelections[i] = SlotInfo{-1, 0};
    }
    itemSelections[8] = SlotInfo{Block::TNT, 10};
}

auto Player::spawn(World *wrld) -> void {
    bool spawned = false;
    int count = 30;

    while (!spawned && count >= 0) {
        count--;
        int x = rand() % 64 - 32 + 128;
        int z = rand() % 64 - 32 + 128;

        for (int y = 50; y > 32; y--) {
            auto blk = wrld->worldData[wrld->getIdx(x, y, z)];

            if (blk != 0 && blk != 8) {
                pos = {x + 0.5f, y + 1, z + 0.5f};
                pos.y += 1.8f;
                spawnPoint = pos;

                cam.pos = pos;
                model.pos = pos;

                SC_APP_INFO("SPAWNED AT {} {} {} {}", x, y, z, blk);
                return;
            }
        }
    }

    pos = {128 + 0.5f, 40 + 1, 128 + 0.5f};
    pos.y += 1.8f;
    spawnPoint = pos;
}

const float GRAVITY_ACCELERATION = 28.0f;

void Player::update(float dt, World *wrld) {
    heartTime += dt;
    fps_timer += dt;
    fps_count++;
    if (fps_timer > 1.0f) {
        fps_timer = 0.0f;
        fps_display = fps_count;
        fps_count = 0;
    }
    hitCD -= dt;

    if (HP > 0) {
        isAlive = true;
    } else {
        isAlive = false;
    }

    if (isAlive) {

        glm::vec3 look = {0, 0, -1};
        look = glm::rotateX(look, DEGTORAD(-rot.x));
        look = glm::rotateY(look, DEGTORAD(-rot.y));

        if (is_firing && arrows != 0) {
            ArrowData data;
            data.inRange = false;
            data.pos = pos;
            data.pos.y -= (1.80f - 1.5965f);
            data.vel = look * 24.0f;
            data.rot = glm::vec2(rot.x, rot.y);
            data.size = {0.1f, 0.0f, 0.1f};
            data.lifeTime = 10.0f;
            data.playerArrow = true;

            wrld->arrow->add_arrow(data);
            is_firing = false;
            arrows--;
        }

        chat->update(dt);

        hasDir = false;
        if (!in_pause)
            rotate(dt, wrld->cfg.sense);
        jump_icd -= dt;

        // Update position
        float mag = (acc.x * acc.x + acc.z * acc.z);
        if (mag > 1.0f) {
            acc.x /= mag;
            acc.z /= mag;
        }

        vel.x += acc.x * playerSpeed;
        vel.z += acc.z * playerSpeed;

        if (!is_underwater)
            vel.y -= GRAVITY_ACCELERATION * dt;
        else
            vel.y -= GRAVITY_ACCELERATION * 0.3f * dt;
        is_falling = true;

        glm::vec3 testpos = pos + vel * dt;
        if (testpos.x < 0.5f || testpos.x > wrld->world_size.x + 0.5f) {
            vel.x = 0;
            testpos = pos + vel * dt;
        }
        if (testpos.z < 0.5f || testpos.z > wrld->world_size.z + 0.5f) {
            vel.z = 0;
            testpos = pos + vel * dt;
        }

        model.pos = pos - glm::vec3(0.3f, 1.8f, 0.3f);

        auto blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y + 0.2f,
                                                testpos.z)];
        if (blk == 8 || blk == 10)
            is_head_water = true;
        else
            is_head_water = false;

        blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y - 1.9f,
                                           testpos.z)];
        if (blk == 8 || blk == 10)
            is_underwater = true;
        else
            is_underwater = false;

        sound_icd -= dt;
        if (sound_icd < 0 && vel.x != 0 && vel.z != 0 && blk != 8 &&
            blk != 10) {
            wrld->sound_manager->play(blk, pos, true);
            sound_icd = 0.38f;
        }

        blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y - 1.2f,
                                           testpos.z)];
        if (blk == 8 || blk == 10)
            water_cutoff = true;
        else
            water_cutoff = false;

        test_collide(wrld, dt);

        if (is_falling && !fallDamaging) {
            fallDamaging = true;
            startY = pos.y;
        }

        if (fallDamaging && !is_falling) {
            fallDamaging = false;
            float totalFall = std::roundf(startY - pos.y);

            if (totalFall >= 0.0f) {
                int tF = totalFall - 3;

                if (tF > 0 && !water_cutoff)
                    HP -= tF;
            }
        }

        auto vel2 = vel;
        if (is_underwater || is_head_water || water_cutoff) {
            vel.x *= 0.5f;
            vel.z *= 0.5f;
            vel.y *= 0.7f;
        }

        if (is_head_water) {
            air -= dt;

            if (air < -1.1f) {
                air += 1.0f;
                HP -= 2;
            }
        } else {
            air = 10.0f;
        }

        pos += vel * dt;
        vel = vel2;
        vel.x *= dt;
        vel.z *= dt;

        if (vel.x < 0.1f && vel.x > -0.1f) {
            vel.x = 0.0f;
        }
        if (vel.z < 0.1f && vel.z > -0.1f) {
            vel.z = 0.0f;
        }

        blk = wrld->worldData[wrld->getIdx(pos.x, pos.y - 1.85f, pos.z)];

        on_ground = (blk != 0 && blk != 8 && blk != 10);

        if (on_ground)
            if (jumping)
                jumping = false;

        if (vel.x != 0 || vel.z != 0) {
            view_timer += dt;
        } else {
            view_timer = 0;
        }

        if (Option::get().bobbing) {
            view_bob = sinf(view_timer * 3.14159 * 3.0f) / 18.0f;
        } else {
            view_bob = 0.0f;
        }
        cube_bob = cosf(view_timer * 3.14159 * 5.0f) / 44.0f;

        // Update camera
        cam.pos = pos;
        cam.pos.y += view_bob;
        cam.pos.y -= (1.80f - 1.5965f);
        cam.rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
        cam.update();

        viewmat = glm::mat4(1.0f);
        viewmat = glm::rotate(viewmat, DEGTORAD(rot.x), {1, 0, 0});
        viewmat = glm::rotate(viewmat, DEGTORAD(rot.y), {0, 1, 0});
        viewmat = glm::translate(viewmat, -pos);

        if (in_pause) {
            pauseMenu->update();
        }

        acc.x = 0.0f;
        acc.z = 0.0f;
    }
    blockRep->update(dt);
}

auto Player::draw(World *wrld) -> void {
    if (isAlive) {
        int selectedBlock;
        selectedBlock = (in_cursor_x) + (in_cursor_y * 9);

        blockRep->terrain_atlas = terrain_atlas;

        blockRep->drawBlkHand(itemSelections[selectorIDX].type, wrld, cube_bob);

        playerHUD->begin2D();

        auto ipos = glm::ivec3(static_cast<int>(pos.x), static_cast<int>(pos.y),
                               static_cast<int>(pos.z));

        playerHUD->clear();

        if (is_head_water) {
            water->draw();
        }

        if (in_inventory) {
            playerHUD->draw_text("Select block", CC_TEXT_COLOR_WHITE,
                                 CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER, 0,
                                 7, CC_TEXT_BG_NONE);
        }

        if (itemSelections[selectorIDX].type >= 0)
            playerHUD->draw_text(
                playerHUD->get_block_name(itemSelections[selectorIDX].type),
                CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_BOTTOM,
                0, 5, CC_TEXT_BG_NONE, 0, -3);

        selector_block_prev = selectedBlock;
        selector_idx_prev = selectorIDX;
        in_inv_delta = in_inventory;

        if (in_inventory || in_tab) {
            if (in_tab) {
                Rendering::RenderContext::get().matrix_scale(
                    {1.25f, 1.25f, 1.25f});
                Rendering::RenderContext::get().matrix_translate(
                    {-46.0f, -20.875f, 0.0f});
                background_rectangle->draw();
                Rendering::RenderContext::get().matrix_clear();
            } else {
                background_rectangle->draw();
            }
        } else {
            crosshair->draw();
        }

        item_box->draw();
        selector->draw();

        playerHUD->draw_text("Position: " + std::to_string((int)ipos.x) + ", " +
                                 std::to_string((int)ipos.y) + ", " +
                                 std::to_string((int)ipos.z),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_LEFT,
                             CC_TEXT_ALIGN_TOP, 0, 0, CC_TEXT_BG_DYNAMIC);

        prev_ipos = ipos;

        if (Option::get().fps) {
            playerHUD->draw_text("FPS: " + std::to_string(fps_display),
                                 CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                                 CC_TEXT_ALIGN_TOP, 0, 0, false);

            playerHUD->draw_text("&fScore: &e" + std::to_string(score),
                                 CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                                 CC_TEXT_ALIGN_TOP, 5, -1, false);
        } else {
            playerHUD->draw_text("&fScore: &e" + std::to_string(score),
                                 CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                                 CC_TEXT_ALIGN_TOP, 5, 0, false);
        }
#if BUILD_PLAT == BUILD_3DS
        playerHUD->draw_text("Arrows: " + std::to_string(arrows),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                             CC_TEXT_ALIGN_CENTER, -21, -9, false);
#else
        playerHUD->draw_text("Arrows: " + std::to_string(arrows),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                             CC_TEXT_ALIGN_CENTER, -29, -10, false);
#endif

        int i = 9;
        for (int x = chat->data.size() - 1; x >= 0; x--) {
            auto &p = chat->data.at(x);
            if (i < 0)
                break;

            playerHUD->draw_text(p.text, CC_TEXT_COLOR_WHITE,
                                 CC_TEXT_ALIGN_LEFT, CC_TEXT_ALIGN_CENTER, 0,
                                 -i - 2, CC_TEXT_BG_NONE);
            i--;
        }
        chat_size = chat->data.size();

        if (in_chat) {
            playerHUD->draw_text("> " + chat_text, CC_TEXT_COLOR_WHITE,
                                 CC_TEXT_ALIGN_LEFT, CC_TEXT_ALIGN_BOTTOM, 0, 0,
                                 5);
        }

        chat_text_size = chat_text.size();
        in_chat_delta = in_chat;

        for (int i = 0; i < 9; i++) {
            int count = (int)itemSelections[i].quantity;
            blockRep->drawBlk(itemSelections[i].type, i, 0, 3, 10);

            if (count > 1) {
#if BUILD_PLAT == BUILD_3DS
                playerHUD->draw_text(std::to_string(count), CC_TEXT_COLOR_WHITE,
                                     CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER,
                                     0, 0, false, -77 + i * 20,
                                     -124 * 240 / 272);
#else
                playerHUD->draw_text(std::to_string(count), CC_TEXT_COLOR_WHITE,
                                     CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER,
                                     0, 0, false, -77 + i * 20, -124);
#endif
            }
        }

        if (air < 10) {
            for (int i = 0; i < air; i++) {
                float half = air - static_cast<float>(i);

                if (half <= 0.5f)
                    airContainerPop->draw();
                else
                    airContainer->draw();
                Rendering::RenderContext::get().matrix_translate(
                    {9.0f, 0.0f, 0.0f});
            }
        }
        Rendering::RenderContext::get().matrix_clear();

        playerHUD->rebuild();

        int off = 0;

        Rendering::RenderContext::get().matrix_clear();
        if (HP <= 5) {
            Rendering::RenderContext::get().matrix_translate(
                {off, sinf(heartTime * 25), 0.0f});
        } else {
            Rendering::RenderContext::get().matrix_translate({off, 0.0f, 0.0f});
        }

        for (int i = 0; i < 10; i++) {
            heartBG->draw();
            if (HP - i * 2 > 0) {
                auto diff = HP - i * 2;
                if (diff > 1 || diff == 0) {
                    heartFull->draw();
                } else {
                    heartHalf->draw();
                }
            }

            off += 9;

            Rendering::RenderContext::get().matrix_clear();
            if (HP <= 5) {
                Rendering::RenderContext::get().matrix_translate(
                    {off, sinf(i + heartTime * 25), 0.0f});
            } else {
                Rendering::RenderContext::get().matrix_translate(
                    {off, 0.0f, 0.0f});
            }
        }
        Rendering::RenderContext::get().matrix_clear();
        playerHUD->end2D();

        if (in_pause) {
            pauseMenu->draw();
        }
    } else {
        playerHUD->begin2D();
        playerHUD->clear();

        playerHUD->draw_text("You died!", CC_TEXT_COLOR_RED,
                             CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER, 0, 0,
                             0, 0, 0);
        playerHUD->draw_text("Score: " + std::to_string(score),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER,
                             CC_TEXT_ALIGN_CENTER, 0, -1, 0, 0, 0);
#if BUILD_PLAT == BUILD_3DS
        playerHUD->draw_text("Respawn", CC_TEXT_COLOR_WHITE,
                             CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER, 0, -2,
                             0, 0, -6);
#else
        playerHUD->draw_text("Respawn", CC_TEXT_COLOR_WHITE,
                             CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER, 0, -4,
                             0, 0, -6);
#endif

        playerHUD->rebuild();

#if BUILD_PLAT != BUILD_PSP && BUILD_PLAT != BUILD_3DS
        auto progID =
            Rendering::ShaderManager::get().get_current_shader().programID;
        auto location = glGetUniformLocation(progID, "drawSky");
        glUniform1i(location, 2);
#endif

        death_rectangle->draw();

#if BUILD_PLAT != BUILD_PSP && BUILD_PLAT != BUILD_3DS
        glUniform1i(location, 0);
#endif

        Rendering::RenderContext::get().matrix_translate({0, -64, 0});
        Rendering::TextureManager::get().bind_texture(pauseMenu->gui_tex);

        pauseMenu->sel_sprite->draw();
        Rendering::RenderContext::get().matrix_clear();

        playerHUD->end2D();
    }
}

void Player::OnHit(World *w, int damage, glm::vec3 from, bool player) {
    if (!isAlive)
        return;

    if (hitCD > 0)
        return;

    HP -= damage;
    if (HP < 0) {
        isAlive = false;
    }
    hitCD = 0.6f;
    vel.x += from.x * 2.5f;
    vel.z += from.z * 2.5f;
}

} // namespace CrossCraft
