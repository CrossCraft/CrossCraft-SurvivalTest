/**
 * @file ChunkMesh.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief Describes a 16x16x16 chunk mesh
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include <Rendering/Rendering.hpp>
#include "../World/World.hpp"
#include "ChunkUtil.hpp"

using namespace Stardust_Celeste;

namespace CrossCraft {

    class World;

    namespace Chunk {
        /**
 * @brief Chunk mesh object.
 */
        class ChunkMesh {
        public:
            /**
     * @brief Construct a new Chunk Mesh object at a location
     *
     * @param x Chunk X position
     * @param y Chunk Y position
     * @param z Chunk Z position
     */
            ChunkMesh(int x, int y, int z);

            /**
     * @brief Destroy the Chunk Mesh object
     */
            ~ChunkMesh();

            /**
     * @brief Generate a brand new mesh
     *
     * @param wrld - World to reference
     */
            void generate(const World *wrld);

            /**
     * @brief Generate a brand new mesh
     *
     * @param wrld - World to reference
     */
            void generate_border();

            void generate_blank();

            /**
     * @brief Draw the chunk mesh
     *
     */
            void draw(MeshSelection meshSel);

            /**
     * @brief Random Ticks the section
     *
     * @param wrld
     */
            void rtick(World *wrld);

            bool needsRegen;

        private:
            void full_check(World *wrld);
            void layer_check(World *wrld, int y);
            /**
     * @brief Reset a mesh and allocate space
     *
     */
            void reset_allocate();

            /**
     * @brief Finalize a mesh - cleanup memory use
     *
     */
            void finalize_mesh();

            int cX, cY, cZ;
            int rtcounter;

            MeshCollection meshCollection;

            friend class MeshBuilder;
            bool blank;
        };

    }// namespace Chunk
}// namespace CrossCraft