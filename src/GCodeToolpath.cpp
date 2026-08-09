//
//  GCodeToolpath.cpp
//  Implementation of GCodeMove and GCodeToolpath methods.
//

#include "GCodeToolpath.h"
#include <cfloat>

//--------------------------------------------------------------
// Helper: compute arc sweep angle given start/end vectors from center
//--------------------------------------------------------------
static float computeArcSweep(const glm::vec2& v1, const glm::vec2& v2, MoveType type) {
    float angle1 = atan2(v1.y, v1.x);
    float angle2 = atan2(v2.y, v2.x);
    float sweep = angle2 - angle1;
    if (type == MoveType::ArcCW) {
        if (sweep > 0) sweep -= TWO_PI;
        if (std::abs(sweep) < 1e-6f) sweep = -TWO_PI;
    } else {
        if (sweep < 0) sweep += TWO_PI;
        if (std::abs(sweep) < 1e-6f) sweep = TWO_PI;
    }
    return sweep;
}

//--------------------------------------------------------------
// GCodeMove
//--------------------------------------------------------------

float GCodeMove::getLength() const {
    if (type == MoveType::ArcCW || type == MoveType::ArcCCW) {
        float radius = glm::length(glm::vec2(start) - glm::vec2(arcCenter));
        glm::vec2 v1 = glm::vec2(start) - glm::vec2(arcCenter);
        glm::vec2 v2 = glm::vec2(end) - glm::vec2(arcCenter);
        float sweep = computeArcSweep(v1, v2, type);
        float arcLen2D = std::abs(sweep) * radius;
        float dz = end.z - start.z;
        return std::sqrt(arcLen2D * arcLen2D + dz * dz);
    }
    return glm::length(end - start);
}

std::vector<glm::vec3> GCodeMove::linearize(int segments) const {
    std::vector<glm::vec3> pts;
    if (type == MoveType::ArcCW || type == MoveType::ArcCCW) {
        glm::vec2 center2D = glm::vec2(arcCenter);
        glm::vec2 v1 = glm::vec2(start) - center2D;
        glm::vec2 v2 = glm::vec2(end) - center2D;
        float angle1 = atan2(v1.y, v1.x);
        float sweep = computeArcSweep(v1, v2, type);
        float radius = glm::length(v1);
        pts.reserve(segments + 1);
        for (int i = 0; i <= segments; i++) {
            float t = (float)i / (float)segments;
            float angle = angle1 + sweep * t;
            float z = start.z + (end.z - start.z) * t;
            pts.push_back(glm::vec3(
                center2D.x + cos(angle) * radius,
                center2D.y + sin(angle) * radius,
                z
            ));
        }
    } else {
        pts.push_back(start);
        pts.push_back(end);
    }
    return pts;
}

//--------------------------------------------------------------
// GCodeToolpath
//--------------------------------------------------------------

static constexpr int kArcBoundsSegments = 16;
static constexpr float kDefaultRapidRate = 3000.0f; // mm/min

void GCodeToolpath::computeBoundsAndStats() {
    if (moves.empty()) {
        minBounds = maxBounds = glm::vec3(0);
        totalDistance = 0;
        estimatedTime = 0;
        return;
    }
    minBounds = glm::vec3(FLT_MAX);
    maxBounds = glm::vec3(-FLT_MAX);
    totalDistance = 0;
    estimatedTime = 0;

    for (const auto& m : moves) {
        minBounds = glm::min(minBounds, glm::min(m.start, m.end));
        maxBounds = glm::max(maxBounds, glm::max(m.start, m.end));

        if (m.type == MoveType::ArcCW || m.type == MoveType::ArcCCW) {
            auto pts = m.linearize(kArcBoundsSegments);
            for (const auto& p : pts) {
                minBounds = glm::min(minBounds, p);
                maxBounds = glm::max(maxBounds, p);
            }
        }

        float len = m.getLength();
        totalDistance += len;

        float rate = m.feedRate;
        if (m.type == MoveType::Rapid || rate <= 0) {
            rate = kDefaultRapidRate;
        }
        estimatedTime += (len / rate) * 60.0f; // seconds
    }
}

std::vector<float> GCodeToolpath::getUniqueZLayers() const {
    std::set<float> zSet;
    for (const auto& m : moves) {
        float z1 = std::round(m.start.z * 1000.0f) / 1000.0f;
        float z2 = std::round(m.end.z * 1000.0f) / 1000.0f;
        zSet.insert(z1);
        zSet.insert(z2);
    }
    return std::vector<float>(zSet.begin(), zSet.end());
}

std::vector<const GCodeMove*> GCodeToolpath::getMovesAtZ(float z, float tolerance) const {
    std::vector<const GCodeMove*> result;
    for (const auto& m : moves) {
        if (std::abs(m.start.z - z) <= tolerance || std::abs(m.end.z - z) <= tolerance) {
            result.push_back(&m);
        }
    }
    return result;
}

glm::vec3 GCodeToolpath::getPositionAt(int moveIndex, float fraction) const {
    if (moves.empty() || moveIndex < 0) return glm::vec3(0);
    if (moveIndex >= (int)moves.size()) return moves.back().end;
    const auto& m = moves[moveIndex];
    fraction = std::clamp(fraction, 0.0f, 1.0f);
    if (m.type == MoveType::ArcCW || m.type == MoveType::ArcCCW) {
        auto pts = m.linearize(32);
        float totalLen = 0;
        std::vector<float> cumLen;
        cumLen.push_back(0);
        for (size_t i = 1; i < pts.size(); i++) {
            totalLen += glm::length(pts[i] - pts[i - 1]);
            cumLen.push_back(totalLen);
        }
        float targetLen = fraction * totalLen;
        for (size_t i = 1; i < cumLen.size(); i++) {
            if (cumLen[i] >= targetLen) {
                float segFrac = (cumLen[i] - cumLen[i - 1]) > 0 ?
                    (targetLen - cumLen[i - 1]) / (cumLen[i] - cumLen[i - 1]) : 0;
                return glm::mix(pts[i - 1], pts[i], segFrac);
            }
        }
        return pts.back();
    }
    return glm::mix(m.start, m.end, fraction);
}

int GCodeToolpath::findMoveIndexForLine(int lineNumber) const {
    for (int i = 0; i < (int)moves.size(); i++) {
        if (moves[i].sourceLineNumber == lineNumber) return i;
    }
    // If exact match not found, find the closest move at or before this line
    int best = -1;
    for (int i = 0; i < (int)moves.size(); i++) {
        if (moves[i].sourceLineNumber >= 0 && moves[i].sourceLineNumber <= lineNumber) {
            best = i;
        }
    }
    return best;
}

float GCodeToolpath::moveIndexToPosition(int moveIndex) const {
    if (moves.empty()) return 0.0f;
    return std::clamp((float)(moveIndex + 1) / (float)moves.size(), 0.0f, 1.0f);
}

int GCodeToolpath::positionToMoveIndex(float position) const {
    if (moves.empty()) return -1;
    position = std::clamp(position, 0.0f, 1.0f);
    int idx = (int)(position * (float)moves.size()) - 1;
    return std::clamp(idx, 0, (int)moves.size() - 1);
}

int GCodeToolpath::getLineAtPosition(float position) const {
    int idx = positionToMoveIndex(position);
    if (idx < 0 || idx >= (int)moves.size()) return -1;
    return moves[idx].sourceLineNumber;
}
