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
    
    set_size(ofGetWidth(), ofGetHeight());
    
    //display stuff
    show_transit_lines = false;
    show_path_with_color = false;
    show_do_not_reverse = false;
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
    
}

//genertaes gcode and writes it to a file
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
    line(x,y, x+w, y);
    line(x+w,y, x+w, y+h);
    line(x+w,y+h, x, y+h);
    line(x,y+h, x, y);
}

void ofxGCode::rounded_rect(ofRectangle rect, float corner_size, int corner_resolution){
    vector<ofVec2f> pnts = get_rounded_pnts(rect.x, rect.y, rect.width, rect.height, corner_size, corner_resolution);
    polygon(pnts);
}
void ofxGCode::rounded_rect(float x, float y, float w, float h, float corner_size, int corner_resolution){
    vector<ofVec2f> pnts = get_rounded_pnts(x, y, w, h, corner_size, corner_resolution);
    polygon(pnts);
}

vector<ofVec2f> ofxGCode::get_rounded_pnts(ofRectangle rect, float corner_size, int corner_resolution){
    return get_rounded_pnts(rect.x, rect.y, rect.width, rect.height, corner_size, corner_resolution);
}
vector<ofVec2f> ofxGCode::get_rounded_pnts(float x, float y, float w, float h, float corner_size, int corner_resolution){
    
    ofRectangle base = ofRectangle(x,y,w,h);
    
    vector<ofVec2f> pnts;
    
    pnts.push_back( ofVec2f(base.x+corner_size, base.y) );
    pnts.push_back( ofVec2f(base.x+base.width-corner_size, base.y) );
    
    //top right
    for (int i=1; i<corner_resolution; i++){
        ofVec2f center;
        center.x = base.x+base.width-corner_size;
        center.y = base.y+corner_size;
        
        float angle = ofMap(i, 0, corner_resolution, -PI/2, 0);
        ofVec2f pos;
        pos.x = center.x + cos(angle) * corner_size;
        pos.y = center.y + sin(angle) * corner_size;
        
        pnts.push_back(pos);
    }
    
    pnts.push_back( ofVec2f(base.x+base.width, base.y+base.height-corner_size) );
    
    //bottom right
    for (int i=1; i<corner_resolution; i++){
        ofVec2f center;
        center.x = base.x+base.width-corner_size;
        center.y = base.y+base.height-corner_size;
        
        float angle = ofMap(i, 0, corner_resolution, 0, PI/2);
        ofVec2f pos;
        pos.x = center.x + cos(angle) * corner_size;
        pos.y = center.y + sin(angle) * corner_size;
        
        pnts.push_back(pos);
    }
    
    pnts.push_back( ofVec2f(base.x+corner_size, base.y+base.height) );
    
    //bottom left
    for (int i=1; i<corner_resolution; i++){
        ofVec2f center;
        center.x = base.x+corner_size;
        center.y = base.y+base.height-corner_size;
        
        float angle = ofMap(i, 0, corner_resolution, PI/2, PI);
        ofVec2f pos;
        pos.x = center.x + cos(angle) * corner_size;
        pos.y = center.y + sin(angle) * corner_size;
        
        pnts.push_back(pos);
    }
    
    pnts.push_back( ofVec2f(base.x, base.y+corner_size) );
    
    //top left
    for (int i=1; i<corner_resolution; i++){
        ofVec2f center;
        center.x = base.x+corner_size;
        center.y = base.y+corner_size;
        
        float angle = ofMap(i, 0, corner_resolution, PI, (3*PI)/2);
        ofVec2f pos;
        pos.x = center.x + cos(angle) * corner_size;
        pos.y = center.y + sin(angle) * corner_size;
        
        pnts.push_back(pos);
    }
    
    return pnts;
}

void ofxGCode::circle(ofVec2f center, float size){
    circle(center.x, center.y, size);
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
vector<ofVec2f> ofxGCode::get_circle_pnts(ofVec2f center, float size, int steps){
    float angle_step = TWO_PI/steps;
    vector<ofVec2f> pnts;
    for (int i=0; i<steps; i++){
        float angle = angle_step * i;
        ofVec2f pos;
        pos.x = center.x + cos(angle) * size;
        pos.y = center.y + sin(angle) * size;
        pnts.push_back(pos);
    }
    return pnts;
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
        line(shape_pnts[i].x, shape_pnts[i].y, shape_pnts[i+1].x, shape_pnts[i+1].y);
    }
    if (close){
        line(shape_pnts[shape_pnts.size()-1].x, shape_pnts[shape_pnts.size()-1].y, shape_pnts[0].x, shape_pnts[0].y);
    }
}

//drawing polygone from points
void ofxGCode::polygon(vector<ofVec2f> pnts, bool close_shape){
    begin_shape();
    for (int i=0; i<pnts.size(); i++){
        vertex(pnts[i]);
    }
    end_shape(close_shape);
}

//Lines
void ofxGCode::line(GLine _line){
    if (_line.skip_me)  return;
    line(_line.a.x,_line.a.y, _line.b.x,_line.b.y);
}
void ofxGCode::line(ofVec2f a, ofVec2f b){
    line(a.x, a.y, b.x, b.y);
}

void ofxGCode::line(float x1, float y1, float x2, float y2){
    ofVec2f p1 = getModelPoint(x1,y1);
    ofVec2f p2 = getModelPoint(x2,y2);
    
    //clip the points to fit our canvas, rejecting the line if it would be entirely out of bounds
    if (!clip.clip(p1, p2)) {
        //cout<<"no part of this line is on screen"<<endl;
        return;
    }
    
    GLine line;
    line.set(p1, p2);
    lines.push_back(line);
}

//adds a vector of GLines
void ofxGCode::add_lines(vector<GLine> new_lines){
    for (int i=0; i<new_lines.size(); i++){
        line(new_lines[i]);
    }
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
    vector<ofVec2f> pnts = get_bezier_pnts(p1, c1, c2, p2, steps);
    begin_shape();
    for (int i=0; i<pnts.size(); i++){
        vertex(pnts[i]);
    }
    end_shape(false);
}

vector<ofVec2f> ofxGCode::get_bezier_pnts(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, int steps){
    vector<ofVec2f> pnts;
    for (int i=0; i<=steps; i++){
        ofPoint pnt = ofBezierPoint(p1, c1, c2, p2, (float)i/(float)steps);
        pnts.push_back(pnt);
    }
    return pnts;
}

void ofxGCode::dot(float x, float y){
    line(x,y,x,y);
}


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
                line(polylines[j][k].x,polylines[j][k].y, polylines[j][next_id].x,polylines[j][next_id].y);
            }
        }
    }
    
    ofPopMatrix();
}


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

//sets all current lines so they cannot be trimmed or set as outwards only
void  ofxGCode::lock_lines(){
    for (int i=0; i<lines.size(); i++){
        lines[i].set_locked(true);
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
vector<GLine> ofxGCode::trim_lines_inside(vector<GLine> lines, vector<ofVec2f> bounds){
    vector<GLine> output;
    
    //go through each line and try to trim it
    for (int i=0; i<lines.size(); i++){
        //trim it, adding any extra lines generated to the output
        lines[i].trim_inside(bounds, &output);
        //then if this line is still valid, add it as well
        if (!lines[i].skip_me){
            output.push_back(lines[i]);
        }
    }
    
    return output;
}

//trims the current list of lines, removing any points inside the given polygon
void ofxGCode::trim_inside(vector<ofVec2f> bounds){
    lines = trim_lines_inside(lines, bounds);
}

//helper function for trimming rectangles
vector<GLine> ofxGCode::trim_lines_inside(vector<GLine> lines, ofRectangle bounds){
    vector<ofVec2f> pnts;
    pnts.push_back(ofVec2f(bounds.x, bounds.y));
    pnts.push_back(ofVec2f(bounds.x+bounds.width, bounds.y));
    pnts.push_back(ofVec2f(bounds.x+bounds.width, bounds.y+bounds.height));
    pnts.push_back(ofVec2f(bounds.x, bounds.y+bounds.height));
    return trim_lines_inside(lines, pnts);
}
void ofxGCode::trim_inside(ofRectangle bounds){
    lines = trim_lines_inside(lines, bounds);
}

//takes any vector of lines and returns a new vector where any lines outside the shape have been removed
vector<GLine> ofxGCode::trim_lines_outside(vector<GLine> lines, vector<ofVec2f> bounds){
    vector<GLine> output;
    
    //go through each line and try to trim it
    for (int i=0; i<lines.size(); i++){
        //trim it, adding any extra lines generated to the output
        lines[i].trim_outside(bounds, &output);
        //then if this line is still valid, add it as well
        if (!lines[i].skip_me){
            output.push_back(lines[i]);
        }
    }
    
    return output;
}

//trims the current list of lines, removing any points outside the given polygon
void ofxGCode::trim_outside(vector<ofVec2f> bounds){
    lines = trim_lines_outside(lines, bounds);
}

//helper function for trimming rectangles
vector<GLine> ofxGCode::trim_lines_outside(vector<GLine> lines, ofRectangle bounds){
    vector<ofVec2f> pnts;
    pnts.push_back(ofVec2f(bounds.x, bounds.y));
    pnts.push_back(ofVec2f(bounds.x+bounds.width, bounds.y));
    pnts.push_back(ofVec2f(bounds.x+bounds.width, bounds.y+bounds.height));
    pnts.push_back(ofVec2f(bounds.x, bounds.y+bounds.height));
    return trim_lines_outside(lines, pnts);
}
void ofxGCode::trim_outside(ofRectangle bounds){
    lines = trim_lines_outside(lines, bounds);
}


//--------------------------------------------------------------
//any lines outside of this bounds will be forced to draw from the center out.
void ofxGCode::set_outwards_only_bounds(ofRectangle safe_area){
    
    for (int i=0; i<lines.size(); i++){
        if (!lines[i].is_locked){
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
            
            //if neither is inside, see if the center point of the line is inside and attempt to split it
            else{
                ofVec2f mid = lines[i].a*0.5 + lines[i].b*0.5;
                bool mid_inside = safe_area.inside(mid);
                //if midpoint is inside, split it!
                if (mid_inside){
                    //make a new line
                    GLine new_line = GLine(mid, lines[i].b);
                    lines.push_back(new_line);
                    //trim this line
                    lines[i].b = mid;
                    //push i back so we re-evaluate this line
                    i--;
                }
                //otherwise, give up. select the point closest to the center and have that be A
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
            
            
        }
    }
    
}

//--------------------------------------------------------------
void ofxGCode::translate(float x, float y){
    for (int i=0; i<lines.size(); i++){
        if (!lines[i].is_locked){
            lines[i].a.x += x;
            lines[i].a.y += y;
            lines[i].b.x += x;
            lines[i].b.y += y;
        }
    }
}

//--------------------------------------------------------------
vector<vector<ofVec2f>> ofxGCode::load_outlines(string file_path){
    vector<vector<ofVec2f>> outlines;
    
    ofFile file(file_path);
    
    if(!file.exists()){
        cout<<"The outline file " << file_path << " is missing"<<endl;
        return outlines;
    }
    ofBuffer buffer(file);
    
    //Read file line by line
    vector<ofVec2f> cur_outline;
    for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        //Split line into strings
        vector<string> words = ofSplitString(line, ",");

        if (words.size()>0){

            //using # to mark the start of a new shape
            if (words[0]=="#"){
                if (cur_outline.size() > 1){
                    outlines.push_back(cur_outline);
                    cur_outline.clear();
                }
            }
            //there should be two words
            else if (words.size()==2){
                ofVec2f pnt;
                pnt.x = ofToFloat(words[0]);
                pnt.y = ofToFloat(words[1]);
                cur_outline.push_back(pnt);
            }
        }

    }
    
    //add the last shape if there's anything there
    cout<<cur_outline.size()<<endl;
    if (cur_outline.size() > 1){
        outlines.push_back(cur_outline);
    }
    
    return outlines;
}

//--------------------------------------------------------------
vector<GLine> ofxGCode::load_lines(string file_path){
    vector<GLine> new_lines;
    
    ofFile file(file_path);
    
    if(!file.exists()){
        cout<<"The file " << file_path << " is missing"<<endl;
        return new_lines;
    }
    ofBuffer buffer(file);
    
    //Read file line by line
    for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        //Split line into strings
        vector<string> words = ofSplitString(line, ",");
        
        if (words.size()==4){
            GLine line;
            line.a.x =  ofToFloat(words[0]);
            line.a.y =  ofToFloat(words[1]);
            line.b.x =  ofToFloat(words[2]);
            line.b.y =  ofToFloat(words[3]);
            new_lines.push_back(line);
        }
        
    }
    return new_lines;
    
}

//--------------------------------------------------------------
void ofxGCode::save_lines(string file_path){
    ofFile myTextFile;
    myTextFile.open(file_path,ofFile::WriteOnly);
    for (int i=0; i<lines.size(); i++){
        myTextFile<<lines[i].a.x<<","<<lines[i].a.y<<","<<lines[i].b.x<<","<<lines[i].b.y<<endl;
    }
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
    cout<<"clip_outside is depricated. Use trim_outside"<<endl;
}

//this would be more efficient if you used Trammel's clipping class
//this thing is wildly inefficient
void ofxGCode::clip_inside(ofRectangle bounding_box){
    cout<<"clip_inside is depricated. Use trim_inside"<<endl;
}

void ofxGCode::clip_inside(vector<ofVec2f> bounds){
    cout<<"clip_inside is depricated. Use trim_inside"<<endl;
}



