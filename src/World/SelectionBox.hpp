#pragma once
#include "World.hpp"

namespace CrossCraft {
class World;
class SelectionBox {
  public:
    SelectionBox();

    auto update_position(World *wrld) -> void;
    auto draw() -> void;
    
    auto draw_break(World* wrld) -> void;
  private:
    glm::ivec3 position;

    uint16_t idx_counter[11];
    std::vector<Rendering::Vertex> m_verts[11];
    std::vector<uint16_t> m_index[11];
    Rendering::Mesh blockMesh[11];


    auto add_face_to_mesh(std::array<float, 12> data,
        std::array<float, 8> uv, uint32_t lightVal,
        glm::vec3 mypos, uint8_t type) -> void;
};
} // namespace CrossCraft