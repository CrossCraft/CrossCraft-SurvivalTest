#include "../BlockConst.hpp"
#include "../Chunk/ChunkUtil.hpp"
#include "../ResourcePackManager.hpp"
#include "../World/SaveData.hpp"
#include "Player.hpp"
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

namespace GI {
extern GLFWwindow *window;
}
#endif

namespace CrossCraft {

std::string chat_text;
Player *player_ptr;
#if BUILD_PC
void character_callback(GLFWwindow *window, unsigned int codepoint) {
    if (player_ptr != nullptr) {
        if (player_ptr->in_chat) {
            chat_text.push_back((char)codepoint);
        }
    }
}
#endif

auto Player::tab_start(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->is_firing = true;
    // if (!p->in_pause)
    //     p->in_tab = true;
}
auto Player::tab_end(std::any d) -> void {
    // auto p = std::any_cast<Player *>(d);
    // p->in_tab = false;
}

auto Player::enter_chat(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (!p->in_inventory && chat_text.size() == 0 && !p->in_pause) {
        if (p->in_chat) {
            chat_text = "";
        }

        p->in_chat = !p->in_chat;

#if BUILD_PC
        if (p->in_chat)
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        Utilities::Input::set_cursor_center();
#endif
    }
}

auto Player::enter_chat_slash(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (!p->in_inventory && chat_text.size() == 0 && !p->in_pause) {
        if (p->in_chat) {
            chat_text = "";
        }

        p->in_chat = true;

#if BUILD_PC
        if (p->in_chat)
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        Utilities::Input::set_cursor_center();
#endif
    }
}
auto Player::submit_chat(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    p->in_chat = false;
    chat_text = "";

#if BUILD_PC
    glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Utilities::Input::set_cursor_center();
#endif
}

auto Player::delete_chat(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (p->in_chat) {
        if (chat_text.size() > 0) {
            chat_text.erase(chat_text.end() - 1);
        }
    }
}

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
auto Player::move_reset(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    p->vel.x = 0.0f;
    p->vel.z = 0.0f;
}

auto Player::move_forward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {
        p->vel.x += -sinf(DEGTORAD(-p->rot.y));
        p->vel.z += -cosf(DEGTORAD(-p->rot.y));
    }
}

auto Player::move_backward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {

        p->vel.x += sinf(DEGTORAD(-p->rot.y));
        p->vel.z += cosf(DEGTORAD(-p->rot.y));
    } else if (p->in_pause) {
        if (p->pauseMenu->selIdx == 0) {
            p->in_pause = false;
        } else if (p->pauseMenu->selIdx == 1) {
            SaveData::save(p->wrldRef);
        } else if (p->pauseMenu->selIdx == 2) {
            exit(0);
        }
    }
}

auto Player::move_left(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {
        p->vel.x += -sinf(DEGTORAD(-p->rot.y + 90.f));
        p->vel.z += -cosf(DEGTORAD(-p->rot.y + 90.f));
    }
}

auto Player::move_right(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {
        p->vel.x += sinf(DEGTORAD(-p->rot.y + 90.f));
        p->vel.z += cosf(DEGTORAD(-p->rot.y + 90.f));
    }
}

auto Player::move_up(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {
        if (!p->jumping && p->jump_icd < 0.0f && !p->is_underwater &&
            !p->is_falling) {
            p->vel.y = 8.4f;
            p->jumping = true;
            p->jump_icd = 0.33f;
            p->is_falling = false;
        }

        if (p->is_underwater && p->water_cutoff) {
            p->vel.y = 4.3f;
            p->jumping = true;
            p->jump_icd = 0.33f;
            p->is_falling = true;
        }
    }
}

auto Player::press_up(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (p->in_inventory && !p->in_chat && !p->in_pause) {
        p->in_cursor_y -= 1;
        if (p->in_cursor_y <= -1)
            p->in_cursor_y = 4;

        if (p->in_cursor_x >= 6 && p->in_cursor_y == 4)
            p->in_cursor_y = 3;
    } else if (p->in_pause) {
        p->pauseMenu->selIdx--;
        if (p->pauseMenu->selIdx < 0) {
            p->pauseMenu->selIdx = 0;
        }
    } else {
        move_up(d);
    }
}

auto Player::press_down(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_pause) {
        p->psp_chat();
        p->in_chat = false;
    } else if (p->in_pause) {
        p->pauseMenu->selIdx++;
        if (p->pauseMenu->selIdx > 2) {
            p->pauseMenu->selIdx = 2;
        }
    } else if (!p->in_chat && p->in_inventory) {
        p->in_cursor_y += 1;
        if (p->in_cursor_y >= 5)
            p->in_cursor_y = 0;

        if (p->in_cursor_x >= 6 && p->in_cursor_y == 4)
            p->in_cursor_y = 0;
    } else if (!p->isAlive) {
        p->respawn(p->spawnPoint);
    }
}

auto Player::press_left(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (p->in_inventory && !p->in_chat) {
        p->in_cursor_x -= 1;

        if (p->in_cursor_x <= -1) {
            if (p->in_cursor_y == 4)
                p->in_cursor_x = 5;
            else
                p->in_cursor_x = 8;
        }
    } else {
        dec_selector(d);
    }
}

auto Player::press_right(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (p->in_inventory && !p->in_chat) {
        p->in_cursor_x += 1;

        if (p->in_cursor_x >= 9 || (p->in_cursor_y == 4 && p->in_cursor_x >= 6))
            p->in_cursor_x = 0;
    } else {
        inc_selector(d);
    }
}

auto Player::change_selector(std::any d) -> void {

    auto s = std::any_cast<SelData>(d);
    if (!s.player->in_chat && !s.player->in_pause) {
        s.player->selectorIDX = s.selIDX;
        s.player->selector->set_position({148 + 20 * s.player->selectorIDX, 4});
    }
}

auto Player::pause(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (p->in_pause) {
        p->pauseMenu->exit();
        p->in_pause = false;
    } else {
        p->pauseMenu->enter();
        p->in_pause = true;
    }
}

auto Player::inc_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX++;

    if (p->selectorIDX > 8)
        p->selectorIDX = 0;

    p->selector->set_position({148 + 20 * p->selectorIDX, 4});
}

auto Player::dec_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX--;

    if (p->selectorIDX < 0)
        p->selectorIDX = 8;

    p->selector->set_position({148 + 20 * p->selectorIDX, 4});
}

auto Player::toggle_inv(std::any d) -> void {}

auto Player::move_down(std::any d) -> void {}

auto Player::rotate(float dt, float sense) -> void {
    using namespace Utilities::Input;
    // Rotate player
    const auto rotSpeed = 500.0f;
    float cX, cY;

#if BUILD_PLAT == BUILD_VITA
    cX = get_axis("Vita", "LX");
    cY = get_axis("Vita", "LY");

    if (cX <= 0.4f && cX >= -0.4f)
        cX = 0.0f;
    if (cY <= 0.4f && cY >= -0.4f)
        cY = 0.0f;

    cX *= 0.3f;
    cY *= 0.3f;
#elif BUILD_PLAT == BUILD_PSP
    cX = get_axis("PSP", "X");
    cY = get_axis("PSP", "Y");

    if (cX <= 0.4f && cX >= -0.4f)
        cX = 0.0f;
    if (cY <= 0.4f && cY >= -0.4f)
        cY = 0.0f;

    cX *= 0.3f;
    cY *= 0.3f;
#else
    cX = get_axis("Mouse", "X");
    cY = get_axis("Mouse", "Y");

    cX *= 3.0f;
    cY *= 3.0f;
#endif
    if (!in_inventory && !in_chat) {
        rot.y += cX * rotSpeed * dt * sense;
        rot.x += cY * rotSpeed * dt * sense;

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
    }
}

auto Player::psp_chat() -> void {}

} // namespace CrossCraft
