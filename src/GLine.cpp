//
//  GLine.cpp
//
//  Created by Andrew Wallace on 2/16/20.
//

#include "GLine.hpp"

void GLine::set(ofVec2f _a, ofVec2f _b){
    set(_a.x, _a.y, _b.x, _b.y);
}

void GLine::set(GLine other){
    set(other.a, other.b);
}

void GLine::set(float x1, float y1, float x2, float y2){
    a.x = x1;
    a.y = y1;
    b.x = x2;
    b.y = y2;
}

//this does not change the current line, but returns a new one
GLine GLine::get_offset(ofVec2f offset){
    GLine line;
    line.set(a,b);
    line.a += offset;
    line.b += offset;
    return line;
}

void GLine::draw(){
    ofDrawLine(a,b);
}
