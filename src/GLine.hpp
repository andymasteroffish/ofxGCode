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
    
    GLine();
    GLine(ofVec2f _a, ofVec2f _b);
    void set(ofVec2f _a, ofVec2f _b);
    void set(GLine other);
    void set(float x1, float y1, float x2, float y2);
    GLine get_offset(ofVec2f offset);
    void draw();
    
    vector<GLine> get_segments(int num_segments);
    
    bool intersects(GLine other);
    bool clip_to_other_line(GLine other);
    bool clip_to_other_line(ofVec2f other_a, ofVec2f other_b);
    bool clip_to_other_line(float other_a_x, float other_a_y, float other_b_x, float other_b_y);
    
    void swap_a_and_b();
    
    bool clip_inside_rect(ofRectangle rect);
    bool clip_inside_polygon(vector<ofVec2f> pnts);
    bool clip_outside_polygon(vector<ofVec2f> pnts);
    
    bool checkInPolygon(vector<ofVec2f> p, float x, float y);
    
    ofVec2f a, b;
    bool skip_me;
    bool do_not_reverse;
    
};

#endif /* GLine_hpp */
