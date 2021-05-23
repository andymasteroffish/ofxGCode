/*
*  ofxHersheyFont.h
*
*  Created by Tobias Zimmer, August 2016.
*  www.tobiaszimmer.net
*
*  Font originally developed by Dr. Allen V. Hershey in 1967.
*  Font vectors made available by Paul Bourke.
*  paulbourke.net/dataformats/hershey/
*
*  A simple single line font for machines like CNC, Lasercutter, ...
*  Available characters: ASCII codes 32 to 126.
*
*/

// This is a heavily modified verison of ofxHersheyFont by Tobias Zimmer.
// you can find the original here: https://github.com/tobiaszimmer/ofxHersheyFont

// This class is not included by default in ofxGCode. You will need an extra #include statement for it.

#include "ofMain.h"
#include "ofxGCode.hpp"

class ofxHersheyFont {
    
public:
    
    ///constructor
    ofxHersheyFont();

    ///draws a string into the provided ofxGCode. If a width value is provided, the text will wrap to the next line if it would exceed that width
    ///the total height of the drawn text is returned
    float draw(string stringValue, float x, float y, float scale, ofxGCode * gcode, float width);
    
    ///draws a string into the provided ofxGCode
    void draw(string stringValue, float x, float y, float scale, ofxGCode * gcode);
    
    ///draws a string into the provided ofxGCode with an option to center the text
    void draw(string stringValue, float x, float y, float scale, bool centered, ofxGCode * gcode);
    
    ///draws a string into the provided ofxGCode with an option to center the text and to rotate it
    void draw(string stringValue, float x, float y, float scale, bool centered, float angle, ofxGCode * gcode);
    
    ///returns the total width to draw a string at the given scale
    float getWidth(string stringValue, float scale);
    
    ///returns the total height to draw a string at the given scale. Newline characters are considered
    float getHeight(string stringValue, float scale);
    
    ///returns the height of a capital letter at the given scale
    float getCapitalHeight(float scale);
    
    ///returns the line height at the given scale
    float getLineHeight(float scale);

    
    //at a scale of 1, the height of a capital is 21
    //line_height is that value plus enough white space for the next line
    int line_height;
    
private:
    
    ///draws an individual character into the ofxGCode. This is used in the string drawing functions
    void drawChar(int asciiValue, ofxGCode * gcode);
    

};


