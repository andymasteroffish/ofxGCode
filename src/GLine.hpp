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
    void set(GLine other);
    void set(float x1, float y1, float x2, float y2);
    GLine get_offset(ofVec2f offset);
    void draw();
    
    bool intersects(GLine other);
    bool clip_to_other_line(ofVec2f other_a, ofVec2f other_b);
    bool clip_to_other_line(float other_a_x, float other_a_y, float other_b_x, float other_b_y);
    
    void swap_a_and_b();
    
    ofVec2f a, b;
    
};

#endif /* GLine_hpp */
