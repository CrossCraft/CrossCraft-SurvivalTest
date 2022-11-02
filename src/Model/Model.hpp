#pragma once
#include <Rendering/Mesh.hpp>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace CrossCraft {
using namespace Stardust_Celeste;

struct Model {
    void load(YAML::Node node, glm::vec2 texSize);
    void draw(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    uint16_t idx_counter;
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;
    Rendering::Mesh<Rendering::Vertex> blockMesh;
};
} // namespace CrossCraft