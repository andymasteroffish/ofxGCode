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

bool GLine::intersects(GLine other){
    ofPoint out;
    ofPoint my_a = a;
    ofPoint my_b = b;
    ofPoint other_a = other.a;
    ofPoint other_b = other.b;
    if (ofLineSegmentIntersection(my_a, my_b, other_a, other_b, out)){
        return true;
    }
    return false;
}
    
bool GLine::clip_to_other_line(ofVec2f other_a, ofVec2f other_b){
    return clip_to_other_line(other_a.x, other_a.y, other_b.x, other_b.y);
}

//this always assumes A is standing still
bool GLine::clip_to_other_line(float other_a_x, float other_a_y, float other_b_x, float other_b_y){
    ofPoint out;
    ofPoint my_a = a;
    ofPoint my_b = b;
    ofPoint other_a = ofPoint(other_a_x, other_a_y);
    ofPoint other_b = ofPoint(other_b_x, other_b_y);
    if (ofLineSegmentIntersection(my_a, my_b, other_a, other_b, out)){
        b = out;
        return true;
    }
    return false;
}

void GLine::swap_a_and_b(){
    ofVec2f temp = ofVec2f(a);
    a.set(b);
    b.set(temp);
}
