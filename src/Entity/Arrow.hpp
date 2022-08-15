#pragma once

#include "Entity.hpp"
#include <vector>
#include <Graphics/2D/FontRenderer.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include <Rendering/Primitive/Rectangle.hpp>
#include <any>
#include <glm.hpp>
#include "../Player/Player.hpp"
#include "../World/World.hpp"

namespace CrossCraft
{
    using namespace Stardust_Celeste;
    class Player;

    struct ArrowData : public Entity
    {
        float lifeTime;
        bool playerArrow;
        bool inRange;
        void doPhysics(float dt, World *w);
    };

    class Arrow
    {
    public:
        Arrow();
        ~Arrow();

        void add_arrow(ArrowData data);

        void update(float dt, Player *p, World *w);
        void draw();

    private:
        uint32_t texture;
        std::vector<ArrowData> arrows;
        uint16_t idx_counter;
        std::vector<Rendering::Vertex> m_verts, m_verts2;
        std::vector<uint16_t> m_index;
        Rendering::Mesh blockMesh, blockMesh2;
    };
}