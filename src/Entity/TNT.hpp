#pragma once
#include "../Player/Player.hpp"
#include "../World/World.hpp"
#include "Entity.hpp"
#include <Graphics/2D/FontRenderer.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include <Rendering/Primitive/Rectangle.hpp>
#include <any>
#include <glm.hpp>
#include <vector>

namespace CrossCraft {
using namespace Stardust_Celeste;

class Player;

struct TNTData : public Entity {
    bool inRange;
    float fuse;
    bool Killed;
    float immune;
    void doPhysics(float dt, World *w);
    void OnHit(World* w, int damage, glm::vec3 from, bool player);
};

class TNT {
  public:
    TNT();
    ~TNT();

    void add_TNT(TNTData data);

    void update(float dt, Player *p, World *w);
    void draw();

    uint32_t terrain_atlas;

    std::vector<TNTData> tnt_list;
  private:

    auto add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                          uint32_t lightVal, glm::vec3 pos, uint8_t type)
        -> void;

    uint16_t idx_counter;
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;
    Rendering::Mesh blockMesh;
};
} // namespace CrossCraft