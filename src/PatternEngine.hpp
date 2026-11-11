#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace fg {
    enum class ObjectKind {
        Spike,
        Block,
        SmallSpike,
        Saw
    };

    struct GeneratedObject {
        ObjectKind kind = ObjectKind::Spike;
        float x = 0.0f;
        float y = 0.0f;
        float scale = 1.0f;
    };

    class PatternEngine {
    public:
        explicit PatternEngine(std::uint32_t seed);
        std::vector<GeneratedObject> generate(
            std::size_t count,
            float length,
            float difficulty,
            float density
        );

    private:
        std::mt19937 m_rng;
        float randomFloat(float min, float max);
        int randomInt(int min, int max);
    };
}
