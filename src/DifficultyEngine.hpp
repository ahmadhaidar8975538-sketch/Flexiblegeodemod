#pragma once

namespace fg {
    struct DifficultyProfile {
        float minSpacing = 0.0f;
        float maxSpacing = 0.0f;
        float densityBias = 0.0f;
    };

    class DifficultyEngine {
    public:
        static DifficultyProfile profile(float difficulty);
    };
}
