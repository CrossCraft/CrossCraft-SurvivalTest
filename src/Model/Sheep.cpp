#include "Sheep.hpp"
#include "../TexturePackManager.hpp"
#include "../Utils.hpp"
#include <yaml-cpp/yaml.h>
namespace CrossCraft
{

    Sheep::Sheep()
    {
        {
            YAML::Node config =
                YAML::LoadFile(PLATFORM_APP_FILE_PREFIX + "models/sheep.yaml");

            auto tSize = config["model"][0]["texsize"];
            auto texSize = glm::vec2(tSize[0].as<float>(), tSize[1].as<float>());

            leg.load(config["model"][1], texSize);
            torso.load(config["model"][2], texSize);
            head.load(config["model"][3], texSize);

            tex = TexturePackManager::get().load_texture(
                "assets/mob/sheep.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
                false, false);
            tex2 = TexturePackManager::get().load_texture(
                "assets/mob/sheep_fur.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
                false, false);
        }

        YAML::Node config =
            YAML::LoadFile(PLATFORM_APP_FILE_PREFIX + "models/sheep_fur.yaml");

        auto tSize = config["model"][0]["texsize"];
        auto texSize = glm::vec2(tSize[0].as<float>(), tSize[1].as<float>());

        legWool.load(config["model"][1], texSize);
        torsoWool.load(config["model"][2], texSize);
        headWool.load(config["model"][3], texSize);
    }

    Sheep::~Sheep() {}

    void Sheep::draw(SheepData *sd)
    {
        auto ctx = &Rendering::RenderContext::get();
        ctx->matrix_clear();
        ctx->matrix_translate({sd->pos.x, sd->pos.y - 0.75f, sd->pos.z});
        ctx->matrix_rotate(glm::vec3(sd->rot, 0.0f));
        ctx->matrix_scale({0.9f, 0.9f, 0.9f});

        Rendering::TextureManager::get().bind_texture(tex);

        head.draw({0.0f, 0.0f, 0.0f}, {0, 0, 0}, {1, 1, 1});
        torso.draw({-0.1875f, -0.25f, 0.0f}, {0, 0, 90}, {1, 1, 1});
        leg.draw({0.0f, -0.8125f, 0.1875f}, {0, 180, 0}, {-1, 1, 1});
        leg.draw({0.0f, -0.8125f, -0.1875f}, {0, 0, 0}, {1, 1, 1});
        leg.draw({ 0.65625, -0.8125f, 0.1875f }, { 0, 180, 0 }, { -1, 1, -1 });
        leg.draw({ 0.65625, -0.8125f, -0.175f}, {0, 0, 0}, {1, 1, -1});

        if (sd->hasWool)
        {
            Rendering::TextureManager::get().bind_texture(tex2);
            headWool.draw({-0.1875f, 0.0f, 0.0f}, {0, 0, 0}, {1, 1, 1});
            torsoWool.draw({-0.1875f, -0.25f, 0.0f}, {0, 0, 90}, {1, 1, 1});
            legWool.draw({0.0f, -0.8125f, 0.1875f}, {0, 180, 0}, {-1, 1, 1});
            legWool.draw({0.0f, -0.8125f, -0.1875f}, {0, 0, 0}, {1, 1, 1});
            legWool.draw({0.65625, -0.8125f, 0.1875f}, {0, 180, 0}, {-1, 1, -1});
            legWool.draw({0.65625, -0.8125f, -0.175f}, {0, 0, 0}, {1, 1, -1});
        }
        ctx->matrix_clear();
    }
} // namespace CrossCraft