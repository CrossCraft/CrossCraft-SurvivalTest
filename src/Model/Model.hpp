#pragma once
#include <yaml-cpp/yaml.h>
#include <Rendering/Mesh.hpp>
#include <vector>

namespace CrossCraft::Model {
    using namespace Stardust_Celeste;

    struct Model {
        void load(YAML::Node node, glm::vec2 texSize);
        void draw(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

        uint16_t idx_counter;
        Rendering::Mesh<Rendering::Vertex> blockMesh;
    };
}// namespace CrossCraft::Model