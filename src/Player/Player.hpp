/**
 * @file Player.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief Player controller
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "../Entity/Entity.hpp"
#include "../UI/TextHelper.hpp"
#include "../UI/UserInterface.hpp"
#include "../World/World.hpp"
#include "AABB.hpp"
#include "BlockRep.hpp"
#include "Chat.hpp"
#include "Graphics/2D/FontRenderer.hpp"
#include "PauseMenu.hpp"
#include <Audio/AudioListener.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include <Rendering/Primitive/Rectangle.hpp>
#include <any>
#include <glm.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {

class World;
class BlockRep;

class PlayerEntity : public Entity {
  public:
    int8_t HP;
    float air;
    uint8_t arrows;
    uint16_t score;
    bool isAlive;
};

struct SlotInfo {
    int16_t type;
    uint8_t quantity;
};
/**
 * @brief Player controller object
 *
 */
class Player : public PlayerEntity {
  public:
    /**
     * @brief Construct a new Player object
     *
     */
    Player();

    /**
     * @brief Destroy the Player object
     *
     */
    ~Player() = default;

    /**
     * @brief Updates the player position and camera object
     *
     * @param dt Delta Time
     */
    auto update(float dt, World *wrld) -> void;

    /**
     * @brief Draw the player UI
     *
     */
    auto draw(World *wrld) -> void;

    auto spawn(World *wrld) -> void;
    auto respawn(glm::vec3 spawnPoint) -> void;

    /**
     * @brief Get the position
     *
     * @return glm::vec3
     */
    inline auto get_pos() -> glm::vec3 { return pos; }

    /**
     * @brief Get the rotation
     *
     * @return glm::vec3
     */
    inline auto get_rot() -> glm::vec2 { return rot; }

    static auto move_forward(std::any p) -> void;
    static auto move_backward(std::any p) -> void;
    static auto move_left(std::any p) -> void;
    static auto move_right(std::any p) -> void;

    static auto move_up(std::any p) -> void;
    static auto move_down(std::any p) -> void;
    static auto move_reset(std::any p) -> void;

    static auto press_up(std::any p) -> void;
    static auto press_down(std::any p) -> void;
    static auto press_left(std::any p) -> void;
    static auto press_right(std::any p) -> void;

    static auto respawn(std::any p) -> void;

    static auto change_selector(std::any p) -> void;
    static auto inc_selector(std::any p) -> void;
    static auto dec_selector(std::any p) -> void;

    static auto toggle_inv(std::any p) -> void;
    static auto enter_chat(std::any p) -> void;
    static auto enter_chat_slash(std::any p) -> void;
    static auto submit_chat(std::any p) -> void;
    static auto delete_chat(std::any p) -> void;
    auto psp_chat() -> void;

    static auto tab_start(std::any p) -> void;
    static auto tab_end(std::any p) -> void;

    static auto pause(std::any p) -> void;

    int32_t selectorIDX;
    SlotInfo itemSelections[9];

    uint32_t terrain_atlas;
    bool in_inventory;
    bool in_chat;
    bool countChange;
    int in_cursor_x;
    int in_cursor_y;
    bool in_tab;
    glm::vec3 spawnPoint;
    ScopePtr<Chat> chat;

    bool is_firing;

    Rendering::Camera cam;

    glm::mat4 projmat, viewmat;

    ScopePtr<PauseMenu> pauseMenu;
    bool in_pause;

    World *wrldRef;

    ScopePtr<BlockRep> blockRep;
    float hitCD;
    void OnHit(World *w, int damage, glm::vec3 from, bool player);

  private:
    const float playerSpeed = 4.3f;
    auto rotate(float dt, float sense) -> void;

    float jump_icd;
    float view_bob, view_timer;
    float cube_bob;
    float sound_icd;

    bool fallDamaging;
    float startY;

    ScopePtr<Audio::Listener> listener;

    ScopePtr<Graphics::G2D::Sprite> item_box;
    ScopePtr<Graphics::G2D::Sprite> selector;
    ScopePtr<Graphics::G2D::Sprite> crosshair;
    ScopePtr<Graphics::G2D::Sprite> water;
    ScopePtr<Graphics::G2D::Sprite> overlay;
    ScopePtr<Graphics::G2D::Sprite> heartBG;
    ScopePtr<Graphics::G2D::Sprite> heartHalf;
    ScopePtr<Graphics::G2D::Sprite> heartFull;
    ScopePtr<Graphics::G2D::Sprite> airContainer;
    uint32_t gui_texture, icons_texture, water_texture, overlay_texture,
        font_texture;

    bool hasDir;
    AABB model;

    bool is_underwater, is_head_water, water_cutoff;
    bool on_ground, jumping;

    ScopePtr<Rendering::Primitive::Rectangle> background_rectangle;
    ScopePtr<Rendering::Primitive::Rectangle> death_rectangle;
    ScopePtr<UserInterface> playerHUD;

    bool in_inv_delta, in_chat_delta;
    glm::ivec3 prev_ipos;
    int chat_size;
    int chat_text_size;
    int selector_block_prev, selector_idx_prev;

    float fps_timer;
    int fps_count;
    int fps_display;
};

struct SelData {
    Player *player;
    int selIDX;
};

struct RespawnRequest {
    Player *player;
    World *wrld;
};

} // namespace CrossCraft
