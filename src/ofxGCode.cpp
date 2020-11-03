//
//  ofxGCode
//
//  Created by Andrew Wallace on 11/7/19.
//

#include "ofxGCode.hpp"

void ofxGCode::setup(float _pixels_per_inch){
    //set some defaults
    circle_resolution = 50;
    pen_down_value = 60;
    
    //inches for axidraw
    pixels_per_inch = _pixels_per_inch;
    
    show_transit_lines = false;
    show_path_with_color = false;
    show_do_not_reverse = false;
    do_not_draw_dots = false;
    
    //last_translate_id = 0;
    
    max_dist_to_consider_pnts_touching = 0.001;
    
    set_size(ofGetWidth(), ofGetHeight());
    
    debug_show_point_numbers = false;
    
    demo_col.set(0,0,0);
}

void ofxGCode::set_size(int w, int h){
    clip.setup(ofVec2f(0, 0), ofVec2f(w,h));
    
    clear();
}

void ofxGCode::clear(){
    lines.clear();
}

void ofxGCode::draw(int max_lines_to_show){
    
    int draw_count = 0;
    
    int end_index = MIN(max_lines_to_show, lines.size());
    for (int i=0; i<end_index; i++){
        //the line
        GLine line = lines[i];
        
        ofSetColor(demo_col.r, demo_col.g, demo_col.b, 255 * 0.75);
        
        //fading between colors to show order
        if (show_path_with_color){
            float prc = (float)i/(float)lines.size();
            ofSetColor(0, 255.0*(1.0-prc), 255*prc);
        }
        
        if (show_do_not_reverse && line.do_not_reverse){
            ofSetColor(255, 38, 226);
            //throw wings on it
            float prc = 0.9;
            ofVec2f wing_pnt;
            wing_pnt.x = (1.0-prc)*line.a.x + prc*line.b.x;
            wing_pnt.y = (1.0-prc)*line.a.y + prc*line.b.y;
            //ofDrawCircle(wing_pnt.x, wing_pnt.y, 2);
            float angle = atan2(line.a.y-line.b.y, line.a.x-line.b.x);
            float dist = 7;
            float spread = PI/8;
            ofDrawLine(wing_pnt.x, wing_pnt.y, wing_pnt.x+cos(angle+spread)*dist, wing_pnt.y+sin(angle+spread)*dist);
            ofDrawLine(wing_pnt.x, wing_pnt.y, wing_pnt.x+cos(angle-spread)*dist, wing_pnt.y+sin(angle-spread)*dist);
        }
        
        line.draw();
    
        
        //the transit to the next line
        if (i < end_index-1 && show_transit_lines){
            ofVec2f transit_start = line.b;
            ofVec2f transit_end = lines[i+1].a;
            if (transit_start != transit_end){
                ofSetColor(255, 0,0, 60);
                ofDrawLine(transit_start, transit_end);
            }
//            else{
//                cout<<"skip transit at "<<transit_start<<endl;
//            }
        }
    }
    
    /*
    if (list.size() > 1){
        float prev_x = list[0].x;
        float prev_y = list[0].y;
        float prev_speed = list[0].speed;
        float prev_pressure = list[0].pressure;
        for (int i=0; i<list.size(); i++){
            if (draw_count > max_lines_to_show && max_lines_to_show > 0){
                return;
            }
            GCodePoint pnt = list[i];
            if (pnt.pressure == 0){
                draw_count++;
                if (show_transit_lines){
                    ofSetColor(255, 0,0, 60);
                }else{
                    ofSetColor(255, 0,0, 0);
                }
                
            }else{
                float speed_prc = ofMap(speed, 1000, max_speed, 1, 0);  //setting an arbitrary min
                speed_prc = powf(speed_prc, 0.5);                       //smoothing this out since a medium speed still looks pretty black
                ofSetColor(demo_col.r, demo_col.g, demo_col.b, speed_prc * 255);
                
                //fading between colors to show order
                if (show_path_with_color){
                    float prc = (float)i/(float)list.size();
                    ofSetColor(0, 255.0*(1.0-prc), 255*prc);
                }
                
                if (show_do_not_reverse && pnt.do_not_reverse){
                    ofSetColor(255, 38, 226);
                    if (list[i-1].pressure == 0){
                        //throw wings on it
                        float prc = 0.9;
                        ofVec2f wing_pnt;
                        wing_pnt.x = (1.0-prc)*prev_x + prc*pnt.x;
                        wing_pnt.y = (1.0-prc)*prev_y + prc*pnt.y;
                        //ofDrawCircle(wing_pnt.x, wing_pnt.y, 2);
                        float angle = atan2(prev_y-pnt.y, prev_x-pnt.x);
                        float dist = 7;
                        float spread = PI/8;
                        ofDrawLine(wing_pnt.x, wing_pnt.y, wing_pnt.x+cos(angle+spread)*dist, wing_pnt.y+sin(angle+spread)*dist);
                        ofDrawLine(wing_pnt.x, wing_pnt.y, wing_pnt.x+cos(angle-spread)*dist, wing_pnt.y+sin(angle-spread)*dist);
                    }
                }
            }
            
            //won't be able to see dots if we're drawing lines
            //THIS IS NOT REALLY Working right now
            bool is_dot = false;// prev_x == pnt.x && prev_y == pnt.y && pnt.pressure > 0;
            
            if (do_not_draw_dots)   is_dot = false;
            
            if (!is_dot){
                ofDrawLine(prev_x, prev_y, pnt.x, pnt.y);
            }else{
                ofDrawCircle(pnt.x, pnt.y, 2);
            }
            
            //testing
            if (debug_show_point_numbers){
                 if (pnt.pressure == 0){
                     ofSetColor(255,0,0);
                 }else{
                     ofSetColor(0);
                 }
                //ofDrawBitmapString(ofToString(i), (pnt.x+prev_x)/2.0, (pnt.y+prev_y)/2.0);
                ofDrawBitmapString(ofToString(i), pnt.x, pnt.y-((float)i*2));// - i*10);
            }
            
            prev_x = pnt.x;
            prev_y = pnt.y;
            prev_speed = pnt.speed;
            prev_pressure = pnt.pressure;
        }
    }
     */
}


void ofxGCode::save(string name){
    float inches_per_pixel = 1.0 / pixels_per_inch;
    
    vector<string> commands;
    commands.clear();
    
    //pen up and positioned at the origin
    commands.push_back("M3 S0");
    commands.push_back("G0 X0 Y0");
    
    ofVec2f last_pos = ofVec2f(0,0);
    
    for(int i=0; i<lines.size(); i++){
        GLine line = lines[i];
        ofVec2f pos_a = ofVec2f(line.a.x * inches_per_pixel, line.a.y * inches_per_pixel);
        ofVec2f pos_b = ofVec2f(line.b.x * inches_per_pixel, line.b.y * inches_per_pixel);
        
        //if we are not at the start point, pen up and move there and pen down
        if ( pos_a != last_pos){
            commands.push_back("M3 S0");
            commands.push_back("G0 X"+ofToString(pos_a.x)+" Y"+ofToString(pos_a.y));
            commands.push_back("M3 S"+ofToString(pen_down_value));
        }
        else{
            //cout<<"do not life pen at "<<line.a<<endl;
        }
        
        //move to the end point
        commands.push_back("G1 X"+ofToString(pos_b.x)+" Y"+ofToString(pos_b.y));
        
        //store it
        last_pos = ofVec2f (pos_b);
    }
    
    //add some closing steps
    commands.push_back("M3 S0");
    commands.push_back("G0 X0 Y0");
    
    cout<<"transit distance: "<<measureTransitDistance()<<endl;
    
    //write it to file
    
    cout<<"saving "<<commands.size()<<" commands"<<endl;
    ofFile myTextFile;
    myTextFile.open(name,ofFile::WriteOnly);
    for (int i=0; i<commands.size(); i++){
        myTextFile<<commands[i]<<endl;
    }
    
    cout<<"SAVED"<<endl;
}


void ofxGCode::rect(ofRectangle box){
    rect(box.x, box.y, box.width, box.height);
}
void ofxGCode::rect(float x, float y, float w, float h){
    line(x,y, x+w, y, true);
    line(x+w,y, x+w, y+h, false);
    line(x+w,y+h, x, y+h, false);
    line(x,y+h, x, y, false);
}

void ofxGCode::circle(float x, float y, float size){
    float angle_step =(TWO_PI/(float)circle_resolution);
    begin_shape();
    for (int i=0; i<circle_resolution; i++){
        ofVec2f pnt;
        float angle = angle_step  * i;
        
        pnt.x = x + sin(angle) * size;
        pnt.y = y + cos(angle) * size;
        vertex(pnt.x, pnt.y);
    }
    end_shape(true);
}

//Emulating the begin/end shape functionality
void ofxGCode::begin_shape(){
    shape_pnts.clear();
}
void ofxGCode::vertex(ofVec2f p){
    shape_pnts.push_back(p);
}
void ofxGCode::vertex(float x, float y){
    shape_pnts.push_back(ofVec2f(x,y));
}
void ofxGCode::end_shape(bool close){
    if (shape_pnts.size() < 2){
        //cout<<"not enough points to make a shape"<<endl;
        return;
    }
    for (int i=0; i<shape_pnts.size()-1; i++){
        line(shape_pnts[i].x, shape_pnts[i].y, shape_pnts[i+1].x, shape_pnts[i+1].y, i==0);
    }
    if (close){
        line(shape_pnts[shape_pnts.size()-1].x, shape_pnts[shape_pnts.size()-1].y, shape_pnts[0].x, shape_pnts[0].y, false);
    }
}

//drawing polygone from points
void ofxGCode::polygon(vector<ofVec2f> pnts){
    begin_shape();
    for (int i=0; i<pnts.size(); i++){
        vertex(pnts[i]);
    }
    end_shape(true);
}

//Lines
void ofxGCode::line(GLine _line){
    if (_line.skip_me)  return;
    line(_line.a.x,_line.a.y, _line.b.x,_line.b.y);
}
void ofxGCode::line(ofVec2f a, ofVec2f b, bool lift_pen){
    line(a.x, a.y, b.x, b.y);
}

void ofxGCode::line(float x1, float y1, float x2, float y2){
    line(x1,y1, x2,y2, true);
}

void ofxGCode::line(float x1, float y1, float x2, float y2, bool lift_pen){
    ofVec2f p1 = getModelPoint(x1,y1);
    ofVec2f p2 = getModelPoint(x2,y2);
    
    //clip the points to fit our canvas, rejecting the line if it would be entirely out of bounds
    if (!clip.clip(p1, p2)) {
        cout<<"no part of this line is on screen"<<endl;
        return;
    }

    
    GLine line;
    line.set(p1, p2);
    lines.push_back(line);
    
    //KILL ME
//    if (!clip.clip(p1, p2)) {
//        //cout<<"no part of this line is on screen"<<endl;
//        return;
//    }
//
//    if (lift_pen){
//        point(p1.x, p1.y, speed, 0);
//    }else{
//        point(p1.x, p1.y, speed, pressure);
//    }
//    point(p2.x, p2.y, speed, pressure);
}

//Thick lines are just multiple lines, eenly spaced
void ofxGCode::thick_line(float x1, float y1, float x2, float y2, float spacing, int layers){
    thick_line(ofVec2f(x1,y1), ofVec2f(x2,y2), spacing, layers);
}

void ofxGCode::thick_line(ofVec2f base_a, ofVec2f base_b, float spacing, int layers){
    //get the angle of the line
    float angle = atan2(base_a.y-base_b.y, base_a.x-base_b.x);
    float tan_angle = angle + PI/2;
    
    //draw it
    float dist_offset = spacing * (layers-1) * 0.5;
    
    
    for (int t=0; t<layers; t++){
        float dist = t * spacing - dist_offset;
        //cout<<"my dist "<<dist<<endl;
        ofVec2f a = ofVec2f(base_a);
        ofVec2f b = ofVec2f(base_b);
        a.x += cos(tan_angle) * dist;
        a.y += sin(tan_angle) * dist;
        b.x += cos(tan_angle) * dist;
        b.y += sin(tan_angle) * dist;
        line(a,b);
    }
}

//Bezier Curves
void ofxGCode::bezier(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, int steps){
    
    begin_shape();
    
    for (int i=0; i<=steps; i++){
        ofPoint pnt = ofBezierPoint(p1, c1, c2, p2, (float)i/(float)steps);
        vertex(pnt);
    }
    
    end_shape(false);
    
}

void ofxGCode::dot(float x, float y){
    //MAKE THIS WORK
    
    /*
    ofVec2f pnt = getModelPoint(x,y);
    
    if (clip.check_point(pnt) == false){
        return;
    }
    
    //transit line
    point(x, y, speed, 0);
    //drop the pen
    point(x, y, speed, pressure);
     */
    
}

//void ofxGCode::point(float x, float y, float speed, float pressure){
//    
//    if (x == last_x && y == last_y  ) {
//        //cout<<"same point as last. ignored. "<<x<<","<<y<<endl;
//        return;
//    }
//    last_x = x;
//    last_y = y;
//    
//    //cout<<"adding at "<<x<<","<<y<<endl;
//    
//    GCodePoint pnt = GCodePoint(x,y,speed,pressure);
//    list.push_back(pnt);
//}

//https://openframeworks.cc/documentation/graphics/ofTrueTypeFont/#show_getStringAsPoints
void ofxGCode::text(string text, ofTrueTypeFont * font, float x, float y){
    bool vflip = true; // OF flips y coordinate in the default perspective,
    // should be false if using a camera for example
    bool filled = false; // or false for contours
    vector < ofPath > paths = font->getStringAsPoints(text, vflip, filled);
    
    ofPushMatrix();
    ofTranslate(x,y);
    
    for (int i = 0; i < paths.size(); i++){
        // for every character break it out to polylines
        vector <ofPolyline> polylines = paths[i].getOutline();
        
        // for every polyline, draw lines
        for (int j = 0; j < polylines.size(); j++){
            for (int k = 0; k < polylines[j].size(); k++){        
                int next_id = (k+1) % polylines[j].size();
                line(polylines[j][k].x,polylines[j][k].y, polylines[j][next_id].x,polylines[j][next_id].y, k==0);
            }
        }
    }
    
    ofPopMatrix();
}

//void ofxGCode::translate(float x, float y){
//    for (int i=0; i<list.size(); i++){
//        list[i].x += x;
//        list[i].y += y;
//    }
//
//    last_translate_id = list.size()-1;  //everything before this point has been moved at least once
//}

//This function is by Andy, it attempts to recreate the functionality of modelX() and modelY() in Processing
//Currently it only works in 2D. 3D transformations will break it.
//it sometimes gets locked at 90 degree angles when the actual angle is 89 or 91. Not sure why
//it could definitely be more efficient by using quaternions properly
ofVec2f ofxGCode::getModelPoint(ofVec3f pnt){
    return getModelPoint(pnt.x, pnt.y);
}
ofVec2f ofxGCode::getModelPoint(float x, float y){
    //get the model of th current matrix
    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    ofMatrix4x4 mat(m);
    
    //check if this model matches the baseline, no matrix model and avoid a lot of unecessary work if it does
    ofVec4f baseline[4];
    baseline[0] = ofVec4f(1,0,0,0);
    baseline[1] = ofVec4f(0,1,0,0);
    baseline[2] = ofVec4f(0,0,1,0);
    baseline[3] = ofVec4f(-ofGetWidth()/2,-ofGetHeight()/2,-1,1);   //the z value is the wildcard. I'm not sure how it is set
    
    bool matches = true;
    for (int i=0; i<4; i++){
        if (mat._mat[i].x != baseline[i].x) matches = false;
        if (mat._mat[i].y != baseline[i].y) matches = false;
        if (mat._mat[i].z != baseline[i].z && i!=3) matches = false;
        if (mat._mat[i].w != baseline[i].w) matches = false;
    }
    
    //if it all matches the baseline, we're not in a matrix and can just return the input values
    if (matches){
        //cout<<"nothing doing"<<endl;
        return ofVec2f(x,y);
    }
    
    //get the model of the default screen (this can't be used as the baseline above for reasons I don't totally understand)
    ofMatrix4x4 ident = ofMatrix4x4::newIdentityMatrix();
    
    //extract info from that
    ofVec3f trans_val = mat.getTranslation();
    ofVec3f scale_val = mat.getScale();
    ofQuaternion quat = mat.getRotate();
    ofVec3f rot_val = quat.asVec3();
    ofVec3f euler = quat.getEuler();
    
    
    ofPoint origPoint = ofPoint(x,y);
    ofVec2f scaled_point;
    scaled_point.x = origPoint.x * scale_val.x;
    scaled_point.y = origPoint.y * scale_val.y;
    
    float euler_deg = euler.z;
    if (euler.x != 0){  //the euler degreees start counting back from 90 and the x and y values of rot_val go to 180
        euler_deg = 90 + (90-euler_deg);
    }
    float mat_angle = ofDegToRad (euler_deg);
    float base_angle = atan2(scaled_point.x, scaled_point.y);   //shouldn't this be y then x??? Not sure why this works
    float angle = base_angle - mat_angle;
    
    float dist = scaled_point.length();
    
    ofVec2f return_val;
    return_val.x = sin(angle) * dist +  (ofGetWidth()/2 + trans_val.x);
    return_val.y = cos(angle) * dist + (ofGetHeight()/2 + trans_val.y);
    
    return return_val;
}

//this is not perfect yet. Some of the resulting order is deifnitely not as efficient as it could be
void ofxGCode::sort(){
    
    //try to break the lines into groups of continuous lines
    vector<GCodeLineGroup> line_groups;
    GCodeLineGroup cur_group;
    while (lines.size() > 0){
        //if cur group is empty just grab the next line segment
        if (cur_group.lines.size() == 0){
            cur_group.add_to_front(lines[0]);
            lines.erase(lines.begin());
        }
        
        //go through and find segments that we can slap on the front or end
        bool added_any = false;
        for (int i=lines.size()-1; i>=0; i--){
            if (lines[i].a == cur_group.end_pos){
                cur_group.add_to_back(lines[i]);
                lines.erase(lines.begin()+i);
                added_any = true;
            }
            else if (lines[i].b == cur_group.start_pos){
                cur_group.add_to_front(lines[i]);
                lines.erase(lines.begin()+i);
                added_any = true;
            }
        }
        
        //if we added any, keep going, otherwise bail
        if (!added_any){
            line_groups.push_back(cur_group);
            cur_group.clear();
        }
    }
    //when we're done, add the last group if there's anything in it
    if (cur_group.lines.size() > 0){
        line_groups.push_back(cur_group);
    }
    
    
    //go through finding the closest end point
    ofVec2f cur_pnt = ofVec2f();
    while (line_groups.size() > 0){
        int close_id;
        float close_dist_sq = 9999999;
        bool need_to_flip = false;
        
        //check each unsorted line group
        for (int i=0; i<line_groups.size(); i++){
            float dist_sq_a = ofDistSquared(line_groups[i].start_pos.x, line_groups[i].start_pos.y, cur_pnt.x, cur_pnt.y);
            
            //only get distance to B if it is OK to flip this line
            float dist_sq_b = 99999999;
            bool can_reverse = !line_groups[i].do_not_reverse;
            if (can_reverse){
                dist_sq_b = ofDistSquared(line_groups[i].end_pos.x, line_groups[i].end_pos.y, cur_pnt.x, cur_pnt.y);
            }
            
            //are either of these poitns the closest so far?
            if (dist_sq_b < close_dist_sq){
                close_dist_sq = dist_sq_b;
                need_to_flip = true;
                close_id = i;
            }
            if (dist_sq_a < close_dist_sq){
                close_dist_sq = dist_sq_a;
                need_to_flip = false;
                close_id = i;
            }
        }
        
        GCodeLineGroup group = line_groups[close_id];
        
        if (need_to_flip){
            for (int i=group.lines.size()-1; i>=0; i--){
                group.lines[i].swap_a_and_b();
                lines.push_back(group.lines[i]);
            }
            cur_pnt = group.start_pos;
        }else{
            for (int i=0; i<group.lines.size(); i++){
                lines.push_back(group.lines[i]);
            }
            cur_pnt = group.end_pos;
        }
        
        //remove it from unsorted
        line_groups.erase(line_groups.begin() + close_id);
    }
}

float ofxGCode::measureTransitDistance(){
    float distance = 0.0;
    
    for (int i=0; i<lines.size(); i++){
        distance += ofDist(lines[i].a.x, lines[i].a.y, lines[i].b.x, lines[i].b.y);
    }
    
    return distance;
}

//takes any vector of lines and returns a new vector where the spaces inside the polygon have been trimmed
vector<GLine> ofxGCode::trim_lines_inside_polygon(vector<GLine> lines, vector<ofVec2f> bounds){
    vector<GLine> output;
    
    //go through each line and try to trim it
    for (int i=0; i<lines.size(); i++){
        //trim it, adding any extra lines generated to the output
        lines[i].trim_inside_polygon(bounds, &output);
        //then if this line is still valid, add it as well
        if (!lines[i].skip_me){
            output.push_back(lines[i]);
        }
    }
    
    return output;
}

//trims the current list of lines, removing any points inside the given polygon
void ofxGCode::trim_inside_polygon(vector<ofVec2f> bounds){
    lines = trim_lines_inside_polygon(lines, bounds);
}

//helper function for trimming rectangles
vector<GLine> ofxGCode::trim_lines_inside_box(vector<GLine> lines, ofRectangle bounds){
    vector<ofVec2f> pnts;
    pnts.push_back(ofVec2f(bounds.x, bounds.y));
    pnts.push_back(ofVec2f(bounds.x+bounds.width, bounds.y));
    pnts.push_back(ofVec2f(bounds.x+bounds.width, bounds.y+bounds.height));
    pnts.push_back(ofVec2f(bounds.x, bounds.y+bounds.height));
    return trim_lines_inside_polygon(lines, pnts);
}
void ofxGCode::trim_inside_box(ofRectangle bounds){
    lines = trim_lines_inside_box(lines, bounds);
}

//takes any vector of lines and returns a new vector where any lines outside the shape have been removed
vector<GLine> ofxGCode::trim_lines_outside_polygon(vector<GLine> lines, vector<ofVec2f> bounds){
    vector<GLine> output;
    
    //go through each line and try to trim it
    for (int i=0; i<lines.size(); i++){
        //trim it, adding any extra lines generated to the output
        lines[i].trim_outside_polygon(bounds, &output);
        //then if this line is still valid, add it as well
        if (!lines[i].skip_me){
            output.push_back(lines[i]);
        }
    }
    
    return output;
}

//trims the current list of lines, removing any points outside the given polygon
void ofxGCode::trim_outside_polygon(vector<ofVec2f> bounds){
    lines = trim_lines_outside_polygon(lines, bounds);
}

//helper function for trimming rectangles
vector<GLine> ofxGCode::trim_lines_outside_box(vector<GLine> lines, ofRectangle bounds){
    vector<ofVec2f> pnts;
    pnts.push_back(ofVec2f(bounds.x, bounds.y));
    pnts.push_back(ofVec2f(bounds.x+bounds.width, bounds.y));
    pnts.push_back(ofVec2f(bounds.x+bounds.width, bounds.y+bounds.height));
    pnts.push_back(ofVec2f(bounds.x, bounds.y+bounds.height));
    return trim_lines_outside_polygon(lines, pnts);
}
void ofxGCode::trim_outside_box(ofRectangle bounds){
    lines = trim_lines_outside_box(lines, bounds);
}




ofPoint ofxGCode::find_intersection(GCodePoint a, GCodePoint b, vector<ofVec2f> bounds){
    for (int i=0; i<bounds.size(); i++){
        int next_id = (i+1)%bounds.size();
        
        ofPoint out;
        ofPoint pnt_a = ofPoint(a.x, a.y);
        ofPoint pnt_b = ofPoint(b.x, b.y);
        ofPoint border1 = bounds[i];
        ofPoint border2 = bounds[next_id];
        
        if (ofLineSegmentIntersection(pnt_a, pnt_b, border1, border2, out)){
            return out;
        }
    }
    
    return ofPoint(-1,-1);
}

vector<ofPoint> ofxGCode::find_intersections(GCodePoint a, GCodePoint b, vector<ofVec2f> bounds){
    vector<ofPoint> vals;
    for (int i=0; i<bounds.size(); i++){
        int next_id = (i+1)%bounds.size();
        
        ofPoint out;
        ofPoint pnt_a = ofPoint(a.x, a.y);
        ofPoint pnt_b = ofPoint(b.x, b.y);
        ofPoint border1 = bounds[i];
        ofPoint border2 = bounds[next_id];
        
        if (ofLineSegmentIntersection(pnt_a, pnt_b, border1, border2, out)){
            vals.push_back(out);
        }
    }
    
    return vals;
}

//--------------------------------------------------------------
//returns true if points are close enough to consider identical
bool ofxGCode::are_points_the_same(ofVec2f a, ofVec2f b){
    return ofDistSquared(a.x,a.y, b.x,b.y) <= powf(max_dist_to_consider_pnts_touching,2);
    
}

//--------------------------------------------------------------
//any lines outside of this bounfs will be forced to draw from the center out.
void ofxGCode::set_outwards_only_bounds(ofRectangle safe_area){
    
    for (int i=0; i<lines.size(); i++){
        bool a_inside = safe_area.inside(lines[i].a);
        bool b_inside = safe_area.inside(lines[i].b);
        //if both sides are in the safe area, do nothing. We can flip this line if we need to
        if (a_inside && b_inside){
            lines[i].do_not_reverse = false;
        }
        
        //if only A is inside, keep the order but make sure it doesn't get flipped
        else if (a_inside && !b_inside){
            lines[i].do_not_reverse = true;
        }
        
        //if only B is inside, flip it and make sure it does not get flipped again
        else if (b_inside && !a_inside){
            lines[i].swap_a_and_b();
            lines[i].do_not_reverse = true;
        }
        
        //if neither is inside, select the point closest to the center and have that be A
        else{
            ofVec2f center;
            center.x = (safe_area.x + safe_area.x+safe_area.width)/2;
            center.y = (safe_area.y + safe_area.y+safe_area.height)/2;
            if (center.squareDistance(lines[i].a) > center.squareDistance(lines[i].b)){
                lines[i].swap_a_and_b();
            }
            lines[i].do_not_reverse = true;
        }
    }
    
    /*
    //cout<<"---set outwards---"<<endl;
    if (list.size() < 2){
        return;
    }
    
    ofVec2f center;
    center.x = (safe_area.x + safe_area.x+safe_area.width)/2;
    center.y = (safe_area.y + safe_area.y+safe_area.height)/2;
    
    //cout<<"center "<<center.x<<" , "<<center.y<<endl;
    
    for (int i=0; i<list.size()-1; i++){
        GCodePoint pnt_a = list[i];
        //is this the start of a line segment?
        if (pnt_a.pressure == 0){
            int end_id = i+1;
            GCodePoint pnt_b = list[end_id];
            for (int k=i+1; k<list.size(); k++){
                if (list[k].pressure == 0){
                    break;
                }else{
                    end_id = k;
                    pnt_b = list[k];
                }
            }

            //is at least one point outside?
            if (!safe_area.inside(pnt_a.x, pnt_a.y) || !safe_area.inside(pnt_b.x, pnt_b.y)){
                
                //if they're both outside, we need to split
                if (!safe_area.inside(pnt_a.x, pnt_a.y) && !safe_area.inside(pnt_b.x, pnt_b.y)){
//                    cout<<"gotta split at "<<list[i].x<<" , "<<list[i].y<<endl;
//                    cout<<" ends  at "<<list[end_id].x<<" , "<<list[end_id].y<<endl;
//                    cout<<"  i: "<<i<<"  end id "<<end_id<<endl;
                    //start at a and move along until we find a segment in the safe zone
                    for (int p=i+1; p<=end_id; p++){
                        //cout<<"  trying "<<list[p].x<<" , "<<list[p].y<<endl;
                        if (safe_area.inside(list[p].x, list[p].y)){
                           // cout<<"  "<<list[p].x<<" , "<<list[p].y<<" works"<<endl;
                            end_id = p;
                            pnt_b = list[p];
                            GCodePoint new_p = GCodePoint(list[p].x,list[p].y,speed,0);
                            list.insert(list.begin()+end_id+1, new_p);
                            break;
                        }
                    }
                }
                
                bool need_to_flip = false;
                
                //if neight is in the safe area, figure out which is closer to the center
                if (!safe_area.inside(pnt_b.x, pnt_b.y) && !safe_area.inside(pnt_a.x, pnt_a.y)){
                    float dist_a = ofDistSquared(pnt_a.x, pnt_a.y, center.x, center.y);
                    float dist_b = ofDistSquared(pnt_b.x, pnt_b.y, center.x, center.y);
                    if (dist_b < dist_a){
                        need_to_flip = true;
                    }
                }
                
                //if b is inside and a is out, we need to flip
                if (safe_area.inside(pnt_b.x, pnt_b.y) && !safe_area.inside(pnt_a.x, pnt_a.y)){
                    need_to_flip = true;
                }
                
                //if they are in the right order, just keep them that way
//                if (safe_area.inside(pnt_a.x, pnt_a.y)){
//                    //do nothing
////                    cout<<"do nothing for "<<i<<" to "<<end_id<<endl;
////                    cout<<"  start "<<list[i].x<<" , "<<list[i].y<<endl;
////                    cout<<"  end "<<list[end_id].x<<" , "<<list[end_id].y<<endl;
//                }
                
                if (need_to_flip){
//                    cout<<"flip for "<<i<<" to "<<end_id<<endl;
//                    cout<<"  start "<<list[i].x<<" , "<<list[i].y<<endl;
//                    cout<<"  end "<<list[end_id].x<<" , "<<list[end_id].y<<endl;
                    GCodePoint * t0 = &list[i];
                    GCodePoint * t1 = &list[end_id];
                    int temp_pressure = t0->pressure;
                    int temp_speed = t0->speed;
                    t0->pressure = t1->pressure;
                    t0->speed = t1->speed;
                    t1->pressure = temp_pressure;
                    t1->speed = temp_speed;

                    std::reverse(list.begin()+i, list.begin()+end_id+1);
                }

                //mark them all as do not reverse
                for (int p=i; p<=end_id; p++){
                    //cout<<"   set "<<p<<" to not reverse at "<<list[p].x<<" , "<<list[p].y<<endl;
                    list[p].do_not_reverse = true;
                }
            }
        }
    }
    */
}

//--------------------------------------------------------------
//code is a modified version of code by Randolph Franklin
//from http://paulbourke.net/geometry/insidepoly/
bool ofxGCode::checkInPolygon(vector<ofVec2f> p, float x, float y)
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




//these are depricated
void ofxGCode::clip_outside(ofRectangle bounding_box){
    cout<<"clip_outside is depricated. Use trim_outside_box"<<endl;
}

//this would be more efficient if you used Trammel's clipping class
//this thing is wildly inefficient
void ofxGCode::clip_inside(ofRectangle bounding_box){
    cout<<"clip_inside is depricated. Use trim_inside_box"<<endl;
}

void ofxGCode::clip_inside(vector<ofVec2f> bounds){
    cout<<"clip_inside is depricated. Use trim_inside_polygon"<<endl;
}



