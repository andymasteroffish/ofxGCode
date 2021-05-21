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

// This comes from https://github.com/tobiaszimmer/ofxHersheyFont
// I am modifying it for this project

#include "ofMain.h"
#include "ofxGCode.hpp"

class ofxHersheyFont {
	public:
		ofxHersheyFont();

        float draw(string stringValue, float x, float y, float scale, ofxGCode * gcode, float width);
		void draw(string stringValue, float x, float y, float scale, ofxGCode * gcode);
		void draw(string stringValue, float x, float y, float scale, bool centered, ofxGCode * gcode);
		void draw(string stringValue, float x, float y, float scale, bool centered, float angle, ofxGCode * gcode);
		void setColor(ofColor c);
		float getWidth(string stringValue, float scale);
        float getHeight(string stringValue, float scale);
		float getHeight(float scale);
        float getLineHeight(float scale);

		ofColor color;
    
    int line_height;
    
	private:
		void drawChar(int asciiValue, ofxGCode * gcode);
    

};


