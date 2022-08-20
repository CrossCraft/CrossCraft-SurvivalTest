#include "MusicManager.hpp"
#include "ResourcePackManager.hpp"

namespace CrossCraft {
MusicManager::MusicManager() {
#if PSP
    std::string ext = "bgm";
#else
    std::string ext = "ogg";
#endif

    std::string prefix = "assets/minecraft/music/";
    tracks.push_back(prefix + "calm1." + ext);
    tracks.push_back(prefix + "calm2." + ext);
    tracks.push_back(prefix + "calm3." + ext);
    tracks.push_back(prefix + "hal1." + ext);
    tracks.push_back(prefix + "hal2." + ext);
    tracks.push_back(prefix + "hal3." + ext);
    tracks.push_back(prefix + "hal4." + ext);

    timer = -1.0f;
}
MusicManager::~MusicManager() {}

auto MusicManager::update(float dt) -> void {
    if (timer >= 0) {
        timer -= dt;
        music->update();
    } else {
        music.reset();
        srand(time(0));
        int r = rand() % tracks.size();
        auto file_path = ResourcePackManager::get().get_file(tracks[r]);
        music = create_scopeptr<Audio::Clip>(file_path, true);
        music->play();
        SC_APP_INFO("FILE {}", file_path);
        timer = 420.0f + rand() % 360;
    }
}

} // namespace CrossCraft