#include "MusicManager.hpp"
#include "ResourcePackManager.hpp"

namespace CrossCraft
{
    MusicManager::MusicManager()
    {
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
    MusicManager::~MusicManager()
    {
    }

    auto MusicManager::update(float dt) -> void
    {
        if (timer >= 0) {
            timer -= dt;
            music->update();
        } else {
            music.reset();
            srand(time(0));
            int r = rand() % tracks.size();
            music = create_scopeptr<Audio::Clip>(ResourcePackManager::get().get_file(tracks[r]), true);
            timer = 420.0f + rand() % 360;
        }
    }

}