#include "DigAction.hpp"
#include "../Entity/DropLookup.hpp"
#include "SaveData.hpp"
#include <Utilities/Input.hpp>
#include <gtx/rotate_vector.hpp>

namespace CrossCraft {

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

auto DigAction::dig_release(std::any d) -> void {
    auto w = std::any_cast<World *>(d);
    w->isBreaking = false;
}

int last = 0;

auto get_break_time(uint8_t blk) -> float {
    switch (blk) {
    case Block::Flower1:
    case Block::Flower2:
    case Block::Mushroom1:
    case Block::Mushroom2:
    case Block::TNT:
    case Block::Sapling:
        return 0.0f;

    case Block::Grass:
    case Block::Gravel:
    case Block::Sand:
    case Block::Glass:
    case Block::Leaves:
        return 0.4f;

    case Block::Iron_Ore:
    case Block::Iron:
    case Block::Gold_Ore:
    case Block::Gold:
    case Block::Obsidian:
    case Block::Logs:
    case Block::Coal_Ore:
    case Block::Wood:
    case Block::Bookshelf:
        return 3.0f;

    default:
        return 1.0f;
    }
}

auto DigAction::dig(std::any d) -> void {
    auto w = std::any_cast<World *>(d);

#if BUILD_PC
    if (w->player->in_pause) {

        if (w->player->pauseMenu->pauseState == 0) {
            if (w->player->pauseMenu->selIdx == 0) {
                w->player->in_pause = false;
                w->player->pauseMenu->exit();
            } else if (w->player->pauseMenu->selIdx == 1) {
                w->player->pauseMenu->pauseState = 1;
            } else if (w->player->pauseMenu->selIdx == 2) {
                SaveData::save(w);
            } else if (w->player->pauseMenu->selIdx == 3) {
                exit(0);
            }
        }
        return;
    }
#endif

    if (!w->player->isAlive) {
        w->player->respawn(w->player->spawnPoint);
        return;
    }

    w->player->blockRep->trigger_swing();

    // Create a default vector of the player facing
    auto default_vec = glm::vec3(0, 0, 1);
    default_vec = glm::rotateX(default_vec, DEGTORAD(w->player->get_rot().x));
    default_vec =
        glm::rotateY(default_vec, DEGTORAD(-w->player->get_rot().y + 180));

    // Set our reach and default vector to that reach distance
    const float REACH_DISTANCE = 4.0f;
    default_vec *= REACH_DISTANCE;

    // Cast steps towards said direction
    const u32 NUM_STEPS = 100;
    for (u32 i = 0; i < NUM_STEPS; i++) {
        float percentage =
            static_cast<float>(i) / static_cast<float>(NUM_STEPS);

        auto cast_pos = w->player->get_pos() + (default_vec * percentage);

        auto ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                               static_cast<s32>(cast_pos.y),
                               static_cast<s32>(cast_pos.z));

        // Check valid vector
        if (!validate_ivec3(ivec, w->world_size))
            continue;

        // Check Player hit MOB
        for (auto &m : w->mobManager->mobs) {
            auto diff = m->pos - cast_pos;

            auto lenS = sqrtf(diff.x * diff.x + diff.z * diff.z);
            auto lenF =
                sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

            if (lenS < 0.6f) {
                if (lenF < 1.5f) {
                    m->OnHit(w, 2, default_vec, true);
                    return;
                }
            }
        }
        // Check TNT
        for (auto &m : w->tnt->tnt_list) {
            auto diff = m.pos - cast_pos;

            auto lenS = sqrtf(diff.x * diff.x + diff.z * diff.z);
            auto lenF =
                sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

            if (lenS < 0.6f) {
                if (lenF < 1.5f) {
                    m.OnHit(w, 2, default_vec, true);
                    return;
                }
            }
        }

        // Get Block
        u32 idx = w->getIdx(ivec.x, ivec.y, ivec.z);
        auto blk = w->worldData[idx];

        // Hit through these blocks
        if (blk == Block::Air || blk == Block::Bedrock || blk == Block::Water ||
            blk == Block::Lava)
            continue;

        if (!w->isBreaking) {
            w->isBreaking = true;
            w->totalTimeBreak = get_break_time(blk);
            w->timeLeftToBreak = w->totalTimeBreak;
            w->breaking = ivec;
        } else {
            if (ivec != w->breaking) {
                w->isBreaking = true;
                w->totalTimeBreak = get_break_time(blk);
                w->timeLeftToBreak = w->totalTimeBreak;
                w->breaking = ivec;
            } else {
                // Ivec is breaking
                if (w->timeLeftToBreak < 0) {
                    // We found a working block -- create break particles
                    w->psystem->initialize(blk, cast_pos);

                    if (blk != Block::TNT) {

                        DropData d;
                        d.inRange = false;
                        d.animTime = 0.0f;
                        d.pos = cast_pos;
                        d.size = {0.25f, 0.25f, 0.25f};
                        d.vel = {0.0f, 2.0f, 0.0f};
                        lookup(blk, d);

                        if (d.quantity > 0)
                            w->drops->add_drop(d);

                    } else {
                        TNTData data;
                        data.inRange = false;
                        data.pos = ivec;
                        data.rot = {0, 0};
                        data.size = {0.1f, 0.1f, 0.1f};
                        data.vel = {0.1f, 5.0f, 0.1f};
                        data.fuse = 2.0f;
                        data.immune = 0.25f;
                        data.Killed = false;

                        w->tnt->add_TNT(data);
                    }
                    uint16_t x = ivec.x / 16;
                    uint16_t y = ivec.z / 16;
                    uint32_t id = x << 16 | (y & 0x00FF);

                    // Check if it's a sponge
                    bool was_sponge = false;
                    if (w->worldData[idx] == Block::Sponge) {
                        was_sponge = true;
                    }

                    w->sound_manager->play(blk, cast_pos);

                    // Set to air
                    w->worldData[idx] = Block::Air;

                    // Update metadata
                    int mIdx = ivec.y / 16 * w->world_size.z / 16 *
                                   w->world_size.x / 16 +
                               ivec.z / 16 * w->world_size.x / 16 + ivec.x / 16;

                    w->chunksMeta[mIdx].is_full = false;

                    // Update surrounding blocks on a larger radius for water
                    // filling
                    if (was_sponge) {
                        for (auto i = ivec.x - 3; i <= ivec.x + 3; i++) {
                            for (auto j = ivec.z - 3; j <= ivec.z + 3; j++) {
                                w->add_update({i, ivec.y, j});
                                w->add_update({i, ivec.y + 1, j});
                                w->add_update({i, ivec.y - 1, j});
                                w->add_update({i, ivec.y + 2, j});
                                w->add_update({i, ivec.y - 2, j});
                            }
                        }
                    }

                    // Update Lighting
                    w->update_lighting(ivec.x, ivec.z);

                    if (w->chunks.find(id) != w->chunks.end())
                        w->chunks[id]->generate(w);

                    w->update_surroundings(ivec.x, ivec.z);
                    w->update_nearby_blocks(ivec);

                    w->isBreaking = false;
                } else {
                    w->timeLeftToBreak -= w->stored_dt;

                    // int newL = w->timeLeftToBreak / 0.33f;
                    //
                    // if (newL != last) {
                    //     w->sound_manager->play(blk, cast_pos);
                    //     last = newL;
                    // }
                }
            }
        }
        break;
    }
}

} // namespace CrossCraft