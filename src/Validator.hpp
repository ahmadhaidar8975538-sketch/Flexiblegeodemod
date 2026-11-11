#pragma once

#include "PatternEngine.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace fg {
    struct ValidationReport {
        bool passed = true;
        std::vector<std::string> warnings;
    };

    class Validator {
    public:
        static ValidationReport validate(
            const std::vector<GeneratedObject>& objects,
            float levelLength,
            std::size_t maxObjects
        );
    };
}
