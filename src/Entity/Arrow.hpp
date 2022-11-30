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

struct ArrowData final : public Entity {
    float lifeTime;
    bool playerArrow;
    bool inRange;
    void doPhysics(float dt, World *w);
};

class Arrow final {
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
    Rendering::Mesh<Rendering::Vertex> blockMesh, blockMesh2;
};
} // namespace CrossCraft