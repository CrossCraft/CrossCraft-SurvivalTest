#include "CrossCraftGenerator.hpp"
#include "NoiseUtil.hpp"
#include "WorldGenUtil.hpp"

extern "C" {
extern void generate(uint32_t seed, void *data);

extern double onoise(uint64_t octaves, double x, double y, uint32_t s);
extern double noise1(double x, double y);
extern double noise2(double x, double y);

void generate_heightmap(int32_t *heightMap) {
    for (int z = 0; z < 256; z++) {
        for (int x = 0; x < 256; x++) {
            auto xf = static_cast<float>(x);
            auto zf = static_cast<float>(z);

            auto heightLow = noise1(xf * 1.3, zf * 1.3) / 6.0 - 4.0;
            auto heightHigh = noise2(xf * 1.3, zf * 1.3) / 5.0 + 6.0;

            auto result = 0.0;

            if (onoise(6, xf, zf, 5) / 8.0 > 0) {
                result = heightLow;
            } else {
                if (heightHigh > heightLow)
                    result = heightHigh;
                else
                    result = heightLow;
            }

            result /= 2.0;

            if (result < 0)
                result *= 0.8;

            heightMap[x + z * 256] = 32 + result;
        }
    }
};

void smooth_heightmap(int32_t *heightMap) {
    for (int z = 0; z < 256; z++) {
        for (int x = 0; x < 256; x++) {
            auto xf = static_cast<float>(x);
            auto zf = static_cast<float>(z);

            auto a = noise1(xf * 2, zf * 2) / 8.0;
            int b = 0;

            if (noise2(xf * 2, zf * 2) > 0) {
                b = 1;
            }

            if (a > 2) {
                heightMap[x + z * 256] =
                    ((static_cast<float>(heightMap[x + z * 256]) - b) / 2.0) *
                        2 +
                    b;
            }
        }
    }
}
}
namespace CrossCraft {

auto CrossCraftGenerator::generate(World *wrld) -> void {
    ::generate(NoiseUtil::seed, wrld->worldData);

    // Bottom of World = Bedrock
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            auto idx = (0 * 256 * 256) + (z * 256) + x;
            wrld->worldData[idx] = Block::Bedrock;
        }
    }

    // Update Lighting
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            wrld->update_lighting(x, z);
        }
    }

    wrld->generate_meta();
}

} // namespace CrossCraft