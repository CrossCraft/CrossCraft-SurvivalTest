#include "Zombie.hpp"
#include <yaml-cpp/yaml.h>
#include "../TexturePackManager.hpp"

namespace CrossCraft
{

    Zombie::Zombie()
    {
        YAML::Node config = YAML::LoadFile("models/zombie.yaml");

        auto tSize = config["model"][0]["texsize"];
        auto texSize = glm::vec2(tSize[0].as<float>(), tSize[1].as<float>());

        arm.load(config["model"][1], texSize);
        leg.load(config["model"][2], texSize);
        torso.load(config["model"][3], texSize);
        head.load(config["model"][4], texSize);

        tex = TexturePackManager::get().load_texture(
            "assets/mob/zombie.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
            false, false);
    }

    Zombie::~Zombie()
    {
    }

    void Zombie::draw(ZombieData sd)
    {
        auto ctx = &Rendering::RenderContext::get();
        ctx->matrix_clear();
        ctx->matrix_translate(sd.pos);
        ctx->matrix_rotate(glm::vec3(sd.rot, 0.0f));
        ctx->matrix_scale({0.9f, 0.9f, 0.9f});

        Rendering::TextureManager::get().bind_texture(tex);

        head.draw({0.0f, 0.0f, 0.0f}, {0, 0, 0}, {1, 1, 1});
        arm.draw({0.0f, -0.25f, 0.375f}, {0, 180, 90}, {-1, 1, 1});
        arm.draw({0.0f, -0.25f, -0.375f}, {0, 0, -90}, {1, 1, 1});
        torso.draw({0.0f, -0.25f, 0.0f}, {0, 0, 0}, {1, 1, 1});
        leg.draw({0.0f, -1.0f, 0.125f}, {0, 180, 0}, {-1, 1, 1});
        leg.draw({0.0f, -1.0f, -0.125f}, {0, 0, 0}, {1, 1, 1});
        ctx->matrix_clear();
    }
}