#pragma once
#include <Audio/Clip.hpp>
#include <map>
#include <vector>
#include "../BlockConst.hpp"

namespace CrossCraft {
    using namespace Stardust_Celeste;

    struct ClipList {
        std::vector<RefPtr<Audio::Clip>> clips;
    };

    class SoundManager {
    public:
        SoundManager();
        ~SoundManager();

        auto play(uint8_t block, glm::vec3 pos, bool step = false) -> void;

    private:
        std::map<std::string, ClipList> dig;
        std::map<std::string, ClipList> step;

        auto load_dig() -> void;
        auto load_step() -> void;
    };

}// namespace CrossCraft