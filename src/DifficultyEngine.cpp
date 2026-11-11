#include "DifficultyEngine.hpp"
#include <algorithm>

namespace fg {
    DifficultyProfile DifficultyEngine::profile(float difficulty) {
        const float value = std::clamp(difficulty, 0.0f, 1.0f);
        return {
            58.0f - value * 25.0f,
            92.0f - value * 30.0f,
            0.25f + value * 0.5f
        };
    }
}
