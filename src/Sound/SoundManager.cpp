#include "SoundManager.hpp"
#include "../ResourcePackManager.hpp"
#include "../Utils.hpp"

namespace CrossCraft {

auto SoundManager::load_dig() -> void {
    ClipList l1;
    l1.clips.clear();

    l1.clips.push_back(
        create_refptr<Audio::Clip>(ResourcePackManager::get().get_file(
            "assets/minecraft/sounds/dig/glass1.wav")));
    l1.clips.push_back(
        create_refptr<Audio::Clip>(ResourcePackManager::get().get_file(
            "assets/minecraft/sounds/dig/glass2.wav")));
    l1.clips.push_back(
        create_refptr<Audio::Clip>(ResourcePackManager::get().get_file(
            "assets/minecraft/sounds/dig/glass3.wav")));

    dig.emplace("glass", l1);

    std::string prefix = "assets/minecraft/sounds/dig/";
    std::string name[4] = {"grass", "gravel", "stone", "wood"};

    for (int i = 0; i < 4; i++) {
        ClipList l;
        l.clips.clear();
        for (int c = 0; c < 4; c++)
            l.clips.push_back(
                create_refptr<Audio::Clip>(ResourcePackManager::get().get_file(
                    prefix + name[i] + std::to_string(c + 1) + ".wav")));

        dig.emplace(name[i], l);
    }
}
auto SoundManager::load_step() -> void {
    std::string prefix = "assets/minecraft/sounds/step/";
    std::string name[4] = {"grass", "gravel", "stone", "wood"};

    for (int i = 0; i < 4; i++) {
        ClipList l;
        l.clips.clear();
        for (int c = 0; c < 4; c++)
            l.clips.push_back(
                create_refptr<Audio::Clip>(ResourcePackManager::get().get_file(
                    prefix + name[i] + std::to_string(c + 1) + ".wav")));

        step.emplace(name[i], l);
    }
}

SoundManager::SoundManager() {
    load_dig();
    load_step();
}
SoundManager::~SoundManager() {}

std::string get_dig_mat(uint8_t block) {
    switch (block) {

    case Block::Sapling:
    case Block::Grass:
    case Block::Leaves:
    case Block::Sponge:
    case Block::Flower1:
    case Block::Flower2:
    case Block::Mushroom1:
    case Block::Mushroom2:
    case Block::TNT:
        return "grass";

    case Block::Glass:
        return "glass";

    case Block::Sand:
    case Block::Gravel:
    case Block::Dirt:
        return "gravel";

    case Block::Logs:
    case Block::Wood:
    case Block::Bookshelf:
        return "wood";

    default:
        return "stone";
    }
}

std::string get_step_mat(uint8_t block) {
    switch (block) {

    case Block::Sapling:
    case Block::Grass:
    case Block::Leaves:
    case Block::Sponge:
    case Block::Flower1:
    case Block::Flower2:
    case Block::Mushroom1:
    case Block::Mushroom2:
    case Block::TNT:
        return "grass";

    case Block::Sand:
    case Block::Gravel:
    case Block::Dirt:
        return "gravel";

    case Block::Logs:
    case Block::Wood:
    case Block::Bookshelf:
        return "wood";

    default:
        return "stone";
    }
}

auto SoundManager::play(uint8_t block, glm::vec3 pos, bool s) -> void {
    if (!s) {
        // Dig
        auto str = get_dig_mat(block);

        auto r = rand() % dig[str].clips.size();

        // dig[str].clips[r]->set_position(pos);
        dig[str].clips[r]->play();
    } else if (block != Block::Air) {
        // Step
        auto str = get_step_mat(block);

        auto r = rand() % step[str].clips.size();

        // dig[str].clips[r]->set_position(pos);
        step[str].clips[r]->play();
    }
}
} // namespace CrossCraft
