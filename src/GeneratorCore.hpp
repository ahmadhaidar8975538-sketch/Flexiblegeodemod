#pragma once

#include "PatternEngine.hpp"
#include "Validator.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace fg {
    struct GeneratorSettings {
        float length = 1200.0f;
        float density = 0.55f;       // 0.0 .. 1.0
        float difficulty = 0.45f;    // 0.0 .. 1.0
        std::uint32_t seed = 12345;
        std::size_t maxObjects = 180;
    };

    struct GenerationResult {
        std::vector<GeneratedObject> objects;
        ValidationReport validation;
        std::uint32_t seedUsed = 0;
    };

    class GeneratorCore {
    public:
        GenerationResult generate(const GeneratorSettings& settings) const;
    };
}
