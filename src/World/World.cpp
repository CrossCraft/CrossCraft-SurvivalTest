#include "World.hpp"
#include <Platform/Platform.hpp>
#include <Rendering/Rendering.hpp>
#include <Utilities/Input.hpp>
#include <iostream>
#include "../Entity/DropLookup.hpp"
#include "../Modding/Mod.hpp"
#include "../ResourcePackManager.hpp"
#include "Generation/NoiseUtil.hpp"
#include "Generation/WorldGenUtil.hpp"
#include "SelectionBox.hpp"

#if PSP
#include <pspctrl.h>
#endif

namespace CrossCraft {

    World::World(std::shared_ptr<Player> p) {
        tick_counter = 0;
        player = p;
        pchunk_pos = {-1, -1};
        world_size = {256, 64, 256};
        hmap = nullptr;
        loaded = false;
        terrain_atlas = ResourcePackManager::get().load_texture(
                "assets/minecraft/textures/terrain.png", SC_TEX_FILTER_NEAREST,
                SC_TEX_FILTER_NEAREST, true, false, true);
        particle_atlas = ResourcePackManager::get().load_texture(
                "assets/minecraft/textures/particles.png", SC_TEX_FILTER_NEAREST,
                SC_TEX_FILTER_NEAREST, true, false, true);
        p->terrain_atlas = terrain_atlas;
        p->wrldRef = this;

        NoiseUtil::initialize();

        clouds = create_scopeptr<Clouds>();

        ParticleManager::get().terrain = terrain_atlas;
        ParticleManager::get().death = particle_atlas;
        ParticleManager::get().weather = ResourcePackManager::get().load_texture(
                "assets/minecraft/textures/rain.png", SC_TEX_FILTER_NEAREST,
                SC_TEX_FILTER_NEAREST, true, false, true);

        // Zero the array
        worldData = reinterpret_cast<block_t *>(
                calloc((uint64_t) world_size.x * (uint64_t) world_size.y *
                               (uint64_t) world_size.z,
                       sizeof(block_t)));
        lightData = reinterpret_cast<uint16_t *>(
                calloc((uint64_t) world_size.x * ((uint64_t) world_size.y / 16) *
                               (uint64_t) world_size.z,
                       sizeof(uint16_t)));
        chunksMeta = reinterpret_cast<ChunkMeta *>(
                calloc((uint64_t) world_size.x / 16 * (uint64_t) world_size.y / 16 *
                               (uint64_t) world_size.z / 16,
                       sizeof(ChunkMeta)));

        chunks.clear();

        sbox = create_scopeptr<SelectionBox>();
        drops = create_scopeptr<Drops>();
        arrow = create_scopeptr<Arrow>();
        tnt = create_scopeptr<TNT>();
        tnt->terrain_atlas = terrain_atlas;
        drops->terrain_atlas = terrain_atlas;

        sound_manager = create_scopeptr<SoundManager>();

        place_icd = 0.0f;
        break_icd = 0.0f;
        chunk_generate_icd = 0.0f;

        isBreaking = false;
        timeLeftToBreak = -1.0f;
        breaking = {-1, -1, -1};
        mobManager = create_scopeptr<Mob::MobManager>();
    }

    auto World::spawn() -> void {
        if (!loaded)
            player->spawn(this);
    }
    template<typename T>
    constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

    auto World::generate_meta() -> void {
        for (int x = 0; x < world_size.x / 16; x++)
            for (int y = 0; y < world_size.y / 16; y++)
                for (int z = 0; z < world_size.z / 16; z++) {
                    int idx = y * world_size.x / 16 * world_size.z / 16 +
                              z * world_size.x / 16 + x;

                    auto &meta = chunksMeta[idx];
                    meta.is_empty = true;
                    meta.is_full = true;

                    for (int layer = 0; layer < 16; layer++) {
                        meta.layers[layer].is_empty = true;
                        meta.layers[layer].is_empty = false;
                    }

                    for (int local_x = 0; local_x < 16; local_x++)
                        for (int local_z = 0; local_z < 16; local_z++)
                            for (int local_y = 0; local_y < 16; local_y++) {
                                auto cx = x * 16 + local_x;
                                auto cy = y * 16 + local_y;
                                auto cz = z * 16 + local_z;

                                auto idx = getIdx(cx, cy, cz);

                                auto blk = worldData[idx];

                                if (blk != Block::Air) {
                                    meta.is_empty = false;
                                    meta.layers[local_y].is_empty = false;
                                }
                                if (blk == Block::Air || blk == Block::Water ||
                                    blk == Block::Sapling || blk == Block::Leaves ||
                                    blk == Block::Glass || blk == Block::Slab ||
                                    blk == Block::Flower1 ||
                                    blk == Block::Flower2 ||
                                    blk == Block::Mushroom1 ||
                                    blk == Block::Mushroom2) {
                                    meta.is_full = false;
                                    meta.layers[local_y].is_full = false;
                                }
                            }
                }
    }

    World::~World() {
        Rendering::TextureManager::get().delete_texture(terrain_atlas);
        free(worldData);
        free(lightData);
        free(chunksMeta);
        // Destroy height map
        if (hmap != nullptr)
            free(hmap);

        for (auto const &[key, val]: chunks) {
            if (val)
                delete val;
        }
        chunks.clear();
    }

#if PSP || BUILD_PLAT == BUILD_3DS
    const auto CHUNKS_PER_SECOND = 2.0f;
#elif BUILD_PLAT == BUILD_VITA
    const auto CHUNKS_PER_SECOND = 4.0f;
#else
    const auto CHUNKS_PER_SECOND = 96.0f;
#endif

#if BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_VITA
    const auto RENDER_DISTANCE_DIAMETER = 10.0f;
#elif BUILD_PLAT == BUILD_3DS
    const auto RENDER_DISTANCE_DIAMETER = 5.0f;
#else
    const auto RENDER_DISTANCE_DIAMETER = 16.f;
#endif

    auto World::get_needed_chunks() -> std::vector<glm::ivec2> {
#if BUILD_PLAT == BUILD_3DS
        auto RDIST_DIAMETER = static_cast<float>(RENDER_DISTANCE_DIAMETER);
#else
        auto RDIST_DIAMETER = RENDER_DISTANCE_DIAMETER *
                              static_cast<float>(Option::get().renderDist + 1) /
                              4.0f;
#endif

        auto rad = RDIST_DIAMETER / 2.f;

        std::vector<glm::ivec2> needed_chunks;
        for (auto x = (-rad - 1); x < (rad + 1); x++) {
            for (auto y = (-rad - 1); y < (rad + 1); y++) {
                auto bx = x + pchunk_pos.x;
                auto by = y + pchunk_pos.y;

                auto dx = (bx - pchunk_pos.x);
                dx *= dx;

                auto dy = (by - pchunk_pos.y);
                dy *= dy;

                auto d = dx + dy;

                // If distance <= radius
                if (d <= rad * rad) {
                    needed_chunks.push_back({bx, by});
                }
            }
        }
        return needed_chunks;
    }
    int numTicks = 0;
    int old_rdist = 0;
    void World::update(double dt) {
        // Request 3D Mode
        Rendering::RenderContext::get().set_mode_3D();
        player->update(static_cast<float>(dt), this);
        sbox->update_position(this);

        if (!player->in_pause) {
            clouds->update(dt);
            ParticleManager::get().update(player.get()->pos, dt);
            drops->update(dt, player.get(), this);
            arrow->update(dt, player.get(), this);
            mobManager->update(dt, player.get(), this);
            tnt->update(dt, player.get(), this);
        }

        tick_counter += dt;
        stored_dt = dt;

        break_icd -= dt;
        place_icd -= dt;
        if (tick_counter > 0.5f) {
            tick_counter = 0;
            numTicks++;

            if (numTicks % 2) {
                // wpsystem->initialize(0, player->pos);
            }
            for (auto &[key, value]: chunks) {
                // Random tick
                for (int i = 0; i < 30; i++)
                    value->rtick_update(this);

                // Chunk Updates
                value->update(this);

                value->post_update(this);
            }

            Modding::ModManager::get().onTick();
        }

        // wpsystem->update(dt);
        auto ppos = player->get_pos();
        glm::ivec2 new_pos = {static_cast<int>(ppos.x) / 16,
                              static_cast<int>(ppos.z) / 16};

        if (pchunk_pos != new_pos || Option::get().renderDist != old_rdist) {
            old_rdist = Option::get().renderDist;
            pchunk_pos = new_pos;

            // Get what's needed
            auto needed = get_needed_chunks();

            // Check what we have - what we still need

            std::map<int, Chunk::Stack *> existing_chunks;

            for (auto &ipos: needed) {
                uint16_t x = ipos.x;
                uint16_t y = ipos.y;
                uint32_t id = x << 16 | (y & 0x00FF);

                if (chunks.find(id) != chunks.end()) {
                    // move to existing
                    existing_chunks.emplace(id, chunks[id]);
                    chunks.erase(id);
                } else {
                    // needs generated

                    glm::vec2 appos = {ppos.x / 16.0f, ppos.z / 16.0f};
                    glm::vec2 aipos = {(float) ipos.x, (float) ipos.y};

                    glm::vec2 diff = appos - aipos;
                    float len = diff.x * diff.x + diff.y * diff.y;
                    to_generate.emplace(len, ipos);
                }
            }

            // Remaining can be released
            for (auto &[key, value]: chunks) {
                delete value;
            }
            chunks.clear();

            // Now merge existing into blank map
            chunks.merge(existing_chunks);
        }

        chunk_generate_icd -= dt;

        // Generate remaining
        if (chunk_generate_icd <= 0.0f) {
            chunk_generate_icd = 1.0f / (float) CHUNKS_PER_SECOND;

            if (to_generate.size() > 0) {
                Utilities::Timer t2;
                auto cs = to_generate.size();
                SC_APP_INFO("GENERATING {} CHUNKS!", cs);
                t2.reset();
                while (to_generate.size() > 0) {
                    auto ipos = to_generate.begin()->second;

                    if (ipos.x >= 0 && ipos.x < (world_size.x / 16) && ipos.y >= 0 &&
                        ipos.y < (world_size.z / 16)) {
                        Chunk::Stack *stack = new Chunk::Stack(ipos.x, ipos.y);
                        stack->generate(this);

                        uint16_t x = ipos.x;
                        uint16_t y = ipos.y;
                        uint32_t id = x << 16 | (y & 0x00FF);
                        chunks.emplace(id, stack);
                    } else if (cfg.compat) {
                        Chunk::Stack *stack = new Chunk::Stack(ipos.x, ipos.y);
                        stack->generate_border();

                        uint16_t x = ipos.x;
                        uint16_t y = ipos.y;
                        uint32_t id = x << 16 | (y & 0x00FF);
                        chunks.emplace(id, stack);
                    }

                    to_generate.erase(to_generate.begin()->first);
                }

                auto dt2 = t2.get_delta_time() * 1000;
                SC_APP_INFO("TIME TAKEN {}", dt2);
            }
        }
    }

    void World::draw() {

        // Set up texture
        Rendering::TextureManager::get().bind_texture(terrain_atlas);

#if BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_3DS
        GI::disable(GI_BLEND);
        GI::disable(GI_ALPHA_TEST);
        GI::enable(GI_FOG);

#if BUILD_PLAT == BUILD_PSP
        if (!cfg.oldSky) {
            sceGuFog(0.2f * 3.0f * 16.0f, 0.8f * 4.5f * 16.0f, 0x00FFFFFF);
        } else {
            sceGuFog(0.2f * 3.0f * 16.0f, 0.8f * 4.5f * 16.0f, 0x00FFCC99);
        }
#endif
#endif
        GI::enable(GI_DEPTH_TEST);
        GI::set_culling_mode(true, true);

        // Draw opaque
        for (auto const &[key, val]: chunks) {
            if (val != nullptr)
                val->draw(this);
        }

        GI::enable(GI_BLEND);
#if PSP
        GI::enable(GI_ALPHA_TEST);
#endif
        drops->draw();
        arrow->draw();

        // Set up texture
        Rendering::TextureManager::get().bind_texture(terrain_atlas);

        // Draw transparent
        for (auto const &[key, val]: chunks) {
            if (val != nullptr)
                val->draw_transparent();
        }

#if !(BUILD_PLAT == BUILD_3DS)
        clouds->draw();
#endif
        tnt->draw();

        ParticleManager::get().draw(glm::vec3(player->rot.x, player->rot.y, 0.0f));

        mobManager->draw();
        sbox->draw();

        Rendering::TextureManager::get().bind_texture(terrain_atlas);
        sbox->draw_break(this);
        player->draw(this);
    }

    auto World::rain_toggle(std::any d) -> void {
        UNUSED(d);
        ParticleManager::get().raining = !ParticleManager::get().raining;
    }

    auto World::update_surroundings(int x, int z) -> void {
        auto localX = x % 16;
        auto localZ = z % 16;

        uint16_t cX = x / 16;
        uint16_t cY = z / 16;

        bool xMod = true;
        auto nX = cX;
        if (localX == 0) {
            nX -= 1;
        } else if (localX == 15) {
            nX += 1;
        } else {
            xMod = false;
        }

        if (xMod && nX < 16) {
            uint32_t idxx = nX << 16 | (cY & 0x00FF);
            if (chunks.find(idxx) != chunks.end())
                chunks[idxx]->generate(this);
        }

        bool zMod = true;
        auto nY = cY;
        if (localZ == 0) {
            nY -= 1;
        } else if (localZ == 15) {
            nY += 1;
        } else {
            zMod = false;
        }

        if (zMod && nY < 16) {
            uint32_t idzz = 0 | cX << 16 | (nY & 0x00FF);
            if (chunks.find(idzz) != chunks.end())
                chunks[idzz]->generate(this);
        }
    }

    auto World::update_lighting(int x, int z) -> void {
        // Clear
        for (int i = 0; i < 4; i++) {
            int idx2 = getIdxl(x, i * 16, z);
            lightData[idx2] = 0;
        }

        // Retrace
        for (int y = 63; y >= 0; y--) {
            auto idx = getIdx(x, y, z);
            auto blk = worldData[idx];
            if (blk == Block::Air || blk == Block::Flower1 ||
                blk == Block::Flower2 || blk == Block::Mushroom1 ||
                blk == Block::Mushroom2 || blk == Block::Sapling ||
                blk == Block::Glass || blk == Block::Leaves) {
                auto idx2 = getIdxl(x, y, z);
                lightData[idx2] |= 1 << (y % 16);
                continue;
            }

            auto idx2 = getIdxl(x, y, z);
            lightData[idx2] |= 1 << (y % 16);
            break;
        }
    }

    auto World::set_block(short x, short y, short z, uint8_t block) -> void {
        worldData[getIdx(x, y, z)] = block;
    }

    auto World::add_update(glm::ivec3 ivec) -> void {
        if (!validate_ivec3(ivec, world_size))
            return;

        uint16_t x = ivec.x / 16;
        uint16_t y = ivec.z / 16;
        uint32_t id = x << 16 | (y & 0x00FF);

        if (chunks.find(id) != chunks.end()) {
            chunks[id]->posUpdate.push_back(ivec);
        }
    }

    auto World::update_nearby_blocks(glm::ivec3 ivec) -> void {
        add_update({ivec.x, ivec.y, ivec.z});
        add_update({ivec.x, ivec.y + 1, ivec.z});
        add_update({ivec.x, ivec.y - 1, ivec.z});
        add_update({ivec.x - 1, ivec.y, ivec.z});
        add_update({ivec.x + 1, ivec.y, ivec.z});
        add_update({ivec.x, ivec.y, ivec.z + 1});
        add_update({ivec.x, ivec.y, ivec.z - 1});
    }

    auto World::explode(glm::ivec3 pos) -> void {
        const float EXPLOSION_RADIUS = 3.0f;

        std::map<int, int> surroundUpdate;
        std::map<int, int> genUpdate;

        for (int y = pos.y - EXPLOSION_RADIUS; y <= pos.y + EXPLOSION_RADIUS; y++) {
            for (int z = pos.z - EXPLOSION_RADIUS; z <= pos.z + EXPLOSION_RADIUS;
                 z++) {
                for (int x = pos.x - EXPLOSION_RADIUS;
                     x <= pos.x + EXPLOSION_RADIUS; x++) {

                    auto diff = glm::vec3((float) x - pos.x, (float) y - pos.y,
                                          (float) z - pos.z);
                    auto len =
                            sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

                    if (len >= EXPLOSION_RADIUS)
                        continue;

                    auto idx = getIdx(x, y, z);
                    auto blk = worldData[idx];

                    if (blk == Block::Air || blk == Block::Water ||
                        blk == Block::Lava || blk == Block::Bedrock)
                        continue;

                    worldData[idx] = Block::Air;

                    if (blk == Block::TNT) {
                        TNTData data;
                        data.Killed = false;
                        data.immune = false;
                        data.inRange = false;
                        data.pos = {x, y, z};
                        data.rot = {0, 0};
                        data.size = {0.1f, 0.1f, 0.1f};
                        data.vel = {0.1f, 4.0f, 0.1f};
                        data.fuse = (float) (rand() % 40 + 1) / 20.0f;

                        tnt->add_TNT(data);
                    } else {
                        DropData d;
                        d.inRange = false;
                        d.animTime = 0.0f;
                        d.pos = {(float) x + 0.5f, (float) y + 0.5f, (float) z + 0.5f};
                        d.size = {0.25f, 0.25f, 0.25f};
                        d.vel = {0.0f, 2.0f, 0.0f};
                        lookup(blk, d);

                        if (d.quantity > 0)
                            drops->add_drop(d);
                    }

                    update_lighting(x, z);
                    update_nearby_blocks({x, y, z});

                    uint16_t xx = x / 16;
                    uint16_t yy = z / 16;
                    uint32_t id = xx << 16 | (yy & 0x00FF);

                    if (genUpdate.find(x) == genUpdate.end()) {
                        genUpdate.emplace(id, 0);
                    }

                    if (surroundUpdate.find(x) == surroundUpdate.end()) {
                        surroundUpdate.emplace(x, z);
                    }
                }
            }
        }

        for (auto &[id, n]: genUpdate) {
            if (chunks.find(id) != chunks.end())
                chunks[id]->generate(this);
        }

        for (auto &[x, z]: surroundUpdate) {
            update_surroundings(x, z);
        }
    }

}// namespace CrossCraft
