#include "Pig.hpp"
#include "../TexturePackManager.hpp"
#include "../Utils.hpp"
#include <yaml-cpp/yaml.h>
namespace CrossCraft {

Pig::Pig() {
    YAML::Node config =
        YAML::LoadFile(PLATFORM_APP_FILE_PREFIX + "models/pig.yaml");

    auto tSize = config["model"][0]["texsize"];
    auto texSize = glm::vec2(tSize[0].as<float>(), tSize[1].as<float>());

    leg.load(config["model"][1], texSize);
    torso.load(config["model"][2], texSize);
    head.load(config["model"][3], texSize);

    tex = TexturePackManager::get().load_texture(
        "assets/mob/pig.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);
}

Pig::~Pig() {}

void Pig::draw(PigData *sd) {
    auto ctx = &Rendering::RenderContext::get();
    ctx->matrix_clear();
    ctx->matrix_translate({sd->pos.x, sd->pos.y - 0.875f, sd->pos.z});
    ctx->matrix_rotate(glm::vec3(sd->rot, 0.0f));
    ctx->matrix_scale({0.9f, 0.9f, 0.9f});

    Rendering::TextureManager::get().bind_texture(tex);
    auto sval = sinf(sd->animationTime * 3.14159f * 4.0f / 3.0f) * 30.0f;

    head.draw({0.0f, 0.0f, 0.0f}, {0, 0, 0}, {1, 1, 1});
    torso.draw({0.375f, -0.25f, 0.0625f}, {0, 0, 90}, {1, 1, 1});
    leg.draw({0.3125f, -0.5f, 0.1875f}, {0, 180, sval}, {-1, 1, 1});
    leg.draw({0.3125f, -0.5f, -0.1875f}, {0, 0, sval}, {1, 1, 1});
    leg.draw({1.0625f, -0.5f, 0.1875f}, {0, 180, -sval}, {-1, 1, -1});
    leg.draw({1.0625f, -0.5, -0.1875f}, {0, 0, -sval}, {1, 1, -1});
    ctx->matrix_clear();
}
} // namespace CrossCraft