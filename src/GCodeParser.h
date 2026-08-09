#pragma once
//
//  GCodeParser.h
//  Parses G-code text into a GCodeToolpath.
//  Supports G0, G1, G2, G3, G17-G19, G20/G21, G28, G90/G91, M-codes, comments.
//

#include "GCodeToolpath.h"
#include <string>
#include <vector>
#include <map>

class GCodeParser {
public:
    GCodeParser();

    // Parse a full G-code string (may contain multiple lines)
    GCodeToolpath parse(const std::string& gcodeText);

    // Parse a single line, updating internal state and appending to toolpath
    void parseLine(const std::string& line, int lineNumber, GCodeToolpath& toolpath);

    // Reset parser state to defaults
    void reset();

private:
    // Machine state
    glm::vec3 m_currentPos;
    float m_feedRate;

    // Modal state
    int m_motionMode;       // 0=G0, 1=G1, 2=G2, 3=G3
    int m_distanceMode;     // 90=absolute, 91=incremental
    int m_unitMode;         // 20=inches, 21=mm

    // Parsed word values from a single line
    struct WordSet {
        std::map<char, float> words;
        bool has(char c) const { return words.count(c) > 0; }
        float get(char c, float def = 0) const {
            auto it = words.find(c);
            return it != words.end() ? it->second : def;
        }
    };

    // Strip comments and return cleaned line
    std::string stripComments(const std::string& line);

    // Tokenize a cleaned line into letter-value pairs
    WordSet tokenize(const std::string& cleanLine);

    // Process G-code words and generate moves
    void processWords(const WordSet& words, int lineNumber, GCodeToolpath& toolpath);

    // Generate arc move (G2/G3)
    void processArc(bool clockwise, const WordSet& words, int lineNumber, GCodeToolpath& toolpath);
};
