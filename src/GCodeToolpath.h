#pragma once
//
//  GCodeToolpath.h
//  Shared data structures for parsed G-code toolpaths.
//  Used by GCodeParser (reading) and ofxGCode (generation).
//

#include "ofMain.h"
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <cfloat>

enum class MoveType {
    Rapid,      // G0 - rapid positioning
    Linear,     // G1 - linear interpolation (feed move)
    ArcCW,      // G2 - clockwise arc
    ArcCCW      // G3 - counter-clockwise arc
};

struct GCodeMove {
    MoveType type = MoveType::Rapid;
    glm::vec3 start = glm::vec3(0);    // XYZ start position
    glm::vec3 end = glm::vec3(0);      // XYZ end position
    glm::vec3 arcCenter = glm::vec3(0); // Arc center (absolute position, for arcs only)
    float feedRate = 0;
    int sourceLineNumber = -1;          // Line in source text (0-based, for editor sync)

    /// Approximate length of this move (arc-aware).
    float getLength() const;

    /// Linearize this move into a polyline (useful for rendering arcs).
    std::vector<glm::vec3> linearize(int segments = 32) const;
};

struct GCodeToolpath {
    std::vector<GCodeMove> moves;
    glm::vec3 minBounds = glm::vec3(0);
    glm::vec3 maxBounds = glm::vec3(0);
    float totalDistance = 0;
    float estimatedTime = 0;    // In seconds, estimated from feed rates
    std::vector<std::string> errors; // Parse errors/warnings

    /// Compute bounding box, total distance, and estimated time.
    void computeBoundsAndStats();

    /// Return sorted unique Z heights found in the toolpath.
    std::vector<float> getUniqueZLayers() const;

    /// Return pointers to moves at a given Z height (within tolerance).
    std::vector<const GCodeMove*> getMovesAtZ(float z, float tolerance = 0.01f) const;

    /// Get interpolated position at a given move index and fraction through that move.
    glm::vec3 getPositionAt(int moveIndex, float fraction) const;

    /// Find the first move index originating from a given source line number (0-based).
    /// Returns -1 if no move maps to that line.
    int findMoveIndexForLine(int lineNumber) const;
    
    /// Convert a move index to a normalized position (0.0-1.0) within the toolpath.
    float moveIndexToPosition(int moveIndex) const;
    
    /// Convert a normalized position (0.0-1.0) to a move index.
    int positionToMoveIndex(float position) const;
    
    /// Get the source line number at a given normalized position.
    /// Returns -1 if no move is found.
    int getLineAtPosition(float position) const;
};
