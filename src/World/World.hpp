/**
 * @file World.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief The World
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "../Chunk/ChunkStack.hpp"
#include "../Config.hpp"
#include "../Entity/Arrow.hpp"
#include "../Entity/Drops.hpp"
#include "../Entity/TNT.hpp"
#include "../Mob/MobManager.hpp"
#include "../Model/SteveModel.hpp"
#include "../Model/ZombieModel.hpp"
#include "../Player/Player.hpp"
#include "../Sound/SoundManager.hpp"
#include "Clouds.hpp"
#include "Particles/ParticleManager.hpp"
#include <Utilities/Types.hpp>
#include <glm.hpp>
#include <map>
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <vector>

namespace CrossCraft {

namespace Mob {
class MobManager;
}

namespace Chunk {
class Stack;
} // namespace Chunk

struct LayerMeta {
    bool is_empty;
    bool is_full;
};

struct ChunkMeta {
    bool is_empty;
    bool is_full;

    LayerMeta layers[16];
};

inline auto validate_ivec3(glm::ivec3 ivec, glm::vec3 size) -> bool {
    return ivec.x >= 0 && ivec.x < size.x && ivec.y >= 0 && ivec.y < size.y &&
           ivec.z >= 0 && ivec.z < size.z;
}

class Player;
class SelectionBox;
typedef uint8_t block_t;
class Drops;
class Arrow;
class TNT;

/**
 * @brief The world
 *
 */
class World {
  public:
    /**
     * @brief Construct a new World object
     *
     * @param p Player to use
     */
    World(RefPtr<Player> p);

    /**
     * @brief Destroy the World object
     *
     */
    ~World();

    /**
     * @brief Update (checks chunks, run chunk updates, tick updates);
     *
     * @param dt Delta Time
     */
    auto update(double dt) -> void;

    /**
     * @brief Draw the world
     *
     */
    auto draw() -> void;

    /**
     * @brief Get a World Index
     *
     * @param x Position
     * @param y Position
     * @param z Position
     * @return uint32_t block_t* worldData
     */
    inline uint32_t getIdx(int x, int y, int z) {
        if (x < 0 || x >= world_size.x || y >= world_size.y || y < 0 || z < 0 ||
            z >= world_size.z)
            return 0;
        return (y * world_size.z * world_size.x) + (z * world_size.x) + x;
    }

    /**
     * @brief Get a World Light Index
     *
     * @param x Position
     * @param y Position
     * @param z Position
     * @return uint32_t block_t* worldData
     */
    inline uint32_t getIdxl(int x, int y, int z) {
        if (x < 0 || x >= world_size.x || y >= world_size.y || y < 0 || z < 0 ||
            z >= world_size.z)
            return 0;
        return ((y / 16) * world_size.z * world_size.x) + (z * world_size.x) +
               x;
    }

    block_t *worldData;
    uint16_t *lightData;
    RefPtr<Player> player;
    ScopePtr<Clouds> clouds;
    Config cfg;

    /**
     * @brief Updates surrounding chunks
     *
     * @param x X of block modified
     * @param z Z of block modified
     */
    auto update_surroundings(int x, int z) -> void;

    /**
     * @brief Updates lighting
     *
     * @param x X of block mod
     * @param z Z of block mod
     */
    auto update_lighting(int x, int z) -> void;

    auto generate_meta() -> void;

    /**
     * @brief Updates nearby block states
     *
     * @param ivec Block source from update
     */
    auto update_nearby_blocks(glm::ivec3 ivec) -> void;

    /**
     * @brief Calculate and add update to chunk
     *
     */
    auto add_update(glm::ivec3 ivec) -> void;

    auto explode(glm::ivec3 pos) -> void;

    auto spawn() -> void;

    std::map<int, Chunk::Stack *> chunks;
    ChunkMeta *chunksMeta;

    auto set_block(short x, short y, short z, uint8_t block) -> void;

    glm::vec3 world_size;
    bool loaded;

    bool isBreaking;
    float timeLeftToBreak;
    float totalTimeBreak;
    glm::ivec3 breaking;
    float stored_dt;

    ScopePtr<SoundManager> sound_manager;
    ScopePtr<Arrow> arrow;

    ScopePtr<Mob::MobManager> mobManager;
    ScopePtr<Drops> drops;
    ScopePtr<TNT> tnt;

    static auto rain_toggle(std::any d) -> void;

  private:
    /**
     * @brief Get the needed chunks
     *
     * @return std::vector<glm::ivec2>
     */
    auto get_needed_chunks() -> std::vector<glm::ivec2>;

    glm::ivec2 pchunk_pos;

    ScopePtr<SelectionBox> sbox;

    unsigned int terrain_atlas;
    unsigned int particle_atlas;
    float *hmap;
    float tick_counter;

    float place_icd, break_icd;
    float chunk_generate_icd;

    std::map<float, glm::ivec2> to_generate;

    auto add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                          uint32_t lightVal, glm::vec3 mypos) -> void;

    friend class DigAction;
    friend class PlaceAction;
    friend class CrossCraftGenerator;
    friend class ClassicGenerator;
};

} // namespace CrossCraft