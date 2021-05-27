#pragma once

#include "ofMain.h"
#include "ofxGCode.hpp"

class ofApp : public ofBaseApp{

public:
    void setup();
    void refresh();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //gcode drawing
    ofxGCode gcode;
    
    //shapes to use to trim lines
    ofRectangle box;
    vector<ofVec2f> polygon;
    
    //changing how we want to trim
    int trim_mode;
    
		
};
