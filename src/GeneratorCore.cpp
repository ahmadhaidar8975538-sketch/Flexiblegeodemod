
#include "GeneratorCore.hpp"

#include <algorithm>

namespace fg {

    GenerationResult GeneratorCore::generate(
        const GeneratorSettings& input
    ) const {
        GeneratorSettings settings = input;

        settings.length = std::clamp(
            settings.length, 100.f, 10000.f
        );

        settings.density = std::clamp(
            settings.density, 0.f, 1.f
        );

        settings.difficulty = std::clamp(
            settings.difficulty, 0.f, 1.f
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
            static_cast<float>(settings.bpm) / 120.f;

        const float targetCount =
            settings.density *
            settings.length / 22.f *
            bpmFactor;

        const auto count = std::min(
            settings.maxObjects,
            static_cast<std::size_t>(
                std::max(1.f, targetCount)
            )
        );

        result.objects = patterns.generate(
            count,
            settings.length,
            settings.difficulty,
            settings.density,
            settings.bpm
        );

        result.validation = Validator::validate(
            result.objects,
            settings.length,
            settings.maxObjects
        );

        return result;
    }

}
