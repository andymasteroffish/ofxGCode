//
//  GLine.cpp
//
//  Created by Andrew Wallace on 2/16/20.
//

#include "GLine.hpp"

GLine::GLine(){
    a.x = 0;
    a.y = 0;
    b.x = 0;
    b.y = 0;
    skip_me = false;
}

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

vector<GLine> GLine::get_segments(int num_segments){
    vector<GLine> lines;
    ofVec2f prev_pos = ofVec2f(a);
    for (int i=1; i<=num_segments; i++){
        float prc = (float)i/(float)num_segments;
        ofVec2f pos = (1.0-prc)*a + prc*b;
        GLine line;
        line.set(prev_pos, pos );
        lines.push_back(line);
        prev_pos.set(pos);
    }
    return lines;
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

bool GLine::clip_to_other_line(GLine other){
    return clip_to_other_line(other.a.x, other.a.y, other.b.x, other.b.y);
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

//trims any part of the line not in the polygon
//returns true if the line shoudl be removed entirely
//THIS WILL FAIL IF A AND B ARE OUTSIDE THE POLYGON BUT PART OF THE LINE IS INSIDE
//THIS WILL PARTIALLY FAIL IF THE LINE PASSES THROUGH THE POLYGON IN TWO POINTS
bool GLine::clip_inside_polygon(vector<ofVec2f> pnts){
    bool a_in, b_in;
    
    //if both points of this line are outside of the polygon, just remove it
    a_in = checkInPolygon(pnts, a.x, a.y);
    b_in = checkInPolygon(pnts, b.x, b.y);
    
    if (!a_in && !b_in){
        skip_me = true;
        return true;
    }
    
    //make sure A is on the inside
    if (b_in){
        swap_a_and_b();
    }
    
    //check if we are intersecting any of these lines
    for (int i=0; i<pnts.size(); i++){
        GLine other;
        other.set(pnts[i], pnts[(i+1)%pnts.size()]);
        clip_to_other_line(other);
    }
    
    return false;
}

//trims any part of the line inside the polygon
//returns true if the line shoudl be removed entirely
//THIS WILL FAIL IF A AND B ARE OUTSIDE THE POLYGON BUT PART OF THE LINE IS INSIDE
//THIS WILL PARTIALLY FAIL IF THE LINE PASSES THROUGH THE POLYGON IN TWO POINTS
bool GLine::clip_outside_polygon(vector<ofVec2f> pnts){
    bool a_in, b_in;
    
    //if both points of this line are outside of the polygon, just remove it
    a_in = checkInPolygon(pnts, a.x, a.y);
    b_in = checkInPolygon(pnts, b.x, b.y);
    
    if (a_in && b_in){
        skip_me = true;
        return true;
    }
    
    //make sure A is on the outside
    if (a_in){
        swap_a_and_b();
    }
    
    //check if we are intersecting any of these lines
    for (int i=0; i<pnts.size(); i++){
        GLine other;
        other.set(pnts[i], pnts[(i+1)%pnts.size()]);
        clip_to_other_line(other);
    }
    
    return false;
}




bool GLine::checkInPolygon(vector<ofVec2f> p, float x, float y)
{
    int i, j, c = 0;
    for (i = 0, j = p.size()-1; i < p.size(); j = i++) {
        if ((((p[i].y <= y) && (y < p[j].y)) ||
             ((p[j].y <= y) && (y < p[i].y))) &&
            (x < (p[j].x - p[i].x) * (y - p[i].y) / (p[j].y - p[i].y) + p[i].x))
            c = !c;
    }
    return c;
}
