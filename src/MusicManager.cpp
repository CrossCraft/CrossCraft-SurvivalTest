#include "MusicManager.hpp"
#include "Option.hpp"
#include "ResourcePackManager.hpp"

namespace CrossCraft {
    MusicManager::MusicManager() {

        std::string prefix = "assets/minecraft/music/";
        tracks.push_back(prefix + "calm1.ogg");
        tracks.push_back(prefix + "calm2.ogg");
        tracks.push_back(prefix + "calm3.ogg");
        tracks.push_back(prefix + "hal1.ogg");
        tracks.push_back(prefix + "hal2.ogg");
        tracks.push_back(prefix + "hal3.ogg");
        tracks.push_back(prefix + "hal4.ogg");

        timer = -1.0f;
    }
    MusicManager::~MusicManager() {}

    auto MusicManager::update(float dt) -> void {
        if (!Option::get().music && music.get() != nullptr) {
            music->stop();
            return;
        }

        if (timer >= 0) {
            timer -= dt;
        } else {
            music.reset();
            srand(time(0));
            int r = rand() % tracks.size();
            auto file_path = ResourcePackManager::get().get_file(tracks[r]);
            SC_APP_INFO("FILE {}", file_path);
            music = create_scopeptr<Audio::Clip>(std::move(file_path), true);
            music->play();
            timer = 420.0f + rand() % 360;
        }
    }

}// namespace CrossCraft