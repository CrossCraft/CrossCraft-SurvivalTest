#include "World.hpp"

namespace CrossCraft {
class DigAction {
  public:
    static auto dig(std::any d) -> void;
    static auto dig_release(std::any d) -> void;

  private:
    static auto doInventory(World *w) -> void;
};
} // namespace CrossCraft