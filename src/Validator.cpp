#include "Validator.hpp"
#include <cmath>

namespace fg {
    ValidationReport Validator::validate(
        const std::vector<GeneratedObject>& objects,
        float levelLength,
        std::size_t maxObjects
    ) {
        ValidationReport report;

        if (objects.empty()) {
            report.passed = false;
            report.warnings.emplace_back("Generator produced no objects.");
            return report;
        }

        if (objects.size() > maxObjects) {
            report.passed = false;
            report.warnings.emplace_back("Object count exceeds configured limit.");
        }

        float previousX = -1.0f;
        for (const auto& object : objects) {
            if (!std::isfinite(object.x) || !std::isfinite(object.y)
                || !std::isfinite(object.scale)) {
                report.passed = false;
                report.warnings.emplace_back("Non-finite object coordinate detected.");
                break;
            }
            if (object.x < 0.0f || object.x > levelLength) {
                report.passed = false;
                report.warnings.emplace_back("Object is outside the requested level length.");
                break;
            }
            if (object.x < previousX) {
                report.passed = false;
                report.warnings.emplace_back("Object positions are not ordered.");
                break;
            }
            previousX = object.x;
        }

        if (report.passed) {
            report.warnings.emplace_back(
                "Structural checks passed; this does not prove player-completable gameplay."
            );
        }
        return report;
    }
}
