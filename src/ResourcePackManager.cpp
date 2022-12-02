#include "ResourcePackManager.hpp"
#include <contrib/minizip/unzip.h>
#include <Rendering/Rendering.hpp>
#include <algorithm>
#include <fstream>
#include "Utils.hpp"

namespace CrossCraft {
    using namespace Stardust_Celeste;

    ResourcePackManager::ResourcePackManager() {
        pack_names.clear();
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

        for (auto &l: layers) {
            file << l << "\n";
        }

        file.close();
    }

    ResourcePackManager::~ResourcePackManager() {}

    auto ResourcePackManager::get_path() -> std::string {
        for (int i = layers.size() - 1; i >= 0; i--) {
            auto &l = layers[i];

            std::string path = PLATFORM_FILE_PREFIX + "resourcepacks/" + l + "/";

            return path;
        }

        return "";
    }

    auto ResourcePackManager::get_file(std::string filename) -> std::string {

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

    auto ResourcePackManager::load_texture(std::string filename, u32 magFilter, u32 minFilter, bool repeat, bool flip, bool vram) -> u32 {

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

#define renameCheck(x, y)           \
    if (std::filesystem::exists(x)) \
        std::filesystem::rename(x, y);

    auto ResourcePackManager::convert_old_resourcepacks() -> void {
        std::vector<std::string> old_paths;

        // See if a 'texturepacks' folder exists (version 1)
        std::filesystem::path p("texturepacks/");

        // It does, version 1 conversion.
        if (std::filesystem::exists(p)) {
            // Copy of scan_folder to look for old texturepacks
            for (const auto &entry: std::filesystem::directory_iterator(p)) {
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
            for (auto path: old_paths) {
                // Give them some metadata text
                std::ofstream nametxt(PLATFORM_FILE_PREFIX + "texturepacks/" +
                                      path + "/name.txt");
                nametxt << path << std::endl;
                nametxt.close();

                std::ofstream packtxt(PLATFORM_FILE_PREFIX + "texturepacks/" +
                                      path + "/pack.txt");
                packtxt << "Converted from an older version." << std::endl;
                packtxt.close();

                std::ofstream versiontxt(PLATFORM_FILE_PREFIX + "texturepacks/" +
                                         path + "/version.txt");
                versiontxt << "2" << std::endl;
                versiontxt.close();

                std::string prefix = PLATFORM_FILE_PREFIX + "texturepacks/" + path;

                // Create some needed directories
                std::filesystem::create_directories(
                        prefix + "/assets/minecraft/textures/armor");
                std::filesystem::create_directories(
                        prefix + "/assets/minecraft/textures/gui");
                std::filesystem::create_directories(
                        prefix + "/assets/minecraft/textures/item");
                std::filesystem::create_directories(
                        prefix + "/assets/minecraft/textures/mob");
                std::filesystem::create_directories(prefix +
                                                    "/assets/minecraft/sounds/dig");
                std::filesystem::create_directories(
                        prefix + "/assets/minecraft/sounds/step");
                std::filesystem::create_directories(
                        prefix + "/assets/crosscraft/textures/menu");
                std::filesystem::create_directories(PLATFORM_FILE_PREFIX +
                                                    "resourcepacks/");

                // Moooove some stuuuuff
#if BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_PSP
                sceIoCopy(prefix + "/assets/menu/logo.png",
                          prefix + "/assets/crosscraft/textures/menu/logo.png",
                          true);

                sceIoCopy(prefix + "/assets/armor/chain.png",
                          prefix + "/assets/minecraft/textures/armor/chain.png",
                          true);
                sceIoCopy(prefix + "/assets/armor/plate.png",
                          prefix + "/assets/minecraft/textures/armor/plate.png",
                          true);

                sceIoCopy(prefix + "/assets/gui/gui.png",
                          prefix + "/assets/minecraft/textures/gui/gui.png", true);
                sceIoCopy(prefix + "/assets/gui/icons.png",
                          prefix + "/assets/minecraft/textures/gui/icons.png",
                          true);

                sceIoCopy(prefix + "/assets/2char.png",
                          prefix + "/assets/minecraft/textures/2char.png", true);
                sceIoCopy(prefix + "/assets/char.png",
                          prefix + "/assets/minecraft/textures/char.png", true);
                sceIoCopy(prefix + "/assets/clouds.png",
                          prefix + "/assets/minecraft/textures/clouds.png", true);
                sceIoCopy(prefix + "/assets/default.png",
                          prefix + "/assets/minecraft/textures/default.png", true);
                sceIoCopy(prefix + "/assets/dirt.png",
                          prefix + "/assets/minecraft/textures/dirt.png", true);
                sceIoCopy(prefix + "/assets/grass.png",
                          prefix + "/assets/minecraft/textures/grass.png", true);
                sceIoCopy(prefix + "/assets/particles.png",
                          prefix + "/assets/minecraft/textures/particles.png",
                          true);
                sceIoCopy(prefix + "/assets/rain.png",
                          prefix + "/assets/minecraft/textures/rain.png", true);
                sceIoCopy(prefix + "/assets/rock.png",
                          prefix + "/assets/minecraft/textures/rock.png", true);
                sceIoCopy(prefix + "/assets/terrain.png",
                          prefix + "/assets/minecraft/textures/terrain.png", true);
                sceIoCopy(prefix + "/assets/water.png",
                          prefix + "/assets/minecraft/textures/water.png", true);

                // Remove old empty dirs
                sceIoRemove((prefix + "/assets/menu/").c_str());
                sceIoRemove((prefix + "/assets/armor/").c_str());
                sceIoRemove((prefix + "/assets/gui/").c_str());

                // now put the old contents there
                std::filesystem::copy(
                        prefix, PLATFORM_FILE_PREFIX + "resourcepacks/" + path,
                        std::filesystem::copy_options::overwrite_existing |
                                std::filesystem::copy_options::recursive);

                // Remove the old texture pack
                sceIoRemove(prefix.c_str());

#else

                renameCheck(prefix + "/assets/menu/logo.png",
                            prefix + "/assets/crosscraft/textures/menu/logo.png");

                renameCheck(prefix + "/assets/armor/chain.png",
                            prefix + "/assets/minecraft/textures/armor/chain.png");
                renameCheck(prefix + "/assets/armor/plate.png",
                            prefix + "/assets/minecraft/textures/armor/plate.png");

                renameCheck(prefix + "/assets/gui/gui.png",
                            prefix + "/assets/minecraft/textures/gui/gui.png");
                renameCheck(prefix + "/assets/gui/icons.png",
                            prefix + "/assets/minecraft/textures/gui/icons.png");

                renameCheck(prefix + "/assets/2char.png",
                            prefix + "/assets/minecraft/textures/2char.png");
                renameCheck(prefix + "/assets/char.png",
                            prefix + "/assets/minecraft/textures/char.png");
                renameCheck(prefix + "/assets/clouds.png",
                            prefix + "/assets/minecraft/textures/clouds.png");
                renameCheck(prefix + "/assets/default.png",
                            prefix + "/assets/minecraft/textures/default.png");
                renameCheck(prefix + "/assets/dirt.png",
                            prefix + "/assets/minecraft/textures/dirt.png");
                renameCheck(prefix + "/assets/grass.png",
                            prefix + "/assets/minecraft/textures/grass.png");
                renameCheck(prefix + "/assets/particles.png",
                            prefix + "/assets/minecraft/textures/particles.png");
                renameCheck(prefix + "/assets/rain.png",
                            prefix + "/assets/minecraft/textures/rain.png");
                renameCheck(prefix + "/assets/rock.png",
                            prefix + "/assets/minecraft/textures/rock.png");
                renameCheck(prefix + "/assets/terrain.png",
                            prefix + "/assets/minecraft/textures/terrain.png");
                renameCheck(prefix + "/assets/water.png",
                            prefix + "/assets/minecraft/textures/water.png");

                // Remove old empty dirs
                std::filesystem::remove_all(prefix + "/assets/menu/");
                std::filesystem::remove_all(prefix + "/assets/armor/");
                std::filesystem::remove_all(prefix + "/assets/gui/");

                // now put the old contents there
                std::filesystem::copy(
                        prefix, PLATFORM_FILE_PREFIX + "resourcepacks/" + path,
                        std::filesystem::copy_options::overwrite_existing |
                                std::filesystem::copy_options::recursive);

                // Remove the old texture pack
                std::filesystem::remove_all(prefix);
#endif
            }

// Trash the texturepacks folder!
#if BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_VITA
            sceIoRemove((PLATFORM_FILE_PREFIX + "texturepacks/").c_str());
#else
            std::filesystem::remove_all(PLATFORM_FILE_PREFIX + "texturepacks/");
#endif
        } else {
            // Nothing to do.. for now..
            return;
        }
    }

    auto ResourcePackManager::scan_folder(std::string path) -> void {
        std::filesystem::path p(path);

        for (const auto &entry: std::filesystem::directory_iterator(p)) {

            if (entry.is_directory()) {
                // Add name.txt entry to list
                std::ifstream nametxt(
                        path + "/" + entry.path().filename().string() + "/name.txt");

                // add a fallback to avoid crashes
                if (nametxt.is_open()) {
                    std::stringstream pck;
                    pck << nametxt.rdbuf();
                    std::string packname = pck.str();
                    // remove newline stuff
                    packname.erase(
                            std::remove(packname.begin(), packname.end(), '\r'),
                            packname.end());
                    packname.erase(
                            std::remove(packname.begin(), packname.end(), '\n'),
                            packname.end());
                    pack_names.push_back(packname);
                    nametxt.close();
                } else {
                    pack_names.push_back(entry.path().filename().string());
                }
            } else if (entry.is_regular_file()) {
                auto filename = entry.path().filename().string();

                if (filename.find(".zip") != std::string::npos) {
                    extract_zip(path + filename);
                    pack_names.push_back(entry.path().filename().string().substr(
                            0, filename.find(".zip")));
                }
            }
        }

        std::sort(pack_names.begin(), pack_names.end());
        pack_names.erase(std::unique(pack_names.begin(), pack_names.end()),
                         pack_names.end());
    }

    auto ResourcePackManager::extract_zip(std::string path) -> int {
        unzFile zF = unzOpen(path.c_str());

        unz_global_info global_info;
        unzGetGlobalInfo(zF, &global_info);

        char read_buffer[8192];

        std::string dirname = path.substr(0, path.find(".zip")) + "/";
        std::filesystem::create_directory(dirname);

        for (size_t i = 0; i < global_info.number_entry; i++) {
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
            if (static_cast<size_t>(i + 1) < global_info.number_entry)
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
}// namespace CrossCraft
