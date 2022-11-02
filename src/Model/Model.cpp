#include "Model.hpp"
#include <array>

namespace CrossCraft {
auto getUVs(YAML::Node arr, int idx, glm::vec2 texSize)
    -> std::array<float, 8> {
    auto data = arr[idx];
    auto sp = data["offset"];
    auto ep = data["extent"];

    auto off = glm::vec2(sp[0].as<float>(), sp[1].as<float>());
    auto ext = glm::vec2(ep[0].as<float>(), ep[1].as<float>());

    auto min = off / texSize;
    auto max = (off + ext) / texSize;

    return {min.x, min.y, min.x, max.y, max.x, max.y, max.x, min.y};
}

void Model::load(YAML::Node node, glm::vec2 texSize) {
    auto anchor = node["anchor"];
    auto size = node["size"];

    auto anchorPos = glm::vec3(anchor[0].as<float>(), anchor[1].as<float>(),
                               anchor[2].as<float>());
    auto sizeVec = glm::vec3(size[0].as<float>(), size[1].as<float>(),
                             size[2].as<float>());

    auto uv = node["uv"];

    // You start furthest from the anchor position to grow the mesh around said
    // anchor
    auto start = -anchorPos;

    // Start + Size = far end
    auto endPos = start + sizeVec;

    // Color should be white
    Rendering::Color c;
    c.color = 0xFFFFFFFF;

    idx_counter = 0;
    m_verts.clear();
    m_verts.shrink_to_fit();
    m_index.clear();
    m_index.shrink_to_fit();
    blockMesh.delete_data();

    // Y Max
    auto uvs = getUVs(uv, 0, texSize);

    m_verts.push_back(
        Rendering::Vertex{uvs[0], uvs[1], c, start.x, endPos.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[2], uvs[3], c, start.x, endPos.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[4], uvs[5], c, endPos.x, endPos.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[6], uvs[7], c, endPos.x, endPos.y, start.z});

    // Y Min
    uvs = getUVs(uv, 1, texSize);

    m_verts.push_back(
        Rendering::Vertex{uvs[0], uvs[1], c, start.x, start.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[2], uvs[3], c, start.x, start.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[4], uvs[5], c, endPos.x, start.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[6], uvs[7], c, endPos.x, start.y, start.z});

    // Z Max
    uvs = getUVs(uv, 2, texSize);

    m_verts.push_back(
        Rendering::Vertex{uvs[0], uvs[1], c, start.x, endPos.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[2], uvs[3], c, start.x, start.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[4], uvs[5], c, start.x, start.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[6], uvs[7], c, start.x, endPos.y, endPos.z});

    // Z Min
    uvs = getUVs(uv, 3, texSize);

    m_verts.push_back(
        Rendering::Vertex{uvs[0], uvs[1], c, endPos.x, endPos.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[2], uvs[3], c, endPos.x, start.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[4], uvs[5], c, endPos.x, start.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[6], uvs[7], c, endPos.x, endPos.y, endPos.z});

    // X Max
    uvs = getUVs(uv, 4, texSize);

    m_verts.push_back(
        Rendering::Vertex{uvs[0], uvs[1], c, endPos.x, endPos.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[2], uvs[3], c, endPos.x, start.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[4], uvs[5], c, start.x, start.y, endPos.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[6], uvs[7], c, start.x, endPos.y, endPos.z});

    // X Min
    uvs = getUVs(uv, 5, texSize);

    m_verts.push_back(
        Rendering::Vertex{uvs[0], uvs[1], c, start.x, endPos.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[2], uvs[3], c, start.x, start.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[4], uvs[5], c, endPos.x, start.y, start.z});
    m_verts.push_back(
        Rendering::Vertex{uvs[6], uvs[7], c, endPos.x, endPos.y, start.z});

    // Add Indices
    for (int i = 0; i < 6; i++) {
        m_index.push_back(idx_counter + 0);
        m_index.push_back(idx_counter + 1);
        m_index.push_back(idx_counter + 2);
        m_index.push_back(idx_counter + 2);
        m_index.push_back(idx_counter + 3);
        m_index.push_back(idx_counter + 0);
        idx_counter += 4;
    }

    blockMesh.setup_buffer();
}

void Model::draw(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
    Rendering::RenderContext::get().matrix_push();
    Rendering::RenderContext::get().matrix_clear();

    Rendering::RenderContext::get().matrix_translate(position);
    Rendering::RenderContext::get().matrix_rotate(rotation);
    Rendering::RenderContext::get().matrix_scale(scale);

#if BUILD_PLAT != BUILD_PSP
    glDisable(GL_CULL_FACE);
#else
    sceGuDisable(GU_CULL_FACE);
#endif

    blockMesh.draw();

#if BUILD_PLAT != BUILD_PSP
    glEnable(GL_CULL_FACE);
#else
    sceGuEnable(GU_CULL_FACE);
#endif

    Rendering::RenderContext::get().matrix_pop();
}

} // namespace CrossCraft