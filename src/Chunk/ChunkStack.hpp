/**
 * @file ChunkStack.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief Chunk Stack object
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "../World/World.hpp"
#include "ChunkMesh.hpp"
#include <array>
#include <glm.hpp>
#include <vector>
namespace CrossCraft {
class World;
namespace Chunk {

class ChunkMesh;

/**
 * @brief Chunk Stack
 *
 */
class Stack {
  public:
    /**
     * @brief Construct a new Chunk Stack object
     *
     * @param x X Position
     * @param z Y Position
     */
    Stack(int x, int z);

    /**
     * @brief Destroy the  Stack object
     *
     */
    ~Stack();

    /**
     * @brief Generate the stack
     *
     * @param wrld The world to reference
     */
    void generate(World *wrld);
    /**
     * @brief Generate the stack
     *
     * @param wrld The world to reference
     */
    void generate_border();

    /**
     * @brief Update
     *
     * @param wrld The world to reference
     */
    void update(World *wrld);

    /**
     * @brief Random Tick Update
     *
     * @param wrld The world to reference
     */
    void rtick_update(World *wrld);

    void post_update(World *wrld);

    /**
     * @brief Draw the  stack
     *
     */
    void draw(World *wrld);

    /**
     * @brief Draw the transparent
     *
     */
    void draw_transparent();

    /**
     * @brief Get the position
     *
     * @return glm::ivec2
     */
    inline auto get_pos() -> glm::ivec2 { return {cX, cZ}; }

    std::vector<glm::ivec3> posUpdate;
    bool border;

  private:
    bool check_visible(World *wrld, glm::vec3 posCheck, int cY);
    auto update_check(World *wrld, int blkr, glm::ivec3 chk) -> void;
    std::vector<glm::ivec3> updated;

    bool wasRendered[4];

    std::array<ChunkMesh *, 4> stack;
    int cX, cZ;
};

} // namespace Chunk
} // namespace CrossCraft