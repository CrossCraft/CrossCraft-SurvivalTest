#include "Player.hpp"
#include "../BlockConst.hpp"
#include "../Chunk/ChunkUtil.hpp"
#include "../MP/OutPackets.hpp"
#include "../ResourcePackManager.hpp"
#include <Platform/Platform.hpp>
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

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Stardust_Celeste::Rendering {
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
    : cam(pos, glm::vec3(rot.x, rot.y, 0), DEGTORAD(70.0f), 16.0f / 9.0f, 0.1f,
          255.0f),
    model(pos, { 0.6, 1.8, 0.6 }), itemSelections{ SlotInfo{-1, 0}, SlotInfo{-1,0}, SlotInfo{-1,0}, SlotInfo{-1,0}, SlotInfo{-1,0},
                                                  SlotInfo{-1, 0}, SlotInfo{Block::Mushroom2,10}, SlotInfo{Block::Mushroom1,10}, SlotInfo{Block::TNT, 10} } {
    gui_texture = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    icons_texture = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/gui/icons.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        true, true);
    water_texture = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/water.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST, false,
        true);
    font_texture = ResourcePackManager::get().load_texture(
        "assets/minecraft/textures/default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);

    in_tab = false;
    player_ptr = this;
    in_chat = false;
    in_chat_delta = true;
    client_ref = nullptr;
    chat_text = "";
    in_pause = false;
    hasDir = false;
    is_firing = false;

    size = { 0.6f, 1.8f, 0.6f };

    HP = 20;
    arrows = 255;
    score = 0;

    sound_icd = 0;

    background_rectangle = create_scopeptr<Rendering::Primitive::Rectangle>(
        Rendering::Rectangle{{126, 55}, {226, 167}},
        Rendering::Color{0, 0, 0, 128}, 2);

    item_box = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{149, 1}, {182, 22}},
        Rendering::Rectangle{{0, (256.0f - 22.0f) / 256.0f},
                             {182.0f / 256.0f, 22.0f / 256.0f}});
    item_box->set_position({149, 5});
    item_box->set_layer(-1);

    selector = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{148, 0}, {24, 24}},
        Rendering::Rectangle{{0, (256.0f - 22.0f - 24.0f) / 256.0f},
                             {24.0f / 256.0f, 24.0f / 256.0f}});

    selector->set_position({148, 4});
    selector->set_layer(-2);

    crosshair = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{240 - 8, 136 - 8}, {16, 16}},
        Rendering::Rectangle{
            {(256.0f - 16.0f) / 256.0f, (256.0f - 16.0f) / 256.0f},
            {16.0f / 256.0f, 16.0f / 256.0f}});
    crosshair->set_layer(-1);

    heartBG = create_scopeptr<Graphics::G2D::Sprite>(
        icons_texture, Rendering::Rectangle{{148, 28}, {9, 9}},
        Rendering::Rectangle{{16.0f / 256.0f, (256.0f - 9.0f) / 256.0f},
                             {9.0f / 256.0f, 9.0f / 256.0f}});
    heartBG->set_layer(-1);

    heartFull = create_scopeptr<Graphics::G2D::Sprite>(
        icons_texture, Rendering::Rectangle{{149, 29}, {7, 7}},
        Rendering::Rectangle{{53.0f / 256.0f, (256.0f - 8.0f) / 256.0f},
                             {7.0f / 256.0f, 7.0f / 256.0f}});
    heartFull->set_layer(-2);

    heartHalf = create_scopeptr<Graphics::G2D::Sprite>(
        icons_texture, Rendering::Rectangle{{147, 29}, {7, 7}},
        Rendering::Rectangle{{60.0f / 256.0f, (256.0f - 8.0f) / 256.0f},
                             {7.0f / 256.0f, 7.0f / 256.0f}});
    heartHalf->set_layer(-2);

    water = create_scopeptr<Graphics::G2D::Sprite>(
        water_texture, Rendering::Rectangle{{0, 0}, {480, 272}});
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
    pauseMenu = create_scopeptr<PauseMenu>();
    listener = create_scopeptr<Audio::Listener>();

    fps_count = 0;
    fps_timer = 0.0f;
    fps_display = 0;

    pos = { 0, 0, 0 };
    rot = { 0, 0 };
    vel = { 0, 0, 0 };

    countChange = true;

#if BUILD_PC
    glfwSetCharCallback(Stardust_Celeste::Rendering::window,
                        character_callback);
#endif
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

                cam.pos = pos;
                model.pos = pos;

                SC_APP_INFO("SPAWNED AT {} {} {} {}", x, y, z, blk);
                return;
            }
        }
    }

    pos = {128 + 0.5f, 40 + 1, 128 + 0.5f};
    pos.y += 1.8f;
}

const float GRAVITY_ACCELERATION = 28.0f;

void Player::update(float dt, World *wrld) {
    if (wrld->client != nullptr && wrld->client->disconnected)
        return;

    fps_timer += dt;
    fps_count++;
    if (fps_timer > 1.0f) {
        fps_timer = 0.0f;
        fps_display = fps_count;
        fps_count = 0;
    }

    glm::vec3 look = { 0, 0, -1 };
    look = glm::rotateX(look, DEGTORAD(-rot.x));
    look = glm::rotateY(look, DEGTORAD(-rot.y));

    if (is_firing && arrows != 0) {
        ArrowData data;
        data.pos = pos;
        data.pos.y -= (1.80f - 1.5965f);
        data.vel = look * 24.0f;
        data.rot = glm::vec2(rot.x, rot.y);
        data.size = { 0.1f, 0.0f, 0.1f };
        data.lifeTime = 10.0f;

        wrld->arrow->add_arrow(data);
        is_firing = false;
        arrows--;
    }

    //listener->update(pos, vel, look, { 0, 1, 0 });
    chat->update(dt);

    hasDir = false;
    if (!in_pause)
        rotate(dt, wrld->cfg.sense);
    jump_icd -= dt;

    // Update position
    float mag = (vel.x * vel.x + vel.z * vel.z);
    if (mag > 0.0f) {
        vel.x *= playerSpeed / mag;
        vel.z *= playerSpeed / mag;
    }

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

    auto blk =
        wrld->worldData[wrld->getIdx(testpos.x, testpos.y + 0.2f, testpos.z)];
    if (blk == 8 || blk == 10)
        is_head_water = true;
    else
        is_head_water = false;

    blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y - 1.9f, testpos.z)];
    if (blk == 8 || blk == 10)
        is_underwater = true;
    else
        is_underwater = false;

    sound_icd -= dt;
    if (sound_icd < 0 && vel.x != 0 && vel.z != 0 && blk != 8 && blk != 10) {
        wrld->sound_manager->play(blk, pos, true);
        sound_icd = 0.38f;
    }

    blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y - 1.2f, testpos.z)];
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

            if(tF > 0)
                HP -= tF;
        }
    }

    auto vel2 = vel;
    if (is_underwater || is_head_water || water_cutoff) {
        vel.x *= 0.5f;
        vel.z *= 0.5f;
        vel.y *= 0.7f;
    }

    pos += vel * dt;
    vel = vel2;

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
    view_bob = sinf(view_timer * 3.14159 * 3.0f) / 18.0f;
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

    vel.x = 0.0f;
    vel.z = 0.0f;
}

auto Player::draw(World *wrld) -> void {
    int selectedBlock;
    selectedBlock = (in_cursor_x) + (in_cursor_y * 9);

    blockRep->terrain_atlas = terrain_atlas;

    if (wrld->client == nullptr || !wrld->client->disconnected)
        blockRep->drawBlkHand(itemSelections[selectorIDX].type, wrld, cube_bob);

    playerHUD->begin2D();

    auto ipos = glm::ivec3(static_cast<int>(pos.x), static_cast<int>(pos.y),
                           static_cast<int>(pos.z));

    bool change = in_inv_delta != in_inventory || in_chat_delta != in_chat ||
                  fps_count == 0 || prev_ipos != ipos ||
                  chat_size != chat->data.size() ||
                  selector_block_prev != selectedBlock ||
                  selector_idx_prev != selectorIDX ||
                  chat_text_size != chat_text.size() || in_tab ||
                  (wrld->client != nullptr && wrld->client->disconnected) || countChange;

    if (change)
        playerHUD->clear();

    if (change) {
        if (wrld->client != nullptr) {
            if (wrld->client->disconnected) {
                playerHUD->draw_text(wrld->client->disconnectReason,
                                     CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER,
                                     CC_TEXT_ALIGN_CENTER, 0, 0,
                                     CC_TEXT_BG_NONE);
            }
        }
    }

    if (is_head_water) {
        water->draw();
    }

    if (change) {
        if (in_inventory) {
            playerHUD->draw_text("Select block", CC_TEXT_COLOR_WHITE,
                                 CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER, 0,
                                 7, CC_TEXT_BG_NONE);
        }

        if (itemSelections[selectorIDX].type >= 0)
            playerHUD->draw_text(
                playerHUD->get_block_name(itemSelections[selectorIDX].type),
                CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_BOTTOM,
                0, 4, CC_TEXT_BG_NONE);

        selector_block_prev = selectedBlock;
        selector_idx_prev = selectorIDX;
        in_inv_delta = in_inventory;
    }

    if (change && in_tab && client_ref != nullptr) {
        std::vector<std::string> names;
        for (auto &[key, val] : client_ref->player_rep) {
            names.push_back(val.name);
        }

        for (int i = 0; i < names.size(); i++) {
            playerHUD->draw_text(names[i], CC_TEXT_COLOR_WHITE,
                                 CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER,
                                 (i % 3) * 20 - 20, (-i / 3) + 10,
                                 CC_TEXT_BG_NONE);
        }
    }

    if (in_inventory || in_tab) {
        if (in_tab) {
            Rendering::RenderContext::get().matrix_scale({1.25f, 1.25f, 1.25f});
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

    if (change) {
        playerHUD->draw_text("Position: " + std::to_string((int)ipos.x) + ", " +
                                 std::to_string((int)ipos.y) + ", " +
                                 std::to_string((int)ipos.z),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_LEFT,
                             CC_TEXT_ALIGN_TOP, 0, 0, CC_TEXT_BG_DYNAMIC);

        prev_ipos = ipos;
    }

    if (change)
        playerHUD->draw_text("FPS: " + std::to_string(fps_display),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                             CC_TEXT_ALIGN_TOP, 0, 0, false);

    if (change)
        playerHUD->draw_text("&fScore: &e" + std::to_string(score),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                             CC_TEXT_ALIGN_TOP, 5, -1, false);

    if (change)
        playerHUD->draw_text("Arrows: " + std::to_string(arrows),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                             CC_TEXT_ALIGN_CENTER, -29, -10, false);

    if (change) {
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
    }

    if (in_chat && change) {
        playerHUD->draw_text("> " + chat_text, CC_TEXT_COLOR_WHITE,
                             CC_TEXT_ALIGN_LEFT, CC_TEXT_ALIGN_BOTTOM, 0, 0, 5);
    }

    chat_text_size = chat_text.size();
    in_chat_delta = in_chat;

    if (change) {
        for (int i = 0; i < 9; i++) {
            int count = (int)itemSelections[i].quantity;
            if (count > 1) {
                playerHUD->draw_text(std::to_string(count),
                    CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER,
                    CC_TEXT_ALIGN_CENTER, i * 4 - 16, -12, false, 2 + ((19-i) / 6), -4);
            }
        }
    }


    if (change)
        playerHUD->rebuild();

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
        Rendering::RenderContext::get().matrix_translate({9.0f, 0.0f, 0.0f});
    }
    Rendering::RenderContext::get().matrix_clear();
    playerHUD->end2D();

    if (wrld->client == nullptr || !wrld->client->disconnected) {
        for (int i = 0; i < 9; i++)
            blockRep->drawBlk(itemSelections[i].type, i, 0, 4, 9.0f);
    }

    if (in_pause) {
        pauseMenu->draw();
    }
}

} // namespace CrossCraft
