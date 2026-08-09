//
//  GCodeParser.cpp
//  Parses G-code text into GCodeToolpath.
//

#include "GCodeParser.h"
#include <sstream>
#include <cctype>
#include <cmath>

GCodeParser::GCodeParser() {
    reset();
}

void GCodeParser::reset() {
    m_currentPos = glm::vec3(0);
    m_feedRate = 0;
    m_motionMode = 0;      // G0 (rapid)
    m_distanceMode = 90;   // Absolute
    m_unitMode = 21;       // mm
}

GCodeToolpath GCodeParser::parse(const std::string& gcodeText) {
    reset();
    GCodeToolpath toolpath;

    std::istringstream stream(gcodeText);
    std::string line;
    int lineNumber = 0;

    while (std::getline(stream, line)) {
        parseLine(line, lineNumber, toolpath);
        lineNumber++;
    }

    toolpath.computeBoundsAndStats();
    return toolpath;
}

void GCodeParser::parseLine(const std::string& line, int lineNumber, GCodeToolpath& toolpath) {
    std::string clean = stripComments(line);

    // Skip empty lines
    if (clean.empty()) return;

    // Skip lines that are only whitespace
    bool allSpace = true;
    for (char c : clean) {
        if (!std::isspace((unsigned char)c)) { allSpace = false; break; }
    }
    if (allSpace) return;

    WordSet words = tokenize(clean);
    if (words.words.empty()) return;

    processWords(words, lineNumber, toolpath);
}

std::string GCodeParser::stripComments(const std::string& line) {
    std::string result;
    result.reserve(line.size());
    bool inParenComment = false;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == ';') {
            // Rest of line is comment
            break;
        }
        if (c == '(') {
            inParenComment = true;
            continue;
        }
        if (c == ')') {
            inParenComment = false;
            continue;
        }
        if (!inParenComment) {
            result += c;
        }
    }
    return result;
}

GCodeParser::WordSet GCodeParser::tokenize(const std::string& cleanLine) {
    WordSet ws;
    size_t i = 0;
    size_t len = cleanLine.size();

    while (i < len) {
        // Skip whitespace
        while (i < len && std::isspace((unsigned char)cleanLine[i])) i++;
        if (i >= len) break;

        char letter = std::toupper((unsigned char)cleanLine[i]);
        if (!std::isalpha((unsigned char)letter)) {
            i++;
            continue;
        }
        i++;

        // Skip whitespace between letter and number
        while (i < len && std::isspace((unsigned char)cleanLine[i])) i++;

        // Parse the number
        std::string numStr;
        while (i < len && (std::isdigit((unsigned char)cleanLine[i]) ||
                           cleanLine[i] == '.' || cleanLine[i] == '-' || cleanLine[i] == '+')) {
            numStr += cleanLine[i];
            i++;
        }

        float value = 0;
        if (!numStr.empty()) {
            try { value = std::stof(numStr); }
            catch (...) { value = 0; }
        }

        // For G and M codes, allow multiple per line (G0 X10 G1 Y20 is unusual but possible)
        // We store the last occurrence of each letter
        ws.words[letter] = value;
    }

    return ws;
}

void GCodeParser::processWords(const WordSet& words, int lineNumber, GCodeToolpath& toolpath) {
    // Handle G-codes (mode changes)
    if (words.has('G')) {
        int gCode = (int)words.get('G');
        switch (gCode) {
            case 0:  m_motionMode = 0;  break;  // Rapid
            case 1:  m_motionMode = 1;  break;  // Linear feed
            case 2:  m_motionMode = 2;  break;  // CW arc
            case 3:  m_motionMode = 3;  break;  // CCW arc
            case 17: break;  // XY plane (only supported plane)
            case 18: break;  // XZ plane (parsed but not implemented)
            case 19: break;  // YZ plane (parsed but not implemented)
            case 20: m_unitMode = 20;   break;  // Inches
            case 21: m_unitMode = 21;   break;  // Millimeters
            case 28: {
                // Home: move to origin
                GCodeMove move;
                move.type = MoveType::Rapid;
                move.start = m_currentPos;
                move.end = glm::vec3(0);
                move.feedRate = 0;
                move.sourceLineNumber = lineNumber;
                if (glm::length(move.end - move.start) > 0.0001f) {
                    toolpath.moves.push_back(move);
                }
                m_currentPos = glm::vec3(0);
                return;
            }
            case 90: m_distanceMode = 90; break; // Absolute
            case 91: m_distanceMode = 91; break; // Incremental
            case 92: {
                // Set position (coordinate system offset)
                if (words.has('X')) m_currentPos.x = words.get('X');
                if (words.has('Y')) m_currentPos.y = words.get('Y');
                if (words.has('Z')) m_currentPos.z = words.get('Z');
                return;
            }
            default:
                // Unknown G-code, skip
                break;
        }
    }

    // Handle feed rate
    if (words.has('F')) {
        m_feedRate = words.get('F');
    }

    // Handle M-codes (we don't generate moves for these, just log them)
    // M0/M1=pause, M2/M30=end, M3-M5=spindle, M6=tool change, M7-M9=coolant
    if (words.has('M') && !words.has('X') && !words.has('Y') && !words.has('Z')) {
        return;
    }

    // Check if there's any motion command (X, Y, Z, I, J, K coordinates)
    bool hasMotion = words.has('X') || words.has('Y') || words.has('Z');
    bool hasArcParams = words.has('I') || words.has('J') || words.has('K') || words.has('R');

    if (!hasMotion && !hasArcParams) return;

    // Handle arcs
    if (m_motionMode == 2 || m_motionMode == 3) {
        processArc(m_motionMode == 2, words, lineNumber, toolpath);
        return;
    }

    // Linear moves (G0/G1)
    glm::vec3 target = m_currentPos;

    if (m_distanceMode == 90) {
        // Absolute
        if (words.has('X')) target.x = words.get('X');
        if (words.has('Y')) target.y = words.get('Y');
        if (words.has('Z')) target.z = words.get('Z');
    } else {
        // Incremental
        if (words.has('X')) target.x += words.get('X');
        if (words.has('Y')) target.y += words.get('Y');
        if (words.has('Z')) target.z += words.get('Z');
    }

    // Unit conversion
    if (m_unitMode == 20) {
        // Convert inches to mm for internal representation
        if (words.has('X')) target.x *= 25.4f;
        if (words.has('Y')) target.y *= 25.4f;
        if (words.has('Z')) target.z *= 25.4f;
    }

    // Only add move if there's actual movement
    if (glm::length(target - m_currentPos) < 0.00001f) return;

    GCodeMove move;
    move.type = (m_motionMode == 0) ? MoveType::Rapid : MoveType::Linear;
    move.start = m_currentPos;
    move.end = target;
    move.feedRate = m_feedRate;
    move.sourceLineNumber = lineNumber;

    toolpath.moves.push_back(move);
    m_currentPos = target;
}

void GCodeParser::processArc(bool clockwise, const WordSet& words, int lineNumber, GCodeToolpath& toolpath) {
    glm::vec3 target = m_currentPos;

    if (m_distanceMode == 90) {
        if (words.has('X')) target.x = words.get('X');
        if (words.has('Y')) target.y = words.get('Y');
        if (words.has('Z')) target.z = words.get('Z');
    } else {
        if (words.has('X')) target.x += words.get('X');
        if (words.has('Y')) target.y += words.get('Y');
        if (words.has('Z')) target.z += words.get('Z');
    }

    // Unit conversion for target
    if (m_unitMode == 20) {
        if (words.has('X')) target.x *= 25.4f;
        if (words.has('Y')) target.y *= 25.4f;
        if (words.has('Z')) target.z *= 25.4f;
    }

    glm::vec3 arcCenterAbs;

    if (words.has('R')) {
        // Radius format: compute center from R
        float R = words.get('R');
        if (m_unitMode == 20) R *= 25.4f;

        glm::vec2 p1(m_currentPos.x, m_currentPos.y);
        glm::vec2 p2(target.x, target.y);
        glm::vec2 mid = (p1 + p2) * 0.5f;
        glm::vec2 diff = p2 - p1;
        float dist = glm::length(diff);

        if (dist < 0.0001f || std::abs(R) < dist * 0.5f) {
            toolpath.errors.push_back("Line " + std::to_string(lineNumber + 1) + ": Invalid arc radius");
            m_currentPos = target;
            return;
        }

        float h = std::sqrt(R * R - (dist * 0.5f) * (dist * 0.5f));
        glm::vec2 perp(-diff.y / dist, diff.x / dist);

        // Sign of R determines which center to use
        if ((clockwise && R > 0) || (!clockwise && R < 0)) {
            arcCenterAbs = glm::vec3(mid - perp * h, m_currentPos.z);
        } else {
            arcCenterAbs = glm::vec3(mid + perp * h, m_currentPos.z);
        }
    } else {
        // I, J, K format (offsets from start point -- always incremental in standard G-code)
        float I = words.get('I', 0);
        float J = words.get('J', 0);
        float K = words.get('K', 0);

        if (m_unitMode == 20) { I *= 25.4f; J *= 25.4f; K *= 25.4f; }

        arcCenterAbs = m_currentPos + glm::vec3(I, J, K);
    }

    GCodeMove move;
    move.type = clockwise ? MoveType::ArcCW : MoveType::ArcCCW;
    move.start = m_currentPos;
    move.end = target;
    move.arcCenter = arcCenterAbs;
    move.feedRate = m_feedRate;
    move.sourceLineNumber = lineNumber;

    toolpath.moves.push_back(move);
    m_currentPos = target;
}
