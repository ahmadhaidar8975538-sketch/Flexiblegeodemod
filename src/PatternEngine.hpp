
#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace fg {

    struct GeneratedObject {
        int objectID = 8;
        float x = 0.f;
        float y = 105.f;
        float scale = 1.f;
    };

    class PatternEngine {
    public:
        explicit PatternEngine(std::uint32_t seed);

        std::vector<GeneratedObject> generate(
            std::size_t count,
            float length,
            float difficulty,
            float density,
            std::uint32_t bpm
        );

    private:
        std::mt19937 m_rng;

        float randomFloat(float min, float max);
        int randomInt(int min, int max);

        void add(std::vector<GeneratedObject>& out,
                 int id, float x, float y = 105.f,
                 float scale = 1.f);
    };

}
