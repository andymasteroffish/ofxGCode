//
//  GLine.hpp
//
//  Created by Andrew Wallace on 2/16/20.
//

#ifndef GLine_hpp
#define GLine_hpp

#include "ofMain.h"

//helper class for dealing with lines

class GLine{
public:
    
    void set(ofVec2f _a, ofVec2f _b);
    void set(float x1, float y1, float x2, float y2);
    void draw();
    
    ofVec2f a, b;
    
};

#endif /* GLine_hpp */
