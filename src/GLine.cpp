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
    do_not_reverse = false;
}

GLine::GLine(ofVec2f _a, ofVec2f _b){
    set(_a, _b);
    skip_me = false;
    do_not_reverse = false;
}

GLine::GLine(float x1, float y1, float x2, float y2){
    set(x1,y1, x2,y2);
    skip_me = false;
    do_not_reverse = false;
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
    
    //if this line has no distance, draw a little circle to mark it
    if (a == b){
        ofDrawCircle(a.x, a.y, 0.5);
    }
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

//returns true if line intersects
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

//returns true if line intersects, and writes the intersect point to the passed ofVec2f
bool GLine::intersects(GLine other, ofVec2f &intersect_pnt){
    ofPoint out;        //ofLineSegmentIntersection takes ofPoint, but I don't like 'em
    ofPoint my_a = a;
    ofPoint my_b = b;
    ofPoint other_a = other.a;
    ofPoint other_b = other.b;
    if (ofLineSegmentIntersection(my_a, my_b, other_a, other_b, out)){
        intersect_pnt.x = out.x;
        intersect_pnt.y = out.y;
        return true;
    }
    return false;
}



//this always assumes A is the side that will remain
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
bool GLine::clip_to_other_line(GLine other){
    return clip_to_other_line(other.a.x, other.a.y, other.b.x, other.b.y);
}
bool GLine::clip_to_other_line(ofVec2f other_a, ofVec2f other_b){
    return clip_to_other_line(other_a.x, other_a.y, other_b.x, other_b.y);
}

void GLine::swap_a_and_b(){
    ofVec2f temp = ofVec2f(a);
    a.set(b);
    b.set(temp);
}

//removes all parts of this line inside the polygon
//if a list is provided, any new lines that need to be created will be added there
void GLine::trim_inside_polygon(vector<ofVec2f> pnts, vector<GLine>* list){
   return trim_flexible_polygon(pnts, true, list);
}

//removes all parts of this line outside the polygon
//if a list is provided, any new lines that need to be created will be added there
void GLine::trim_outside_polygon(vector<ofVec2f> pnts, vector<GLine>* list){
    trim_flexible_polygon(pnts, false, list);
}
void GLine::trim_outside_rect(ofRectangle rect, vector<GLine>* list){
    vector<ofVec2f> pnts;
    pnts.push_back(ofVec2f(rect.x, rect.y));
    pnts.push_back(ofVec2f(rect.x+rect.width, rect.y));
    pnts.push_back(ofVec2f(rect.x+rect.width, rect.y+rect.height));
    pnts.push_back(ofVec2f(rect.x, rect.y+rect.height));
    trim_outside_polygon(pnts, list);
}

//this will trim inside or outside depending on what is passed in
void GLine::trim_flexible_polygon(vector<ofVec2f> pnts, bool trim_inside, vector<GLine>* list){
    bool a_in = checkInPolygon(pnts, a.x, a.y);
    bool b_in = checkInPolygon(pnts, b.x, b.y);
    
    vector<ofVec2f> intersection_pnts, intersection_pnts_unordered;
    
    //if a is outside the zone that will be trimmed, treat it as the first intersection
    if ( (!a_in && trim_inside) || (a_in && !trim_inside) ){
        intersection_pnts.push_back(a);
    }
    
    //find all intersections with the shape
    for (int i=0; i<pnts.size(); i++){
        GLine other;
        other.set(pnts[i], pnts[(i+1)%pnts.size()]);
        
        //does it intersect?
        ofVec2f intersect_pnt;
        if (intersects(other, intersect_pnt)){
            intersection_pnts_unordered.push_back(intersect_pnt);
        }
    }
    
    //put the unordered points in order based on dist to A
    while (intersection_pnts_unordered.size() > 0){
        int best_id = 0;
        float close_dist_sq = ofDistSquared(intersection_pnts_unordered[0].x, intersection_pnts_unordered[0].y, a.x, a.y);
        for (int i=1; i<intersection_pnts_unordered.size(); i++){
            float dist_sq = ofDistSquared(intersection_pnts_unordered[i].x, intersection_pnts_unordered[i].y, a.x, a.y);
            if (dist_sq < close_dist_sq){
                best_id = i;
                close_dist_sq = dist_sq;
            }
        }
        intersection_pnts.push_back(intersection_pnts_unordered[best_id]);
        intersection_pnts_unordered.erase(intersection_pnts_unordered.begin()+best_id);
    }
    
    //if b is outside the zone that will be trimmed, treat it as the last intersection
    if ( (!b_in && trim_inside) ||  (b_in && !trim_inside)){
        intersection_pnts.push_back(b);
    }
    
    //if we have an odd number, something bad happened
    if (intersection_pnts.size() % 2 == 1){
        cout<<"SOMETHING BAD HAPPENED. intersections: "<<intersection_pnts.size()<<endl;
        for (int i=0; i<intersection_pnts.size(); i++){
            cout<<"  "<<intersection_pnts[i]<<endl;
        }
        
        //this is a hack and probably bad, but for now I'm just nixing the first point because the 1st and second often seem really close for some reason
        intersection_pnts.erase(intersection_pnts.begin());
    }
    
    //if we have no intersects, this line is entirely in the polygon and should be killed
    //we should never have exactly 1, but if we do, somehting went wrong and we should just remove this line
    if (intersection_pnts.size() <= 1){
        skip_me = true;
        return;
    }
    
    //go through and trim this line to be from the 1st to second intersection
    //after that, put additional lines in the vector if one was provided
    for (int i=0; i<intersection_pnts.size(); i+=2){
        if (i==0){
            a = intersection_pnts[i];
            b = intersection_pnts[i+1];
        }
        else if (list != NULL){
            GLine new_line;
            new_line.set(intersection_pnts[i], intersection_pnts[i+1]);
            list->push_back(new_line);
        }
    }
    
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



//These functions have been depricated
bool GLine::clip_inside_rect(ofRectangle rect){
    cout<<"clip_inside_rect HAS BEEN REMOVED use trim_outside_rect instead!"<<endl;
    return false;
}
bool GLine::clip_inside_polygon(vector<ofVec2f> pnts){
    cout<<"clip_inside_polygon HAS BEEN REMOVED use trim_outside_polygon instead!"<<endl;
    return false;
}
bool GLine::clip_outside_polygon(vector<ofVec2f> pnts){
    cout<<"clip_outside_polygon HAS BEEN REMOVED use trim_inside_polygon instead!"<<endl;
    return false;
}
