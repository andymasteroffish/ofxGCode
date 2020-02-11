//
//  Clipping.hpp
//
//  Created by Andrew Wallace on 4/8/19. Based on code by Trammel Hudson.
//

// This code is entirely based on the Processing VST library by Trammell Hudson
// https://github.com/osresearch/vst

#ifndef Clipping_hpp
#define Clipping_hpp

#include "ofMain.h"

class Clipping{
public:

    ofVec3f min;
    ofVec3f max;

    const static int INSIDE = 0;
    const static int LEFT = 1;
    const static int RIGHT = 2;
    const static int BOTTOM = 4;
    const static int TOP = 8;
    
    void setup(ofVec2f p0, ofVec2f p1);
    int compute_code(ofVec2f p);
    float intercept(float y, float x0, float y0, float x1, float y1);
    bool clip(ofVec2f &p0, ofVec2f &p1);
    bool check_point(ofVec2f pnt);
};

#endif /* Clipping_hpp */
