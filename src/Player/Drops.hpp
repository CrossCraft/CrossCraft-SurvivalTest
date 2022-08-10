#pragma once
#include "Entity.hpp"
#include <vector>
#include <Graphics/2D/FontRenderer.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include <Rendering/Primitive/Rectangle.hpp>
#include <any>
#include <glm.hpp>
#include "Player.hpp"
#include "../World/World.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;

    class Player;

    struct DropData : public Entity
    {
        uint8_t type;
        uint8_t quantity;
        float animTime;

        void doPhysics(float dt, World* w);
    };

    class Drops
    {
    public:
        Drops();
        ~Drops();

        void add_drop(DropData data);

        void update(float dt, Player *p, World* w);
        void draw();

        uint32_t terrain_atlas;

    private:
        int try_pickup(DropData& d, Player* p);
        std::vector<DropData> drops;

        auto setup_model(uint8_t type) -> void;

        auto add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                              uint32_t lightVal, glm::vec3 pos, uint8_t type)
            -> void;

        uint16_t idx_counter[50];
        std::vector<Rendering::Vertex> m_verts[50];
        std::vector<uint16_t> m_index[50];
        Rendering::Mesh blockMesh[50];
    };
}