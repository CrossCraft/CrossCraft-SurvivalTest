#include "Gamestate.hpp"
#include "Modding/Mod.hpp"
#include "MusicManager.hpp"
#include "Rendering/ShaderManager.hpp"
#include "ResourcePackManager.hpp"
#include "Utils.hpp"
#include "World/Generation/ClassicGenerator.hpp"
#include "World/Generation/CrossCraftGenerator.hpp"
#include "World/SaveData.hpp"
#include <gtc/type_ptr.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste::Utilities;

GameState *instanced_gamestate = nullptr;

void GameState::apply_controls() {
    if (instanced_gamestate != nullptr) {
        instanced_gamestate->bind_controls();
    }
}

GameState::~GameState() { on_cleanup(); }

#if BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX
const std::string vert_source = R"(
    #version 400
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aCol;
    layout (location = 2) in vec2 aTex;

    layout (std140) uniform Matrices {
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
    };
    
    out vec2 uv;
    out vec4 color;
    out vec3 position;

    void main() {
        gl_Position = proj * view * model * vec4(aPos, 1.0);
        position = gl_Position.xyz;
        uv = aTex;
        color = aCol;
    }
)";

const std::string frag_source = R"(
    #version 400
    uniform sampler2D tex;
    uniform float scroll;
    uniform int drawSky;
    uniform vec3 fogColor;
    in vec2 uv;
    in vec4 color;
    in vec3 position;

    out vec4 FragColor;

    void main() {
        vec4 texColor = texture(tex, vec2(uv.x + scroll, uv.y));
        if(drawSky == 0)
            texColor *= vec4(1.0f / 255.0f) * color;
        else if(drawSky == 1)
            texColor = vec4(0.599765f, 0.796875f, 1.0f, 1.0f);
        else if(drawSky == 2)
            texColor = vec4(1.0f / 255.0f) * color;

        float dist = abs(position.z);
        const float fogMax = (12.0f * 16.0f * 0.8);
        const float fogMin = (4.0f * 16.0f * 0.2);
        float fogFactor = (fogMax - dist) / (fogMax - fogMin);
        fogFactor = clamp(fogFactor, 0.0f, 1.0f);

        FragColor = vec4(mix(fogColor.rgb, texColor.rgb, fogFactor), texColor.a); 

        if(FragColor.a < 0.1f)
            discard;
   }
)";
#elif BUILD_PLAT == BUILD_VITA
const std::string vert_source =
    R"(
    void main ( float3 position, float4 color, float2 uv,
                float2 out vTexcoord : TEXCOORD0, 
                float4 out vPosition : POSITION, 
                float4 out vColor : COLOR, 
                uniform float4x4 proj, 
                uniform float4x4 view, 
                uniform float4x4 model)
    {
        vPosition = mul(mul(mul(float4(position, 1.f), model), view), proj);
        vTexcoord = uv;
        vColor = color;
    }
)";

const std::string frag_source =
    R"(
    float4 main(float2 vTexcoord : TEXCOORD0, float4 vColor : COLOR0, float4 coords : WPOS, uniform sampler2D tex, uniform float scroll, uniform int drawSky, uniform float3 fogColor) {

        float4 texColor = tex2D(tex, float2(vTexcoord.x + scroll, vTexcoord.y));
        if(drawSky == 0) {
            texColor *= vColor;
        } else if(drawSky == 1) {
            texColor = float4(0.599765f, 0.796875f, 1.0f, 1.0f);
        } else if(drawSky == 2) {
            texColor = vColor;
        }

        float dist = coords.z / coords.w;

        float fogMax = (6.0f * 16.0f * 0.8f);
        float fogMin = (4.0f * 16.0f * 0.2f);
        float fogFactor = (fogMax - dist) / (fogMax - fogMin);
        fogFactor = clamp(fogFactor, 0.0f, 1.0f);

        texColor.rgb = lerp(fogColor.rgb, texColor.rgb, fogFactor);

        if(texColor.a < 0.1f)
            discard;

        return texColor;
    }
)";
#endif

void GameState::on_start() {
    SC_APP_INFO("STARTING GAME STATE");

#if BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX ||                \
    BUILD_PLAT == BUILD_VITA
    auto shad =
        Rendering::ShaderManager::get().load_shader(vert_source, frag_source);
    Rendering::ShaderManager::get().bind_shader(shad);
#endif

    int num_mods = Modding::ModManager::get().get_num_mods();
    SC_APP_INFO("Loaded {} mods!", num_mods);
    instanced_gamestate = this;

    Rendering::RenderContext::get().vsync = Option::get().vsync;

    // Make a world and generate it
    world = create_scopeptr<World>(create_refptr<Player>());

    // Read config
    world->cfg = Config::loadConfig();

    if (world->cfg.oldSky) {
        Rendering::RenderContext::get().set_color(
            Rendering::Color{0x99, 0xCC, 0xFF, 0xFF});

        auto fc = glm::vec3(0.59765f, 0.796875, 1.0f);

#if BUILD_PLAT != BUILD_PSP && BUILD_PLAT != BUILD_3DS
        auto progID =
            Rendering::ShaderManager::get().get_current_shader().programID;
        auto location = glGetUniformLocation(progID, "fogColor");
        glUniform3f(location, fc.x, fc.y, fc.z);
#endif
    } else {
        Rendering::RenderContext::get().set_color(
            Rendering::Color{0xFF, 0xFF, 0xFF, 0xFF});
        auto fc = glm::vec3(1.0f, 1.0f, 1.0f);
#if BUILD_PLAT != BUILD_PSP && BUILD_PLAT != BUILD_3DS
        auto progID =
            Rendering::ShaderManager::get().get_current_shader().programID;
        auto location = glGetUniformLocation(progID, "fogColor");
        glUniform3f(location, fc.x, fc.y, fc.z);
#endif
    }

    {
        // Try Load Save -- if fails, do generation

        FILE *fptr = fopen((PLATFORM_FILE_PREFIX + "save.ccc").c_str(), "r");
        if (!fptr || !SaveData::load_world(world.get())) {
            if (world->cfg.compat)
                ClassicGenerator::generate(world.get());
            else
                CrossCraftGenerator::generate(world.get());
        }
        if (fptr != nullptr)
            fclose(fptr);

        world->spawn();
    }

    // Make new controllers
    psp_controller = new Input::PSPController();
    n3ds_controller = new Input::N3DSController();
    vita_controller = new Input::VitaController();
    key_controller = new Input::KeyboardController();
    mouse_controller = new Input::MouseController();
    n3ds_controller = new Input::N3DSController();

    // Bind our controllers
    bind_controls();

    Input::add_controller(psp_controller);
    Input::add_controller(key_controller);
    Input::add_controller(mouse_controller);
    Input::add_controller(vita_controller);
    Input::add_controller(n3ds_controller);

    Input::set_differential_mode("Mouse", true);
    Input::set_differential_mode("PSP", true);
    Input::set_differential_mode("Vita", true);
    Input::set_differential_mode("3DS", true);

    // Request 3D Mode
    Rendering::RenderContext::get().set_mode_3D();

    Modding::ModManager::set_ptr(world.get());
    Modding::ModManager::get().onStart();
}

void GameState::on_cleanup() {
    delete psp_controller;
    delete vita_controller;
    delete key_controller;
    delete mouse_controller;
    delete n3ds_controller;
}

void GameState::quit(std::any d) {
    // Exit application
    auto app = std::any_cast<Core::Application *>(d);
    app->exit();
}

void GameState::on_update(Core::Application *app, double dt) {
    MusicManager::get().update(dt);

    Input::update();
    world->update(dt);

    Modding::ModManager::get().onUpdate();
}
void GameState::on_draw(Core::Application *app, double dt) { world->draw(); }
} // namespace CrossCraft
