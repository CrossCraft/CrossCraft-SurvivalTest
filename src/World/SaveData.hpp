#pragma once
#include "World.hpp"
#include <any>

namespace CrossCraft {
class World;

class SaveData {
  public:
    static auto load_world(World *wrld) -> bool;
    static auto save(std::any p) -> void;
};
} // namespace CrossCraft
