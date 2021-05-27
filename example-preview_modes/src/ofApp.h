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
    
    ofxGCode gcode;
    
    //values that will be used when redrawing
    bool sort_lines;
    bool set_do_not_reverse_on_edges;
		
    //flag to tell the user when they need to refresh the drawing
    bool need_refresh;
};
