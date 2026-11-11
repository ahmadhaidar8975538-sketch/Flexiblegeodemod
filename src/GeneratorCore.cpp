
#include "GeneratorCore.hpp"

#include <algorithm>
#include <cmath>

namespace fg {

    GenerationResult GeneratorCore::generate(
        const GeneratorSettings& input
    ) const {

        GeneratorSettings settings = input;

        settings.length = std::clamp(
            settings.length, 100.0f, 10000.0f
        );

        settings.density = std::clamp(
            settings.density, 0.0f, 1.0f
        );

        settings.difficulty = std::clamp(
            settings.difficulty, 0.0f, 1.0f
        );

        settings.bpm = std::clamp<std::uint32_t>(
            settings.bpm, 30, 300
        );

        settings.maxObjects = std::clamp<std::size_t>(
            settings.maxObjects, 1, 40000
        );

        PatternEngine patterns(settings.seed);

        GenerationResult result;
        result.seedUsed = settings.seed;

        const float bpmFactor =
            static_cast<float>(settings.bpm) / 120.0f;

        const float targetCount =
            settings.density *
            static_cast<float>(settings.length) /
            22.0f * bpmFactor;

        const auto count = std::min(
            settings.maxObjects,
            static_cast<std::size_t>(
                std::max(1.0f, targetCount)
            )
        );

        result.objects = patterns.generate(
            count,
            settings.length,
            settings.difficulty,
            settings.density
        );

        result.validation = Validator::validate(
            result.objects,
            settings.length,
            settings.maxObjects
        );

        return result;
    }

}
