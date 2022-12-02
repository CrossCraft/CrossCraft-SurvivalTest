#include <Platform/Platform.hpp>
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/projection.hpp>
#include <gtx/rotate_vector.hpp>
#include "../BlockConst.hpp"
#include "../Chunk/ChunkUtil.hpp"
#include "../Controls.hpp"
#include "../Gamestate.hpp"
#include "../ResourcePackManager.hpp"
#include "../World/SaveData.hpp"
#include "Player.hpp"

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
                chat_text.push_back((char) codepoint);
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

    template<typename T>
    constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
    auto Player::move_reset(std::any d) -> void {
        auto p = std::any_cast<Player *>(d);

        p->acc.x = 0.0f;
        p->acc.z = 0.0f;
    }

    auto Player::move_forward(std::any d) -> void {
        auto p = std::any_cast<Player *>(d);

        if (!p->in_inventory && !p->in_chat && !p->in_pause) {
#if PSP || BUILD_PLAT == BUILD_3DS
            if (!Controls::get().pspJoystickView) {
                p->rot.x -= 5.0f;
                if (p->rot.x < -89.9f) {
                    p->rot.x = -89.9f;
                }

                if (p->rot.x > 89.9f) {
                    p->rot.x = 89.9f;
                }
            } else {
                p->acc.x += -sinf(DEGTORAD(-p->rot.y));
                p->acc.z += -cosf(DEGTORAD(-p->rot.y));
            }
#else

            p->acc.x += -sinf(DEGTORAD(-p->rot.y));
            p->acc.z += -cosf(DEGTORAD(-p->rot.y));
#endif
        }
    }

    auto Player::move_backward(std::any d) -> void {
        auto p = std::any_cast<Player *>(d);
        if (!p->in_inventory && !p->in_chat && !p->in_pause) {

#if PSP || BUILD_PLAT == BUILD_3DS
            if (!Controls::get().pspJoystickView) {
                p->rot.x += 5.0f;
                if (p->rot.x < -89.9f) {
                    p->rot.x = -89.9f;
                }

                if (p->rot.x > 89.9f) {
                    p->rot.x = 89.9f;
                }
            } else {
                p->acc.x += sinf(DEGTORAD(-p->rot.y));
                p->acc.z += cosf(DEGTORAD(-p->rot.y));
            }
#elif BUILD_PLAT != BUILD_VITA
            p->acc.x += sinf(DEGTORAD(-p->rot.y));
            p->acc.z += cosf(DEGTORAD(-p->rot.y));
#endif
        } else if (p->in_pause) {
            if (p->pauseMenu->pauseState == 0) {
                if (p->pauseMenu->selIdx == 0) {
                    p->in_pause = false;
                    p->pauseMenu->exit();
                } else if (p->pauseMenu->selIdx == 1) {
                    p->pauseMenu->pauseState = 1;
                } else if (p->pauseMenu->selIdx == 2) {
                    SaveData::save(p->wrldRef);
                } else if (p->pauseMenu->selIdx == 3) {
                    exit(0);
                }
            } else if (p->pauseMenu->pauseState == 1) {
                if (p->pauseMenu->selIdx == 0) {
                    Option::get().music = !Option::get().music;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 1) {
                    Option::get().renderDist++;
                    if (Option::get().renderDist > 3) {
                        Option::get().renderDist = 0;
                    }
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 2) {
                    Option::get().bobbing = !Option::get().bobbing;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 3) {
                    Option::get().sound = !Option::get().sound;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 4) {
                    Option::get().fps = !Option::get().fps;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 5) {
                    Option::get().vsync = !Option::get().vsync;
                    Rendering::RenderContext::get().vsync = Option::get().vsync;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 6) {
                    p->pauseMenu->pauseState = 2;
                } else if (p->pauseMenu->selIdx == 7) {
                    p->pauseMenu->pauseState = 0;
                }
            } else if (p->pauseMenu->pauseState == 2) {
                if (p->pauseMenu->selIdx == 0) {
                    auto val = Controls::get().getNextKey();
                    if (val != 0) {
                        Controls::get().buttonBreak = val;
                    }
                } else if (p->pauseMenu->selIdx == 1) {
                    auto val = Controls::get().getNextKey();
                    if (val != 0) {
                        Controls::get().buttonPlace = val;
                    }
                } else if (p->pauseMenu->selIdx == 2) {
                    auto val = Controls::get().getNextKey();
                    if (val != 0) {
                        Controls::get().buttonMenu = val;
                    }
                } else if (p->pauseMenu->selIdx == 3) {
                    auto val = Controls::get().getNextKey();
                    if (val != 0) {
                        Controls::get().buttonJump = val;
                    }
                } else if (p->pauseMenu->selIdx == 4) {
#if PSP || BUILD_PLAT == BUILD_3DS
                    Controls::get().pspJoystickView =
                            !Controls::get().pspJoystickView;
#elif BUILD_PLAT == BUILD_VITA
                    Controls::get().vitaJoystickSwap =
                            !Controls::get().vitaJoystickSwap;
#endif
                } else if (p->pauseMenu->selIdx == 5) {
                    p->pauseMenu->pauseState = 1;
                }
                Controls::get().writeControls();
                GameState::apply_controls();
            }
            delay(100);
            return;
        }
    }

    auto Player::move_left(std::any d) -> void {
        auto p = std::any_cast<Player *>(d);
        if (!p->in_inventory && !p->in_chat && !p->in_pause) {
#if PSP || BUILD_PLAT == BUILD_3DS
            if (!Controls::get().pspJoystickView) {
                p->rot.y -= 5.0f;
                if (p->rot.y > 360.0f) {
                    p->rot.y -= 360.0f;
                }

                if (p->rot.y < 0.0f) {
                    p->rot.y += 360.0f;
                }

            } else {
                p->acc.x += -sinf(DEGTORAD(-p->rot.y + 90.f));
                p->acc.z += -cosf(DEGTORAD(-p->rot.y + 90.f));
            }
#else
            p->acc.x += -sinf(DEGTORAD(-p->rot.y + 90.f));
            p->acc.z += -cosf(DEGTORAD(-p->rot.y + 90.f));
#endif
        }
    }

    auto Player::move_right(std::any d) -> void {
        auto p = std::any_cast<Player *>(d);
        if (!p->in_inventory && !p->in_chat && !p->in_pause) {
#if PSP || BUILD_PLAT == BUILD_3DS
            if (!Controls::get().pspJoystickView) {
                p->rot.y += 5.0f;
                if (p->rot.y > 360.0f) {
                    p->rot.y -= 360.0f;
                }

                if (p->rot.y < 0.0f) {
                    p->rot.y += 360.0f;
                }

            } else {
                p->acc.x += sinf(DEGTORAD(-p->rot.y + 90.f));
                p->acc.z += cosf(DEGTORAD(-p->rot.y + 90.f));
            }
#else
            p->acc.x += sinf(DEGTORAD(-p->rot.y + 90.f));
            p->acc.z += cosf(DEGTORAD(-p->rot.y + 90.f));
#endif
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
            if (p->pauseMenu->pauseState == 0) {
                if (p->pauseMenu->selIdx > 3) {
                    p->pauseMenu->selIdx = 3;
                }
            } else if (p->pauseMenu->pauseState == 1) {
                if (p->pauseMenu->selIdx > 7) {
                    p->pauseMenu->selIdx = 7;
                }
            } else if (p->pauseMenu->pauseState == 2) {
                if (p->pauseMenu->selIdx > 5) {
                    p->pauseMenu->selIdx = 5;
                }
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

#if BUILD_PLAT == BUILD_3DS
#define SCREEN_W 400
#define SCREEN_H 240
#else
#define SCREEN_W 480
#define SCREEN_H 272
#endif
#define SCREEN_CENTER (SCREEN_W / 2)

    auto Player::inc_selector(std::any d) -> void {
        auto p = std::any_cast<Player *>(d);
        p->selectorIDX++;

        if (p->selectorIDX > 8)
            p->selectorIDX = 0;

        p->selector->set_position({SCREEN_CENTER - 91 + 20 * p->selectorIDX, 4});
    }

    auto Player::dec_selector(std::any d) -> void {
        auto p = std::any_cast<Player *>(d);
        p->selectorIDX--;

        if (p->selectorIDX < 0)
            p->selectorIDX = 8;

        p->selector->set_position({SCREEN_CENTER - 92 + 20 * p->selectorIDX, 4});
    }

    auto Player::toggle_inv(std::any d) -> void {}

    auto Player::move_down(std::any d) -> void {}

    auto Player::rotate(float dt, float sense) -> void {
        using namespace Utilities::Input;
        // Rotate player
        const auto rotSpeed = 500.0f;
        float cX, cY;

        float mX, mY;

#if BUILD_PLAT == BUILD_VITA
        if (Controls::get().vitaJoystickSwap) {
            cX = get_axis("Vita", "LX");
            cY = get_axis("Vita", "LY");
        } else {
            cX = get_axis("Vita", "RX");
            cY = get_axis("Vita", "RY");
        }

        if (cX <= 0.4f && cX >= -0.4f)
            cX = 0.0f;
        if (cY <= 0.4f && cY >= -0.4f)
            cY = 0.0f;

        cX *= 0.3f;
        cY *= 0.3f;

        if (!Controls::get().vitaJoystickSwap) {
            mX = get_axis("Vita", "LX");
            mY = get_axis("Vita", "LY");
        } else {
            mX = get_axis("Vita", "RX");
            mY = get_axis("Vita", "RY");
        }

        if (mX > 0.5f || mX < -0.5f) {
            acc.x += sinf(DEGTORAD(-rot.y + 90.0f)) * mX;
            acc.z += cosf(DEGTORAD(-rot.y + 90.0f)) * mX;
        }

        if (mY > 0.5f || mY < -0.5f) {
            acc.x += sinf(DEGTORAD(-rot.y)) * mY;
            acc.z += cosf(DEGTORAD(-rot.y)) * mY;
        }

#elif BUILD_PLAT == BUILD_PSP
        cX = 0.0f;
        cY = 0.0f;
        if (Controls::get().pspJoystickView) {
            cX = get_axis("PSP", "X");
            cY = get_axis("PSP", "Y");

            if (cX <= 0.4f && cX >= -0.4f)
                cX = 0.0f;
            if (cY <= 0.4f && cY >= -0.4f)
                cY = 0.0f;

            cX *= 0.3f;
            cY *= 0.3f;
        } else {
            mX = get_axis("PSP", "X");
            mY = get_axis("PSP", "Y");
            if (mX > 0.5f || mX < -0.5f) {
                acc.x += sinf(DEGTORAD(-rot.y + 90.0f)) * mX;
                acc.z += cosf(DEGTORAD(-rot.y + 90.0f)) * mX;
            }

            if (mY > 0.5f || mY < -0.5f) {
                acc.x += sinf(DEGTORAD(-rot.y)) * mY;
                acc.z += cosf(DEGTORAD(-rot.y)) * mY;
            }
        }

#elif BUILD_PLAT == BUILD_3DS
        cX = 0.0f;
        cY = 0.0f;
        if (Controls::get().pspJoystickView) {
            cX = get_axis("3DS", "LX");
            cY = get_axis("3DS", "LY");

            if (cX <= 0.4f && cX >= -0.4f)
                cX = 0.0f;
            if (cY <= 0.4f && cY >= -0.4f)
                cY = 0.0f;

            cX *= 0.3f;
            cY *= -0.3f;
        } else {
            mX = get_axis("3DS", "LX");
            mY = -get_axis("3DS", "LY");

            if (mX > 0.5f || mX < -0.5f) {
                vel.x += sinf(DEGTORAD(-rot.y + 90.0f)) * mX;
                vel.z += cosf(DEGTORAD(-rot.y + 90.0f)) * mX;
            }

            if (mY > 0.5f || mY < -0.5f) {
                vel.x += sinf(DEGTORAD(-rot.y)) * mY;
                vel.z += cosf(DEGTORAD(-rot.y)) * mY;
            }
        }

#else
        cX = get_axis("Mouse", "X");
        cY = get_axis("Mouse", "Y");

        int rx, ry;
        glfwGetWindowSize(GI::window, &rx, &ry);
        cX *= (float) rx / 1024.0f;
        cX /= dt * 10.0f;
        cY *= (float) ry / 1024.0f;
        cY /= dt * 10.0f;
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

}// namespace CrossCraft
