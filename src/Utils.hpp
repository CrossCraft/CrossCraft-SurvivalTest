#pragma once
#include <Network/Network.hpp>
#include <Platform/Platform.hpp>
#include <Utilities/Utilities.hpp>
#include <string>

#if BUILD_PLAT != BUILD_VITA
#define PLATFORM_FILE_PREFIX std::string("")
#define PLATFORM_APP_FILE_PREFIX std::string("")
#else
#define PLATFORM_FILE_PREFIX std::string("ux0:/data/CrossCraft-ST/")
#define PLATFORM_APP_FILE_PREFIX std::string("app0:/")
#endif

#if PSP
#include <pspiofilemgr.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <filesystem>

namespace CrossCraft {
using namespace Stardust_Celeste;
inline void netInit() {
#if PSP
    Network::NetworkDriver::get().initGUI();
#elif BUILD_PLAT == BUILD_VITA
    Network::NetworkDriver::get().init();
#endif
}

#if BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_PSP
inline void sceIoCopy(std::string source, std::string dest, bool remove) {
    std::ifstream src(source, std::ios::binary);
    if (src.is_open()) {
        std::ofstream dst(dest, std::ios::binary);
        dst << src.rdbuf();
        src.close();

        if (remove)
            sceIoRemove(source.c_str());
    }
}
#endif

inline void createDirs() {
#if BUILD_PLAT == BUILD_VITA
    sceIoMkdir("ux0:/data/CrossCraft-ST", 0777);
    sceIoMkdir("ux0:/data/CrossCraft-ST/resourcepacks", 0777);
    sceIoMkdir("ux0:/data/CrossCraft-ST/mods", 0777);
    Utilities::Logger::get_app_log()->flush_output = true;
    Utilities::Logger::get_core_log()->flush_output = true;
    if (!std::filesystem::exists(
            "ux0:/data/CrossCraft-ST/resourcepacks/default")) {
        sceIoCopy("app0:/default.zip",
                  "ux0:/data/CrossCraft-ST/resourcepacks/default.zip", true);
    }
#elif BUILD_PLAT == BUILD_PSP
    sceIoMkdir("./resourcepacks", 0777);
    sceIoMkdir("./mods", 0777);
    {
        std::ifstream src("./default.zip", std::ios::binary);
        if (src.is_open()) {
            std::ofstream dst("./resourcepacks/default.zip", std::ios::binary);
            dst << src.rdbuf();
            src.close();
            sceIoRemove("./default.zip");
        }
    }
#endif
}
} // namespace CrossCraft
