#pragma once
#include <Graphics/2D/FontRenderer.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include <Rendering/Primitive/Rectangle.hpp>
#include <any>
#include <glm.hpp>
#include "../UI/TextHelper.hpp"
#include "../UI/UserInterface.hpp"
#include "../World/World.hpp"
#include "AABB.hpp"
#include "Chat.hpp"
using namespace Stardust_Celeste;

namespace CrossCraft {
    class BlockRep {
    public:
        BlockRep();
        ~BlockRep();

        auto drawBlk(int8_t type, int x, int y, int y_offset, float scale) -> void;
        auto drawBlkHand(int8_t type, World *wrld, double cube_bob) -> void;

        auto update(float dt) -> void;
        auto trigger_swing() -> void;

        uint32_t terrain_atlas;
        uint32_t player_tex;

    private:
        // Block Drawing

        auto setup_model(uint8_t type) -> void;

        auto add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv, uint32_t lightVal, glm::vec3 pos, uint8_t type)
                -> void;

        uint16_t idx_counter[51];
        Rendering::Mesh<Rendering::Vertex> blockMesh[51];

        bool isSwinging;
        float swingTime;
    };
}// namespace CrossCraft
