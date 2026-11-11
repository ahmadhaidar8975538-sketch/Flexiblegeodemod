
#include "PatternEngine.hpp"

#include <algorithm>
#include <cmath>

namespace fg {

    PatternEngine::PatternEngine(std::uint32_t seed)
        : m_rng(seed) {}

    float PatternEngine::randomFloat(float min, float max) {
        return std::uniform_real_distribution<float>(min, max)(m_rng);
    }

    int PatternEngine::randomInt(int min, int max) {
        return std::uniform_int_distribution<int>(min, max)(m_rng);
    }

    void PatternEngine::add(
        std::vector<GeneratedObject>& out,
        int id,
        float x,
        float y,
        float scale
    ) {
        out.push_back({id, x, y, scale});
    }

    std::vector<GeneratedObject> PatternEngine::generate(
        std::size_t count,
        float length,
        float difficulty,
        float density,
        std::uint32_t bpm
    ) {
        std::vector<GeneratedObject> out;
        out.reserve(count);

        difficulty = std::clamp(difficulty, 0.f, 1.f);
        density = std::clamp(density, 0.f, 1.f);
        bpm = std::clamp<std::uint32_t>(bpm, 30, 300);

        // Approximate beat spacing in editor coordinates.
        const float beat = 6000.f / static_cast<float>(bpm);
        const float gap = beat * (1.25f - difficulty * 0.45f);

        float x = 150.f;
        int section = 0;

        while (out.size() < count && x < length - 150.f) {
            const int pattern = randomInt(0, 9);
            const float y = 105.f;

            switch (pattern) {

                // Single jump
                case 0:
                case 1:
                    add(out, 8, x, y);
                    x += gap * 2.f;
                    break;

                // Double spike
                case 2:
                    add(out, 8, x, y);
                    add(out, 8, x + gap * 0.65f, y);
                    x += gap * 2.5f;
                    break;

                // Triple spike: mostly for harder difficulties
                case 3:
                    add(out, 8, x, y);
                    add(out, 8, x + gap * 0.55f, y);
                    if (difficulty > 0.45f)
                        add(out, 8, x + gap * 1.1f, y);
                    x += gap * 3.f;
                    break;

                // Block followed by a spike
                case 4:
                    add(out, 1, x, y);
                    add(out, 8, x + gap * 1.2f, y);
                    x += gap * 2.8f;
                    break;

                // Orb-assisted jump sequence
                case 5: {
                    add(out, 8, x, y);

                    const int orbTypes[] = {36, 141, 1332};
                    const int orb = orbTypes[randomInt(0, 2)];

                    add(out, orb, x + gap * 1.25f, y + 85.f);
                    add(out, 8, x + gap * 2.3f, y);

                    x += gap * 3.5f;
                    break;
                }

                // Blue gravity orb sequence
                case 6:
                    add(out, 84, x + gap, y + 90.f);
                    add(out, 8, x + gap * 2.2f, y + 180.f);
                    x += gap * 3.5f;
                    break;

                // Small spike rhythm
                case 7:
                    add(out, 39, x, y);
                    add(out, 39, x + gap * 0.8f, y);
                    add(out, 8, x + gap * 1.6f, y);
                    x += gap * 3.f;
                    break;

                // Saw obstacle
                case 8:
                    add(out, 88, x, y + 45.f);
                    add(out, 8, x + gap * 1.5f, y);
                    x += gap * 3.f;
                    break;

                // Recovery space / breathing room
                default:
                    x += gap * 2.5f;
                    break;
            }

            // Controlled section transitions.
            // Avoid placing portals repeatedly inside every pattern.
            ++section;

            if (section % 8 == 0 && out.size() + 1 < count) {
                const int speedPortals[] = {200, 201, 202, 203};
                const int portal = speedPortals[
                    std::clamp(
                        static_cast<int>(difficulty * 3.99f),
                        0, 3
                    )
                ];

                add(out, portal, x, y);
                x += gap * 1.5f;
            }

            if (section % 16 == 0 && out.size() + 1 < count) {
                add(out, 10, x, y);
                x += gap * 1.5f;
            }
        }

        // Keep generated coordinates inside the requested level.
        out.erase(
            std::remove_if(
                out.begin(),
                out.end(),
                [length](const GeneratedObject& obj) {
                    return !std::isfinite(obj.x) ||
                           !std::isfinite(obj.y) ||
                           obj.x < 0.f ||
                           obj.x > length;
                }
            ),
            out.end()
        );

        return out;
    }

}
