#include "ResourcePackManager.hpp"
#include "Utils.hpp"
#include <Rendering/Rendering.hpp>
#include <algorithm>
#include <contrib/minizip/unzip.h>
#include <fstream>

namespace CrossCraft {
using namespace Stardust_Celeste;

ResourcePackManager::ResourcePackManager() {
    path_names.clear();
    layers.push_back("default");

    std::ifstream fs(PLATFORM_FILE_PREFIX + "resourcepacks.txt");
    if (fs.is_open()) {
        std::string line;
        while (std::getline(fs, line)) {
            layers.push_back(line.substr(0, line.find_first_of('\n')));
        }
    }
    fs.close();
}


auto ResourcePackManager::write_config() -> void {
    std::ofstream file(PLATFORM_FILE_PREFIX + "resourcepacks.txt");

    for (auto& l : layers) {
        file << l << "\n";
    }

    file.close();
}

ResourcePackManager::~ResourcePackManager() {}

auto ResourcePackManager::get_path() -> std::string {
    for (int i = layers.size() - 1; i >= 0; i--) {
        auto &l = layers[i];

        std::string path =
            PLATFORM_FILE_PREFIX + "resourcepacks/" + l + "/";

        return path;
    }

    return "";
}

auto ResourcePackManager::get_sound(std::string filename) -> std::string {

    for (int i = layers.size() - 1; i >= 0; i--) {
        auto &l = layers[i];

        std::string path =
            PLATFORM_FILE_PREFIX + "resourcepacks/" + l + "/" + filename;
        std::fstream file(path);

        if (file.is_open()) {
            file.close();

            return path;
        }
    }

    return "";
}

auto ResourcePackManager::load_texture(std::string filename, u32 magFilter,
                                      u32 minFilter, bool repeat, bool flip,
                                      bool vram) -> u32 {

    for (int i = layers.size() - 1; i >= 0; i--) {
        auto &l = layers[i];

        std::string path =
            PLATFORM_FILE_PREFIX + "resourcepacks/" + l + "/" + filename;
        std::fstream file(path);

        if (file.is_open()) {
            file.close();

            return Rendering::TextureManager::get().load_texture(
                path, magFilter, minFilter, repeat, flip, vram);
        }
    }

    return -1;
}

auto ResourcePackManager::add_layer(std::string name) -> void {
    layers.push_back(name);
}

#if BUILD_PLAT == BUILD_PSP
    static int sceIoMove(const char *src, const char *dest) {
        int ret = 0;
        size_t i = 0;
        char strage[32] = {0};
        char *p1 = nullptr, *p2 = nullptr;
        p1 = std::strchr(src, ':');

        if (p1 == nullptr)
            return -1;

        p2 = std::strchr(dest, ':');
        if (p2 == nullptr)
            return -1;

        if ((p1 - src) != (p2 - dest))
            return -1;

        for (i = 0; (src + i) <= p1; i++) {
            if ((i+1) >= sizeof(strage))
                return -1;

            if (src[i] != dest[i])
                return -1;

            strage[i] = src[i];
        }

        strage[i] = '\0';

        u32 data[2] = {0};
        data[0] = (u32)(p1 + 1);
        data[1] = (u32)(p2 + 1);

        ret = sceIoDevctl(strage, 0x02415830, &data, sizeof(data), nullptr, 0);

        return ret;
    }
#endif

auto ResourcePackManager::convert_old_resourcepacks() -> void {
    std::vector<std::string> old_paths;

    // See if a 'texturepacks' folder exists (version 1)
    std::filesystem::path p("texturepacks/");

    // It does, version 1 conversion.
    if (std::filesystem::exists(p)) {
        // Copy of scan_folder to look for old texturepacks
        for (const auto &entry : std::filesystem::directory_iterator(p)) {
            if (entry.is_directory()) {
                // Add to list
                old_paths.push_back(entry.path().filename().string());
            } else if (entry.is_regular_file()) {
                auto filename = entry.path().filename().string();

                if (filename.find(".zip") != std::string::npos) {
                    extract_zip("texturepacks/" + filename);
                    old_paths.push_back(entry.path().filename().string().substr(
                        0, filename.find(".zip")));
                }
            }
        }
        // Iterate through all of the found TPs
        for (auto path : old_paths) {
            // Give them some metadata text
            std::ofstream nametxt(PLATFORM_FILE_PREFIX + "texturepacks/" + path
                                    + "/name.txt");
            nametxt << path << std::endl;
            nametxt.close();

            std::ofstream packtxt(PLATFORM_FILE_PREFIX + "texturepacks/" + path
                                    + "/pack.txt");
            packtxt << "Converted from an older version." << std::endl;
            packtxt.close();

            std::ofstream versiontxt(PLATFORM_FILE_PREFIX + "texturepacks/" + path
                                    + "/version.txt");
            versiontxt << "2" << std::endl;
            versiontxt.close();

            std::string prefix = PLATFORM_FILE_PREFIX + "texturepacks/" + path;

            // Create some needed directories
            std::filesystem::create_directories(prefix + "/assets/minecraft/textures/armor");
            std::filesystem::create_directories(prefix + "/assets/minecraft/textures/gui");
            std::filesystem::create_directories(prefix + "/assets/minecraft/textures/item");
            std::filesystem::create_directories(prefix + "/assets/minecraft/textures/mob");
            std::filesystem::create_directories(prefix + "/assets/minecraft/sounds/dig");
            std::filesystem::create_directories(prefix + "/assets/minecraft/sounds/step");
            std::filesystem::create_directories(prefix + "/assets/crosscraft/textures/menu");

            // Moooove some stuuuuff
#if BUILD_PLAT == BUILD_PSP
            sceIoMove((prefix + "/assets/menu/logo.png").c_str(), (prefix +
                                    "/assets/crosscraft/textures/menu/logo.png").c_str());
            sceIoMove((prefix + "/assets/armor/chain.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/armor/chain.png").c_str());
            sceIoMove((prefix + "/assets/armor/plate.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/armor/plate.png").c_str());
            sceIoMove((prefix + "/assets/gui/gui.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/gui/gui.png").c_str());
            sceIoMove((prefix + "/assets/gui/icons.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/gui/icons.png").c_str());
            sceIoMove((prefix + "/assets/2char.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/2char.png").c_str());
            sceIoMove((prefix + "/assets/char.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/char.png").c_str());
            sceIoMove((prefix + "/assets/clouds.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/clouds.png").c_str());
            sceIoMove((prefix + "/assets/default.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/default.png").c_str());
            sceIoMove((prefix + "/assets/dirt.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/dirt.png").c_str());
            sceIoMove((prefix + "/assets/grass.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/grass.png").c_str());
            sceIoMove((prefix + "/assets/particles.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/particles.png").c_str());
            sceIoMove((prefix + "/assets/rain.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/rain.png").c_str());
            sceIoMove((prefix + "/assets/rock.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/rock.png").c_str());
            sceIoMove((prefix + "/assets/terrain.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/terrain.png").c_str());
            sceIoMove((prefix + "/assets/water.png").c_str(), (prefix +
                                    "/assets/minecraft/textures/water.png").c_str());
#else
            std::filesystem::rename(prefix + "/assets/menu/logo.png", prefix +
                                    "/assets/crosscraft/textures/menu/logo.png");

            std::filesystem::rename(prefix + "/assets/armor/chain.png", prefix +
                                    "/assets/minecraft/textures/armor/chain.png");
            std::filesystem::rename(prefix + "/assets/armor/plate.png", prefix +
                                    "/assets/minecraft/textures/armor/plate.png");

            std::filesystem::rename(prefix + "/assets/gui/gui.png", prefix +
                                    "/assets/minecraft/textures/gui/gui.png");
            std::filesystem::rename(prefix + "/assets/gui/icons.png", prefix +
                                    "/assets/minecraft/textures/gui/icons.png");

            std::filesystem::rename(prefix + "/assets/2char.png", prefix +
                                    "/assets/minecraft/textures/2char.png");
            std::filesystem::rename(prefix + "/assets/char.png", prefix +
                                    "/assets/minecraft/textures/char.png");
            std::filesystem::rename(prefix + "/assets/clouds.png", prefix +
                                    "/assets/minecraft/textures/clouds.png");
            std::filesystem::rename(prefix + "/assets/default.png", prefix +
                                    "/assets/minecraft/textures/default.png");
            std::filesystem::rename(prefix + "/assets/dirt.png", prefix +
                                    "/assets/minecraft/textures/dirt.png");
            std::filesystem::rename(prefix + "/assets/grass.png", prefix +
                                    "/assets/minecraft/textures/grass.png");
            std::filesystem::rename(prefix + "/assets/particles.png", prefix +
                                    "/assets/minecraft/textures/particles.png");
            std::filesystem::rename(prefix + "/assets/rain.png", prefix +
                                    "/assets/minecraft/textures/rain.png");
            std::filesystem::rename(prefix + "/assets/rock.png", prefix +
                                    "/assets/minecraft/textures/rock.png");
            std::filesystem::rename(prefix + "/assets/terrain.png", prefix +
                                    "/assets/minecraft/textures/terrain.png");
            std::filesystem::rename(prefix + "/assets/water.png", prefix +
                                    "/assets/minecraft/textures/water.png");
#endif

            // Remove old empty dirs
            std::filesystem::remove_all(prefix + "/assets/menu/");
            std::filesystem::remove_all(prefix + "/assets/armor/");
            std::filesystem::remove_all(prefix + "/assets/gui/");

            // Now start migrating everything over to the new directory
            // [..]copy over the default textures to fill in for missing stuff                                +
            std::filesystem::copy(PLATFORM_FILE_PREFIX + "resourcepacks/default",
                                  PLATFORM_FILE_PREFIX + "resourcepacks/" + path,
                                  std::filesystem::copy_options::recursive);
            // now put the old contents there
            std::filesystem::copy(prefix, PLATFORM_FILE_PREFIX + "resourcepacks/"
                                    + path, std::filesystem::copy_options::overwrite_existing
                                    | std::filesystem::copy_options::recursive);

            // Remove the old texture pack
            std::filesystem::remove_all(prefix);

        }

        // Trash the texturepacks folder!
        std::filesystem::remove_all(PLATFORM_FILE_PREFIX + "texturepacks/");
    } else {
        // Nothing to do.. for now..
        return;
    }
}

auto ResourcePackManager::scan_folder(std::string path) -> void {
    std::filesystem::path p(path);

    for (const auto &entry : std::filesystem::directory_iterator(p)) {

        if (entry.is_directory()) {
            // Add to list
            path_names.push_back(entry.path().filename().string());
        } else if (entry.is_regular_file()) {
            auto filename = entry.path().filename().string();

            if (filename.find(".zip") != std::string::npos) {
                extract_zip(path + filename);
                path_names.push_back(entry.path().filename().string().substr(
                    0, filename.find(".zip")));
            }
        }
    }

    std::sort(path_names.begin(), path_names.end());
    path_names.erase(std::unique(path_names.begin(), path_names.end()),
                     path_names.end());
}

auto ResourcePackManager::extract_zip(std::string path) -> int {
    unzFile zF = unzOpen(path.c_str());

    unz_global_info global_info;
    unzGetGlobalInfo(zF, &global_info);

    char read_buffer[8192];

    std::string dirname = path.substr(0, path.find(".zip")) + "/";
    std::filesystem::create_directory(dirname);

    for (int i = 0; i < global_info.number_entry; i++) {
        unz_file_info file_info;
        char filename[512];

        unzGetCurrentFileInfo(zF, &file_info, filename, 512, NULL, 0, NULL, 0);

        std::string prefix_name =
            path.substr(0, path.find(".zip")) + "/" + std::string(filename);

        const size_t filename_length = prefix_name.length();
        if (prefix_name[filename_length - 1] == '/') {
            std::filesystem::create_directory(prefix_name);

            SC_APP_INFO("CREATING: {}", prefix_name);
        } else {
            unzOpenCurrentFile(zF);
            FILE *out = fopen(prefix_name.c_str(), "wb");

            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(zF, read_buffer, 8192);
                if (error > 0) {
                    fwrite(read_buffer, error, 1, out);
                }
            } while (error > 0);

            fclose(out);
        }

        unzCloseCurrentFile(zF);

        // Go the the next entry listed in the zip file.
        if ((i + 1) < global_info.number_entry)
            unzGoToNextFile(zF);
    }

    unzClose(zF);

#if !PSP
    std::filesystem::remove(path);
#else
    sceIoRemove(path.c_str());
#endif
    return 0;
}
} // namespace CrossCraft
