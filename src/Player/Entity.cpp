#include "Entity.hpp"
#include "../World/World.hpp"

namespace CrossCraft {

auto test(glm::vec3 pos, World *wrld) -> bool {
    auto blk = wrld->worldData[wrld->getIdx(pos.x, pos.y, pos.z)];
    return blk != 0 && blk != 8 && blk != 6 && blk != 37 && blk != 38 &&
           blk != 39 && blk != 40 && blk != 10;
}

void Entity::test_collide(glm::vec3 testpos, World *wrld, float dt) {
    int x, y, z;
    bool testX = false;
    bool testY = false;
    bool testZ = false;

    int xMin = (int)(pos.x - size.x / 2.0f);
    int xMax = (int)(pos.x + size.x / 2.0f);
    int yMin = (int)(pos.y - size.y);
    int yMax = (int)(pos.y);
    int zMin = (int)(pos.z - size.z / 2.0f);
    int zMax = (int)(pos.z + size.z / 2.0f);

    if (vel.x < 0.0) {
        x = (int)(pos.x - 0.3f + vel.x * dt);
        testX = true;
    } else if (vel.x > 0.0) {
        x = (int)(pos.x + 0.3f + vel.x * dt);
        testX = true;
    }

    if (vel.y < 0.0) {
        y = (int)(pos.y - 1.8f + vel.y * dt);
        testY = true;
    } else if (vel.y > 0.0) {
        y = (int)(pos.y + vel.y * dt);
        testY = true;
    }

    if (vel.z < 0.0) {
        z = (int)(pos.z - 0.3f + vel.z * dt);
        testZ = true;
    } else if (vel.z > 0.0) {
        z = (int)(pos.z + 0.3f + vel.z * dt);
        testZ = true;
    }

    glm::vec3 newPosition = pos;

    if (testX) {
        bool collided = false;
        for (int y = yMin; y <= yMax; y++) {
            for (int z = zMin; z <= zMax; z++) {
                glm::ivec3 pos = glm::ivec3(x, y, z);
                if (test(pos, wrld)) {
                    collided = true;
                    vel.x = 0;
                }
            }
        }
    }

    if (testY) {
        bool collided = false;
        for (int x = xMin; x <= xMax; x++) {
            for (int z = zMin; z <= zMax; z++) {
                glm::ivec3 pos = glm::ivec3(x, y, z);
                if (test(pos, wrld)) {
                    collided = true;
                    vel.y = 0;
                    is_falling = false;
                }
            }
        }
    }

    if (testZ) {
        bool collided = false;
        for (int x = xMin; x <= xMax; x++) {
            for (int y = yMin; y <= yMax; y++) {
                glm::ivec3 pos = glm::ivec3(x, y, z);
                if (test(pos, wrld)) {
                    collided = true;
                    vel.z = 0;
                }
            }
        }
    }
}

} // namespace CrossCraft