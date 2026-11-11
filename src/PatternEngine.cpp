#include "PatternEngine.hpp"
#include <algorithm>
#include <cmath>

namespace fg {
    PatternEngine::PatternEngine(std::uint32_t seed) : m_rng(seed) {}

    float PatternEngine::randomFloat(float min, float max) {
        return std::uniform_real_distribution<float>(min, max)(m_rng);
    }

    int PatternEngine::randomInt(int min, int max) {
        return std::uniform_int_distribution<int>(min, max)(m_rng);
    }

    std::vector<GeneratedObject> PatternEngine::generate(
        std::size_t count, float length, float difficulty, float density
    ) {
        std::vector<GeneratedObject> result;
        result.reserve(count);

        const float safeDifficulty = std::clamp(difficulty, 0.0f, 1.0f);
        const float safeDensity = std::clamp(density, 0.0f, 1.0f);
        const float baseGap = 58.0f - safeDifficulty * 25.0f;
        const float minGap = std::max(18.0f, baseGap - safeDensity * 8.0f);
        const float maxGap = baseGap + 34.0f - safeDensity * 10.0f;

        float x = 100.0f;
        for (std::size_t i = 0; i < count && x < length - 80.0f; ++i) {
            const int choice = randomInt(0, 99);
            ObjectKind kind = ObjectKind::Spike;
            if (choice < 42) kind = ObjectKind::Spike;
            else if (choice < 70) kind = ObjectKind::Block;
            else if (choice < 88) kind = ObjectKind::SmallSpike;
            else kind = ObjectKind::Saw;

            const float y = randomFloat(90.0f, 210.0f);
            const float scale = randomFloat(0.85f, 1.15f);
            result.push_back({kind, x, y, scale});

            // Paired objects form a small cluster; advance past the pair
            // before generating the next cluster to preserve X ordering.
            const bool addPair = randomInt(0, 99)
                < static_cast<int>(12 + safeDensity * 20)
                && result.size() < count;
            if (addPair) {
                const float pairOffset = randomFloat(18.0f, 30.0f);
                result.push_back({
                    randomInt(0, 1) == 0 ? ObjectKind::SmallSpike : ObjectKind::Block,
                    x + pairOffset,
                    std::clamp(y + randomFloat(-35.0f, 35.0f), 70.0f, 230.0f),
                    randomFloat(0.8f, 1.0f)
                });
                x += pairOffset;
            }

            x += randomFloat(minGap, maxGap);
        }

        return result;
    }
}
