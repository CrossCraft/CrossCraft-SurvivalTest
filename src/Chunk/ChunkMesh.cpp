#include "ChunkMesh.hpp"
#include "../World/Generation/WorldGenUtil.hpp"
#include "../World/World.hpp"
#include "ChunkMeshBuilder.hpp"
#include <gtc/matrix_transform.hpp>

namespace CrossCraft::Chunk {

ChunkMesh::ChunkMesh(int x, int y, int z)
    : needsRegen(0), cX(x), cY(y), cZ(z), rtcounter(0) {
    blank = false;

    transformationMatrix =
        glm::translate(glm::mat4(1.0f), glm::vec3(cX * 16, cY * 16, cZ * 16));
}

ChunkMesh::~ChunkMesh() {}

void ChunkMesh::reset_allocate() {
    meshCollection.transparent.delete_data();
    meshCollection.opaque.delete_data();
    meshCollection.opaque.preallocate_memory();
}

void ChunkMesh::finalize_mesh() {
    meshCollection.transparent.finalize();
    meshCollection.opaque.finalize();
}

void ChunkMesh::rtick(World *wrld) {
    int x = rand() % 16; // + cX * 16;
    int y = rand() % 16; // + cY * 16;
    int z = rand() % 16; // + cZ * 16;

    int idxl = wrld->getIdxl(x + cX * 16, cY * 16, z + cZ * 16);

    bool is_dark = false;
    if (idxl >= 0 && idxl < (wrld->world_size.z * wrld->world_size.y / 16 *
                             wrld->world_size.z))
        is_dark = (!((wrld->lightData[idxl] >> (int)y) & 1));

    x += cX * 16;
    y += cY * 16;
    z += cZ * 16;

    auto idx = wrld->getIdx(x, y, z);

    y += 1;
    if (y >= 64)
        return;

    auto idx2 = wrld->getIdx(x, y, z);
    auto blk2 = Block::Air;
    blk2 = wrld->worldData[idx2];

    auto blk = Block::Air;
    blk = wrld->worldData[idx];

    auto blk2_is_valid_grass =
        (blk2 == Block::Air || blk2 == Block::Sapling ||
         blk2 == Block::Flower1 || blk2 == Block::Flower2 ||
         blk2 == Block::Mushroom1 || blk2 == Block::Mushroom2 ||
         blk2 == Block::Leaves);

    if (blk == Block::Grass && (!blk2_is_valid_grass || is_dark)) {
        wrld->worldData[idx] = Block::Dirt;
        needsRegen = true;
    }

    if (blk == Block::Dirt && blk2_is_valid_grass && !is_dark) {
        wrld->worldData[idx] = Block::Grass;
        needsRegen = true;
    }

    if (blk == Block::Sapling) {
        if (is_dark) {
            wrld->worldData[idx] = Block::Air;
            needsRegen = true;
        } else {
            WorldGenUtil::make_tree(wrld, x, z, y - 1);
            for (int tx = -2; tx < 3; tx++)
                for (int tz = -2; tz < 3; tz++)
                    wrld->update_lighting(x + tx, z + tz);
            needsRegen = true;
        }
    }

    if (blk == Block::Flower1 || blk == Block::Flower2) {
        if (is_dark) {
            wrld->worldData[idx] = Block::Air;
            needsRegen = true;
        }
    }

    if (blk == Block::Mushroom1 || blk == Block::Mushroom2) {
        if (!is_dark) {
            wrld->worldData[idx] = Block::Air;
            needsRegen = true;
        }
    }
}

void ChunkMesh::layer_check(World *wrld, int y) {
    int x = 0;
    int z = 0;

    auto worldSize = wrld->world_size;

    int metaIdx =
        cY * worldSize.z / 16 * worldSize.x / 16 + cZ * worldSize.x / 16 + cX;

    auto meta = wrld->chunksMeta[metaIdx];

    // Bottom check
    if (y == 0 || (y > 0 && !meta.layers[y - 1].is_full)) {
        for (x = 0; x < 16; x++) {
            for (z = 0; z < 16; z++) {
                int idx = ((World *)wrld)
                              ->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

                // Get block
                block_t blk = Block::Air;

                if (idx >= 0)
                    blk = wrld->worldData[idx];

                SurroundPos surround;
                surround.update(x, y, z);

                MeshBuilder::try_add_face(this, wrld, bottomFace, blk,
                                          {x, y, z}, surround.down, LIGHT_BOT);
            }
        }
    }

    // Top check
    if (y == 15 || (y < 15 && !meta.layers[y + 1].is_full)) {
        for (x = 0; x < 16; x++) {
            for (z = 0; z < 16; z++) {
                int idx = ((World *)wrld)
                              ->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

                // Get block
                block_t blk = Block::Air;

                if (idx >= 0)
                    blk = wrld->worldData[idx];

                SurroundPos surround;
                surround.update(x, y, z);

                MeshBuilder::try_add_face(this, wrld, bottomFace, blk,
                                          {x, y, z}, surround.down, LIGHT_BOT);
            }
        }
    }

    // Left check
    x = 0;
    for (z = 0; z < 16; z++) {
        int idx =
            ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

        // Get block
        block_t blk = Block::Air;

        if (idx >= 0)
            blk = wrld->worldData[idx];

        SurroundPos surround;
        surround.update(x, y, z);

        MeshBuilder::try_add_face(this, wrld, leftFace, blk, {x, y, z},
                                  surround.left, LIGHT_SIDE_X);
    }

    // Right check
    x = 15;
    for (z = 0; z < 16; z++) {
        int idx =
            ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

        // Get block
        block_t blk = Block::Air;

        if (idx >= 0)
            blk = wrld->worldData[idx];

        SurroundPos surround;
        surround.update(x, y, z);

        MeshBuilder::try_add_face(this, wrld, leftFace, blk, {x, y, z},
                                  surround.right, LIGHT_SIDE_X);
    }

    // Back check
    z = 0;
    for (x = 0; x < 16; x++) {
        int idx =
            ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

        // Get block
        block_t blk = Block::Air;

        if (idx >= 0)
            blk = wrld->worldData[idx];

        SurroundPos surround;
        surround.update(x, y, z);

        MeshBuilder::try_add_face(this, wrld, leftFace, blk, {x, y, z},
                                  surround.back, LIGHT_SIDE_Z);
    }

    // Front check
    z = 15;
    for (x = 0; x < 16; x++) {
        int idx =
            ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

        // Get block
        block_t blk = Block::Air;

        if (idx >= 0)
            blk = wrld->worldData[idx];

        SurroundPos surround;
        surround.update(x, y, z);

        MeshBuilder::try_add_face(this, wrld, leftFace, blk, {x, y, z},
                                  surround.front, LIGHT_SIDE_Z);
    }
}

void ChunkMesh::full_check(World *wrld) {
    int x = 0;
    int y = 0;
    int z = 0;

    // Bottom face
    y = 0;
    for (x = 0; x < 16; x++) {
        for (z = 0; z < 16; z++) {
            int idx =
                ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

            // Get block
            block_t blk = Block::Air;

            if (idx >= 0)
                blk = wrld->worldData[idx];

            SurroundPos surround;
            surround.update(x, y, z);

            MeshBuilder::try_add_face(this, wrld, bottomFace, blk, {x, y, z},
                                      surround.down, LIGHT_BOT);
        }
    }

    // Top face
    y = 15;
    for (x = 0; x < 16; x++) {
        for (z = 0; z < 16; z++) {
            int idx =
                ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

            // Get block
            block_t blk = Block::Air;

            if (idx >= 0)
                blk = wrld->worldData[idx];

            SurroundPos surround;
            surround.update(x, y, z);

            MeshBuilder::try_add_face(this, wrld, topFace, blk, {x, y, z},
                                      surround.up, LIGHT_TOP);
        }
    }

    // Left face
    x = 0;
    for (z = 0; z < 16; z++) {
        for (y = 0; y < 16; y++) {
            int idx =
                ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

            // Get block
            block_t blk = Block::Air;

            if (idx >= 0)
                blk = wrld->worldData[idx];

            SurroundPos surround;
            surround.update(x, y, z);

            MeshBuilder::try_add_face(this, wrld, leftFace, blk, {x, y, z},
                                      surround.left, LIGHT_SIDE_X);
        }
    }

    // Right face
    x = 15;
    for (z = 0; z < 16; z++) {
        for (y = 0; y < 16; y++) {
            int idx =
                ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

            // Get block
            block_t blk = Block::Air;

            if (idx >= 0)
                blk = wrld->worldData[idx];

            SurroundPos surround;
            surround.update(x, y, z);

            MeshBuilder::try_add_face(this, wrld, rightFace, blk, {x, y, z},
                                      surround.right, LIGHT_SIDE_X);
        }
    }

    // Back face
    z = 0;
    for (x = 0; x < 16; x++) {
        for (y = 0; y < 16; y++) {

            int idx =
                ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

            // Get block
            block_t blk = Block::Air;

            if (idx >= 0)
                blk = wrld->worldData[idx];

            SurroundPos surround;
            surround.update(x, y, z);

            MeshBuilder::try_add_face(this, wrld, backFace, blk, {x, y, z},
                                      surround.back, LIGHT_SIDE_Z);
        }
    }

    // Front face
    z = 15;
    for (x = 0; x < 16; x++) {
        for (y = 0; y < 16; y++) {

            int idx =
                ((World *)wrld)->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

            // Get block
            block_t blk = Block::Air;

            if (idx >= 0)
                blk = wrld->worldData[idx];

            SurroundPos surround;
            surround.update(x, y, z);

            MeshBuilder::try_add_face(this, wrld, frontFace, blk, {x, y, z},
                                      surround.front, LIGHT_SIDE_Z);
        }
    }
}

void ChunkMesh::generate(const World *wrld) {

    // Reset + Allocate
    reset_allocate();

    auto worldSize = wrld->world_size;

    int metaIdx =
        cY * worldSize.z / 16 * worldSize.x / 16 + cZ * worldSize.x / 16 + cX;

    auto meta = wrld->chunksMeta[metaIdx];

    if (meta.is_full) {
        full_check((World *)wrld);
    } else {

        // Loop over the mesh
        for (int y = 0; y < 16; y++) {

            if (meta.layers[y].is_empty)
                continue;

            if (meta.layers[y].is_full) {
                layer_check((World *)wrld, y);
                continue;
            }

            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {

                    int idx =
                        ((World *)wrld)
                            ->getIdx(x + cX * 16, y + cY * 16, z + cZ * 16);

                    // Get block
                    block_t blk = Block::Air;

                    if (idx >= 0)
                        blk = wrld->worldData[idx];

                    // Skip air
                    if (blk == 0)
                        continue;

                    if (blk == Block::Flower1 || blk == Block::Flower2 ||
                        blk == Block::Mushroom1 || blk == Block::Mushroom2 ||
                        blk == Block::Sapling) {
                        MeshBuilder::add_xface_to_mesh(
                            this, getTexCoord(blk, LIGHT_TOP), {x, y, z},
                            LIGHT_TOP, wrld);
                        continue;
                    }

                    // Update surrounding positions
                    SurroundPos surround;
                    surround.update(x, y, z);

                    // Add 6 faces

                    if (blk == Block::Slab) {
                        MeshBuilder::add_slab_to_mesh(this, wrld, blk,
                                                      {x, y, z}, surround);
                        continue;
                    }

                    MeshBuilder::add_block_to_mesh(this, wrld, blk, {x, y, z},
                                                   surround);
                }
            }
        }
    }

    // Finalize the mesh
    finalize_mesh();
}
void ChunkMesh::generate_blank() {
    // Reset + Allocate
    reset_allocate();
    // Finalize the mesh
    finalize_mesh();

    blank = true;
}

void ChunkMesh::generate_border() {

    // Reset + Allocate
    reset_allocate();

    // Loop over the mesh
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 16; y++) {

                auto h = cY * 16 + y;

                // Get block
                block_t blk = Block::Bedrock;
                if (h >= 30)
                    blk = Block::Water;

                // Update surrounding positions
                SurroundPos surround;
                surround.update(x, y, z);

                // Add 6 faces
                if (blk != Block::Water) {
                    if (h == 29) {
                        MeshBuilder::add_face_to_mesh(
                            this, topFace, getTexCoord(blk, LIGHT_TOP),
                            {x, y, z}, LIGHT_TOP, MeshSelection::Opaque);
                    }

                    if (x == 0) {
                        MeshBuilder::add_face_to_mesh(
                            this, leftFace, getTexCoord(blk, LIGHT_SIDE_X),
                            {x, y, z}, LIGHT_SIDE_X, MeshSelection::Opaque);
                    } else if (x == 15) {
                        MeshBuilder::add_face_to_mesh(
                            this, rightFace, getTexCoord(blk, LIGHT_SIDE_X),
                            {x, y, z}, LIGHT_SIDE_X, MeshSelection::Opaque);
                    }
                    if (z == 0) {
                        MeshBuilder::add_face_to_mesh(
                            this, backFace, getTexCoord(blk, LIGHT_SIDE_Z),
                            {x, y, z}, LIGHT_SIDE_Z, MeshSelection::Opaque);
                    } else if (z == 15) {
                        MeshBuilder::add_face_to_mesh(
                            this, frontFace, getTexCoord(blk, LIGHT_SIDE_Z),
                            {x, y, z}, LIGHT_SIDE_Z, MeshSelection::Opaque);
                    }

                } else {

                    if (h == 31) {
                        MeshBuilder::add_face_to_mesh(
                            this, topFace, getTexCoord(blk, LIGHT_TOP),
                            {x, y - 0.1f, z}, LIGHT_TOP,
                            MeshSelection::Transparent);
                    }

                    if (x == 0 && cX == 16) {
                        MeshBuilder::add_face_to_mesh(
                            this, leftFace, getTexCoord(blk, LIGHT_SIDE_X),
                            {x, y, z}, LIGHT_SIDE_X,
                            MeshSelection::Transparent);
                    } else if (x == 15 && cX == -1) {
                        MeshBuilder::add_face_to_mesh(
                            this, rightFace, getTexCoord(blk, LIGHT_SIDE_X),
                            {x, y, z}, LIGHT_SIDE_X,
                            MeshSelection::Transparent);
                    }
                    if (z == 0 && cZ == 16) {
                        MeshBuilder::add_face_to_mesh(
                            this, backFace, getTexCoord(blk, LIGHT_SIDE_Z),
                            {x, y, z}, LIGHT_SIDE_Z,
                            MeshSelection::Transparent);
                    } else if (z == 15 && cZ == -1) {
                        MeshBuilder::add_face_to_mesh(
                            this, frontFace, getTexCoord(blk, LIGHT_SIDE_Z),
                            {x, y, z}, LIGHT_SIDE_Z,
                            MeshSelection::Transparent);
                    }
                }
            }
        }
    }

    // Finalize the mesh
    finalize_mesh();
}

void ChunkMesh::draw(MeshSelection meshSel) {
    if (blank)
        return;

#ifndef PSP
    // Set matrix
    Rendering::RenderContext::get().matrix_model(transformationMatrix);

#else
    Rendering::RenderContext::get().matrix_translate(
        glm::vec3(cX * 16, cY * 16, cZ * 16));
#endif

    // Draw
    meshCollection.select(meshSel)->draw();

    Rendering::RenderContext::get().matrix_clear();
}

} // namespace CrossCraft::Chunk
