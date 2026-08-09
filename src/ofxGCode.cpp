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
    demo_fade_prc = 0.75;
}

void ofxGCode::set_size(int w, int h){
    clip.setup(ofVec2f(0, 0), ofVec2f(w,h));
    
    clear();
}

void ofxGCode::clear(){
    lines.clear();
    segments.clear();
}

void ofxGCode::draw(int max_lines_to_show){
    
    if (max_lines_to_show <= 0) max_lines_to_show = lines.size();
    
    int end_index = MIN(max_lines_to_show, lines.size());
    for (int i=0; i<end_index; i++){
        //the line
        GLine line = lines[i];
        
        ofSetColor(demo_col.r, demo_col.g, demo_col.b, 255 * demo_fade_prc);
        
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

//generates gcode and writes it to a file
void ofxGCode::save(string name){
    float inches_per_pixel = 1.0 / pixels_per_inch;
    
    vector<string> commands;
    
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
    
    ofLogNotice("ofxGCode") << "transit distance: " << measureTransitDistance();
    
    //write it to file
    ofLogNotice("ofxGCode") << "saving " << commands.size() << " commands";
    ofFile myTextFile;
    myTextFile.open(name,ofFile::WriteOnly);
    for (int i=0; i<commands.size(); i++){
        myTextFile<<commands[i]<<endl;
    }
    
    ofLogNotice("ofxGCode") << "saved " << name;
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
vector<ofVec2f> ofxGCode::get_circle_pnts(ofVec2f center, float size, int steps, float angle_offset){
    float angle_step = TWO_PI/steps;
    vector<ofVec2f> pnts;
    for (int i=0; i<steps; i++){
        float angle = angle_offset + angle_step * i;
        ofVec2f pos;
        pos.x = center.x + cos(angle) * size;
        pos.y = center.y + sin(angle) * size;
        pnts.push_back(pos);
    }
    return pnts;
}

vector<ofVec2f> ofxGCode::get_oval_pnts(ofVec2f center, float width, float height, int steps, float angle_offset){
    float angle_step = TWO_PI/steps;
    vector<ofVec2f> pnts;
    for (int i=0; i<steps; i++){
        float angle = angle_offset + angle_step * i;
        ofVec2f pos;
        pos.x = center.x + cos(angle) * width;
        pos.y = center.y + sin(angle) * height;
        pnts.push_back(pos);
    }
    return pnts;
}

vector<ofVec2f> ofxGCode::get_arc_pnts(ofVec2f center, float size, int steps, float start_angle, float end_angle, float height_scale){
    vector<ofVec2f> pnts;
    for (int i=0; i<steps; i++){
        float prc = (float)i / (float)(steps-1);
        float angle = (1.0-prc)*start_angle + prc*end_angle;
        ofVec2f pos;
        pos.x = center.x + cos(angle) * size;
        pos.y = center.y + sin(angle) * size * height_scale;
        pnts.push_back(pos);
    }
    return pnts;
}

vector<ofVec2f> ofxGCode::get_arc_points_ijk(ofVec2f start, ofVec2f end, ofVec2f center, bool clockwise, int steps){
    vector<ofVec2f> pnts;
    
    ofVec2f v1 = start - center;
    ofVec2f v2 = end - center;
    float angle1 = atan2(v1.y, v1.x);
    float angle2 = atan2(v2.y, v2.x);
    float radius = v1.length();
    
    float sweep = angle2 - angle1;
    if (clockwise) {
        if (sweep > 0) sweep -= TWO_PI;
    } else {
        if (sweep < 0) sweep += TWO_PI;
    }
    
    for (int i = 0; i <= steps; i++){
        float t = (float)i / (float)steps;
        float angle = angle1 + sweep * t;
        ofVec2f pos;
        pos.x = center.x + cos(angle) * radius;
        pos.y = center.y + sin(angle) * radius;
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

//drawing polygon from points
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

    // Mirror into the arc-aware segment sequence
    GSegment seg;
    seg.type  = GSegment::Type::Line;
    seg.start = p1;
    seg.end   = p2;
    segments.push_back(seg);
}

//adds a vector of GLines
void ofxGCode::add_lines(vector<GLine> new_lines){
    for (int i=0; i<new_lines.size(); i++){
        line(new_lines[i]);
    }
}

//Thick lines are just multiple lines, evenly spaced
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

vector<ofVec2f> ofxGCode::resample_lines(vector<ofVec2f> src_pnts, float sample_dist, bool close_shape, int steps_per_point){
    vector<ofVec2f> new_pnts;
    int end_index = close_shape ? src_pnts.size() : src_pnts.size()-1;
    
    float cur_dist = 0;
    ofVec2f prev_pos = ofVec2f(src_pnts[0]);
    new_pnts.push_back(src_pnts[0]);
    
    for (int i=0; i<end_index; i++){
        ofVec2f a = src_pnts[i];
        ofVec2f b = src_pnts[ (i+1)%src_pnts.size()];
        
        for (int k=0; k<=steps_per_point; k++){
            float prc = (float)k / (float)steps_per_point;
            ofVec2f pnt = (1.0-prc)*a + prc*b;
            cur_dist += ofDist(prev_pos.x, prev_pos.y, pnt.x, pnt.y);
            if (cur_dist >= sample_dist){
                new_pnts.push_back(pnt);
                cur_dist -= sample_dist;
            }
            prev_pos = ofVec2f(pnt);
        }
    }
    
    return new_pnts;
}

vector<GLine> ofxGCode::pnts_to_lines(vector<ofVec2f> pnts, bool close){
    vector<GLine> new_lines;
    for (int i=0; i<pnts.size()-1; i++){
        new_lines.push_back(GLine(pnts[i], pnts[i+1]));
    }
    
    if (close){
        new_lines.push_back(GLine(pnts[pnts.size()-1], pnts[0]));
    }
    
    return new_lines;
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

//be aware that this tool may no longer work
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

vector<vector<ofVec2f>> ofxGCode::get_text_outlines(string text, ofTrueTypeFont * font){
    bool vflip = true; // OF flips y coordinate in the default perspective,
    // should be false if using a camera for example
    bool filled = false; // or false for contours
    vector < ofPath > paths = font->getStringAsPoints(text, vflip, filled);
    
    vector<vector<ofVec2f>> outlines;
    
    
    for (int i = 0; i < paths.size(); i++){
        // for every character break it out to polylines
        vector <ofPolyline> polylines = paths[i].getOutline();
        
        // for every polyline, draw lines
        for (int j = 0; j < polylines.size(); j++){
            vector<ofVec2f> outline;
            for (int k = 0; k < polylines[j].size(); k++){
                outline.push_back(ofVec2f(polylines[j][k].x,polylines[j][k].y));
            }
            outlines.push_back(outline);
        }
    }
    
    return outlines;
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

//This calls getModelPoint on each point in a vector
vector<ofVec2f> ofxGCode::convert_pnts_to_model_point(vector<ofVec2f> src_pnts){
    vector<ofVec2f> pnts;
    pnts.resize(src_pnts.size());
    for (int i=0; i<src_pnts.size(); i++){
        pnts[i] = getModelPoint(src_pnts[i]);
    }
    return  pnts;
}

//This is the same as convert_pnts_to_model_point but applying the transformation to each point in a set of lines
vector<GLine> ofxGCode::convert_lines_to_model_point(vector<GLine> src_lines){
    vector<GLine> lines;
    lines.resize(src_lines.size());
    for (int i=0; i<src_lines.size(); i++){
        lines[i].set(src_lines[i]);
        lines[i].a = getModelPoint(src_lines[i].a);
        lines[i].b = getModelPoint(src_lines[i].b);
    }
    return  lines;
}

//this is not perfect yet. Some of the resulting order is definitely not as efficient as it could be
void ofxGCode::sort(){
    if (lines.size() == 0){
        return;
    }
    
    //if there are a bunch of locked lines, keep them at the start
    vector<GLine> leading_locked_lines;
    while (lines[0].is_locked){
        leading_locked_lines.push_back(lines[0]);
        lines.erase(lines.begin());
        if (lines.size() == 0){
            break;
        }
    }
    
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
    
    //though any previously locked lines back to the start
    for (int i=0; i<leading_locked_lines.size(); i++){
        lines.push_back(leading_locked_lines[i]);
    }
    
    //go through finding the closest end point
    ofVec2f cur_pnt = ofVec2f();
    while (line_groups.size() > 0){
        int close_id=0;
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
            
            //are either of these points the closest so far?
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
        //cout<<"remove "<<close_id<<" out of "<<line_groups.size()<<endl;
        //if (close_id <  line_groups.size()){
        line_groups.erase(line_groups.begin() + close_id);
        //}
    }
}

//sets all current lines so they cannot be trimmed or set as outwards only
void  ofxGCode::lock_lines(){
    for (int i=0; i<lines.size(); i++){
        lines[i].set_locked(true);
    }
}

void  ofxGCode::unlock_lines(){
    for (int i=0; i<lines.size(); i++){
        lines[i].set_locked(false);
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

//takes a list of lines and removes any lines that intersect a static line
vector<GLine> ofxGCode::trim_intersecting_lines(vector<GLine> lines_to_trim, vector<GLine> static_lines){
    vector<GLine> val;
    for (int i=0; i<lines_to_trim.size(); i++){
        bool can_add = true;
        for (int k=0; k<static_lines.size(); k++){
            if (lines_to_trim[i].intersects(static_lines[k])){
                can_add = false;
                break;
            }
        }
        if (can_add){
            val.push_back(lines_to_trim[i]);
        }
    }
    return  val;
}

void ofxGCode::demo_trim(float x1, float y1, float x2, float y2, bool do_translate){
    //first trim in the box
    ofRectangle box;
    box.x = x1;
    box.y = y1;
    box.width = x2-x1;
    box.height = y2-y1;
    
    trim_outside(box);
    
    if (do_translate){
        translate(-x1, -y1);
    }
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
void ofxGCode::rotate_ccw(){
    float orig_w = clip.max.x;
    float orig_h = clip.max.y;
    for (int i=0; i<lines.size(); i++){
        ofVec2f * pnts[2];
        pnts[0] = &lines[i].a;
        pnts[1] = &lines[i].b;
        for (int k=0; k<2; k++){
            float orig_x = pnts[k]->x;
            float orig_y = pnts[k]->y;
            
            pnts[k]->x = orig_y;
            pnts[k]->y = orig_w-orig_x;
        }
    }
    
    //might need to set clipping plane here
    clip.setup(ofVec2f(0,0), ofVec2f(orig_h, orig_w));
}

//--------------------------------------------------------------
//0-top left, 1-top right, 2-bottom right, 3-bottom left
ofVec2f ofxGCode::perspective_warp(ofVec2f orig_pnt, ofRectangle src_bounds, ofVec2f new_bounds[4], float x_curve, float y_curve){
    
    //get the percentage of the x and y in the original box
    float x_prc = (orig_pnt.x-src_bounds.x) / src_bounds.width;
    float y_prc = (orig_pnt.y-src_bounds.y) / src_bounds.height;
    
    x_prc = powf(x_prc, x_curve);
    y_prc = powf(y_prc, y_curve);
    
    //now move along the top and bottom of the new shape to the same x_prc
    ofVec2f top_pnt = (1.0-x_prc)*new_bounds[0] + x_prc*new_bounds[1];
    ofVec2f bot_pnt = (1.0-x_prc)*new_bounds[3] + x_prc*new_bounds[2];
    
    //now lerp between those based on the y
    ofVec2f new_pos = (1.0-y_prc)*top_pnt + y_prc*bot_pnt;
    
    
    return new_pos;
}

//--------------------------------------------------------------
vector<vector<ofVec2f>> ofxGCode::load_outlines(string file_path){
    vector<vector<ofVec2f>> outlines;
    
    ofFile file(file_path);
    
    if(!file.exists()){
        ofLogError("ofxGCode") << "The outline file " << file_path << " is missing";
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
    ofLogVerbose("ofxGCode") << "outline size: " << cur_outline.size();
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
        ofLogError("ofxGCode") << "The file " << file_path << " is missing";
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

bool ofxGCode::checkInPolygon(vector<ofVec2f> p, ofVec2f pnt){
    return checkInPolygon(p, pnt.x, pnt.y);
}

//--------------------------------------------------------------
// 3-axis G-code output
//--------------------------------------------------------------

string ofxGCode::toGCodeString(float safeZ){
    vector<string> commands;
    
    // Preamble
    commands.push_back("G21 ; mm mode");
    commands.push_back("G90 ; absolute positioning");
    commands.push_back("G0 Z" + ofToString(safeZ, 3));
    commands.push_back("G0 X0 Y0");
    
    ofVec2f lastPos2D(0, 0);
    bool penIsUp = true;
    
    for (size_t i = 0; i < lines.size(); i++){
        GLine line = lines[i];
        float z = (i < z_values.size()) ? z_values[i] : 0.0f;
        
        // If we're not at the start of this line, travel there
        if (line.a != lastPos2D || penIsUp) {
            if (!penIsUp) {
                // Retract
                commands.push_back("G0 Z" + ofToString(safeZ, 3));
                penIsUp = true;
            }
            // Rapid to start XY
            commands.push_back("G0 X" + ofToString(line.a.x, 3) + " Y" + ofToString(line.a.y, 3));
            // Plunge to Z
            commands.push_back("G1 Z" + ofToString(z, 3) + " F300");
            penIsUp = false;
        }
        
        // Feed move to end point
        commands.push_back("G1 X" + ofToString(line.b.x, 3) + " Y" + ofToString(line.b.y, 3) + " Z" + ofToString(z, 3));
        
        lastPos2D = line.b;
    }
    
    // Closing
    commands.push_back("G0 Z" + ofToString(safeZ, 3));
    commands.push_back("G0 X0 Y0");
    commands.push_back("M2 ; end program");
    
    string result;
    for (const auto& cmd : commands) {
        result += cmd + "\n";
    }
    return result;
}

void ofxGCode::save3D(string name, float safeZ){
    string gcodeStr = toGCodeString(safeZ);
    
    ofFile myTextFile;
    myTextFile.open(name, ofFile::WriteOnly);
    myTextFile << gcodeStr;
    
    ofLogNotice("ofxGCode") << "3D G-code saved to " << name;
}


// ===========================================================================
//  Biarc Bezier approximation
//
//  Implements the biarc method described by D. Lacko:
//  http://dlacko.org/blog/2016/10/19/approximating-bezier-curves-by-biarcs/
//
//  This was inspired by Austin Whittier's Observable notebook:
//  https://observablehq.com/@awhitty/approximating-bezier-curves-for-cnc
//  Whittier's notebook explores circular arc approximation of Bezier curves
//  for CNC G-code, and explicitly points to the biarc method (above) as
//  "a better way" that guarantees G1 tangent continuity at arc joints —
//  which is what this implementation uses.
// ===========================================================================

// ---------------------------------------------------------------------------
// Internal helpers (file-scope static so they don't pollute the public API)
// ---------------------------------------------------------------------------

static ofVec2f s_bezier_eval(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, float t)
{
    float mt = 1.0f - t;
    return p1*(mt*mt*mt) + c1*(3.f*mt*mt*t) + c2*(3.f*mt*t*t) + p2*(t*t*t);
}

// Unnormalized first derivative B'(t) of the cubic Bezier.
static ofVec2f s_bezier_deriv(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, float t)
{
    float mt = 1.0f - t;
    return (c1-p1)*(3.f*mt*mt) + (c2-c1)*(6.f*mt*t) + (p2-c2)*(3.f*t*t);
}

// Normalised tangent at t.  Falls back to (1,0) for degenerate curves.
static ofVec2f s_bezier_tangent(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, float t)
{
    ofVec2f d = s_bezier_deriv(p1, c1, c2, p2, t);
    float len = d.length();
    return (len < 1e-8f) ? ofVec2f(1.f, 0.f) : d*(1.f/len);
}

// Intersection of parametric lines  P1+s·D1  and  P2+u·D2.
// Returns false when the lines are parallel.
static bool s_line_intersect(ofVec2f p1, ofVec2f d1,
                              ofVec2f p2, ofVec2f d2,
                              ofVec2f& result)
{
    float denom = d1.x*d2.y - d1.y*d2.x;
    if (fabsf(denom) < 1e-10f) return false;
    float s = ((p2.x-p1.x)*d2.y - (p2.y-p1.y)*d2.x) / denom;
    result = p1 + d1*s;
    return true;
}

// Centre of the unique circle that is tangent to 'tangent' at 'p1'
// and also passes through 'p2'.
// Returns false when the points are coincident or the chord is parallel
// to the tangent (straight-line degenerate case).
static bool s_arc_center(ofVec2f p1, ofVec2f tangent, ofVec2f p2, ofVec2f& center)
{
    ofVec2f perp_t(-tangent.y, tangent.x);          // perpendicular to tangent at p1
    ofVec2f mid  = (p1 + p2) * 0.5f;
    ofVec2f chord = p2 - p1;
    if (chord.length() < 1e-10f) return false;
    ofVec2f perp_chord(-chord.y, chord.x);           // perpendicular bisector direction
    return s_line_intersect(p1, perp_t, mid, perp_chord, center);
}

// True when the arc travelling from 'point' (with that tangent) around
// 'center' is clockwise.
static bool s_arc_is_clockwise(ofVec2f point, ofVec2f center, ofVec2f tangent)
{
    ofVec2f r = point - center;
    // cross(r, tangent) < 0  →  tangent is to the right of r  →  CW
    return (r.x*tangent.y - r.y*tangent.x) < 0.f;
}

// ---------------------------------------------------------------------------
// Single biarc fit
// ---------------------------------------------------------------------------
struct BiArcFit { GArc arc1, arc2; bool valid = false; };

static BiArcFit s_fit_biarc(ofVec2f p1, ofVec2f t1, ofVec2f p2, ofVec2f t2)
{
    BiArcFit result;

    // --- join point G = incenter of triangle (P1, P2, V) -----------------
    ofVec2f V;
    bool have_V = s_line_intersect(p1, t1, p2, t2, V);

    // Guard against near-parallel tangents (V flies to infinity)
    bool use_mid = !have_V
                || (V - p1).length() > 1e5f
                || std::isnan(V.x) || std::isnan(V.y);

    ofVec2f G;
    if (use_mid) {
        G = (p1 + p2) * 0.5f;
    } else {
        float d_p2v  = (p2 - V).length();
        float d_p1v  = (p1 - V).length();
        float d_p1p2 = (p1 - p2).length();
        float perim  = d_p2v + d_p1v + d_p1p2;
        if (perim < 1e-10f) return result;   // degenerate triangle
        G = (p1*d_p2v + p2*d_p1v + V*d_p1p2) / perim;
    }

    // --- arc 1: P1 → G, tangent T1 at P1 ----------------------------------
    ofVec2f C1;
    bool ok1 = s_arc_center(p1, t1, G, C1);
    float R1  = ok1 ? (p1 - C1).length() : 0.f;
    bool cw1  = ok1 ? s_arc_is_clockwise(p1, C1, t1) : false;

    result.arc1.start     = p1;
    result.arc1.end       = G;
    result.arc1.center    = C1;
    result.arc1.radius    = ok1 ? R1 : 0.f;
    result.arc1.clockwise = cw1;

    // --- arc 2: G → P2, tangent T2 at P2 ----------------------------------
    ofVec2f C2;
    bool ok2 = s_arc_center(p2, t2, G, C2);
    float R2  = ok2 ? (p2 - C2).length() : 0.f;
    bool cw2  = ok2 ? s_arc_is_clockwise(p2, C2, t2) : false;

    result.arc2.start     = G;
    result.arc2.end       = p2;
    result.arc2.center    = C2;
    result.arc2.radius    = ok2 ? R2 : 0.f;
    result.arc2.clockwise = cw2;

    result.valid = true;
    return result;
}

// ---------------------------------------------------------------------------
// Hausdorff distance from B(t) on [t0, t1] to a full circle (C, R).
//
// The one-sided Hausdorff distance is:
//
//   max_{t ∈ [t0,t1]}  |dist(B(t), C) - R|
//
// The interior extrema of dist(B(t), C) occur where its derivative is zero:
//
//   d/dt dist(B(t), C)  =  (B(t) - C) · B'(t)  /  dist(B(t), C)  =  0
//
//   ⟹  (B(t) - C) · B'(t)  =  0          (degree-5 polynomial)
//
// We find all roots in [t0, t1] by scanning for sign changes at N_COARSE
// equally-spaced samples, then bisecting each bracket to 16 iterations.
// The maximum radial error over endpoints + all critical points is returned.
// ---------------------------------------------------------------------------
static float s_hausdorff_bezier_circle(
    ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2,
    float t0, float t1,
    ofVec2f C, float R)
{
    // Radial error at a given parameter
    auto err_at = [&](float t) -> float {
        return fabsf((s_bezier_eval(p1, c1, c2, p2, t) - C).length() - R);
    };

    // Value of (B(t)-C)·B'(t) — zero at critical points of dist(B(t), C)
    auto radial_dot = [&](float t) -> float {
        ofVec2f r  = s_bezier_eval(p1, c1, c2, p2, t) - C;
        ofVec2f dp = s_bezier_deriv(p1, c1, c2, p2, t);
        return r.x*dp.x + r.y*dp.y;
    };

    const int N_COARSE = 32;
    float max_err  = MAX(err_at(t0), err_at(t1));   // always check endpoints
    float prev_t   = t0;
    float prev_dot = radial_dot(t0);

    for (int i = 1; i <= N_COARSE; i++) {
        float t   = t0 + (t1 - t0) * (float)i / (float)N_COARSE;
        float dot = radial_dot(t);
        max_err   = MAX(max_err, err_at(t));

        if (prev_dot * dot < 0.f) {
            // Sign change → critical point in (prev_t, t): bisect to refine
            float lo = prev_t, hi = t;
            for (int k = 0; k < 16; k++) {
                float mid = (lo + hi) * 0.5f;
                if (radial_dot(mid) * prev_dot < 0.f) hi = mid;
                else                                  lo = mid;
            }
            max_err = MAX(max_err, err_at((lo + hi) * 0.5f));
        }

        prev_dot = dot;
        prev_t   = t;
    }
    return max_err;
}

// ---------------------------------------------------------------------------
// Hausdorff error of a biarc fit against the true Bezier.
//
// Arc 1 covers the first half of the curve parameter range [0, 0.5],
// arc 2 covers [0.5, 1].  The split at 0.5 is an approximation of the
// true parameter value at the biarc join point G; it is tight enough in
// practice because recursive subdivision keeps each segment small.
//
// Degenerate (straight-line) arcs fall back to point-to-segment distance.
// ---------------------------------------------------------------------------
static float s_biarc_error(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2,
                            const BiArcFit& fit)
{
    // Helper: max distance from Bezier samples on [ta, tb] to a line segment
    auto line_error = [&](float ta, float tb, ofVec2f A, ofVec2f B) -> float {
        float max_d = 0.f;
        ofVec2f seg = B - A;
        float seg_len = seg.length();
        for (int i = 0; i <= 8; i++) {
            float t = ta + (tb - ta) * (float)i / 8.f;
            ofVec2f P = s_bezier_eval(p1, c1, c2, p2, t);
            float d;
            if (seg_len < 1e-8f) {
                d = (P - A).length();
            } else {
                ofVec2f dv = seg * (1.f / seg_len);
                float proj = ofClamp((P - A).dot(dv), 0.f, seg_len);
                d = (P - (A + dv * proj)).length();
            }
            if (d > max_d) max_d = d;
        }
        return max_d;
    };

    float err1 = fit.arc1.isLine()
        ? line_error(0.f, 0.5f, fit.arc1.start, fit.arc1.end)
        : s_hausdorff_bezier_circle(p1, c1, c2, p2, 0.f, 0.5f,
                                    fit.arc1.center, fit.arc1.radius);

    float err2 = fit.arc2.isLine()
        ? line_error(0.5f, 1.f, fit.arc2.start, fit.arc2.end)
        : s_hausdorff_bezier_circle(p1, c1, c2, p2, 0.5f, 1.f,
                                    fit.arc2.center, fit.arc2.radius);

    return MAX(err1, err2);
}

// ---------------------------------------------------------------------------
// Recursive subdivision
// ---------------------------------------------------------------------------
static void s_bezier_to_biarcs(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2,
                                float tolerance, int depth,
                                vector<GArc>& out)
{
    // Skip zero-length segments
    if ((p1 - p2).length() < 0.001f) return;

    if (depth <= 0) {
        // Max recursion reached: fall back to a straight line segment
        GArc a;
        a.start = p1;  a.end = p2;  a.radius = 0.f;
        out.push_back(a);
        return;
    }

    ofVec2f t1 = s_bezier_tangent(p1, c1, c2, p2, 0.f);
    ofVec2f t2 = s_bezier_tangent(p1, c1, c2, p2, 1.f);

    BiArcFit fit = s_fit_biarc(p1, t1, p2, t2);

    if (!fit.valid) {
        GArc a;
        a.start = p1;  a.end = p2;  a.radius = 0.f;
        out.push_back(a);
        return;
    }

    if (s_biarc_error(p1, c1, c2, p2, fit) <= tolerance) {
        out.push_back(fit.arc1);
        out.push_back(fit.arc2);
    } else {
        // De Casteljau subdivision at t = 0.5
        ofVec2f m01   = (p1 + c1)   * 0.5f;
        ofVec2f m12   = (c1 + c2)   * 0.5f;
        ofVec2f m23   = (c2 + p2)   * 0.5f;
        ofVec2f m012  = (m01 + m12) * 0.5f;
        ofVec2f m123  = (m12 + m23) * 0.5f;
        ofVec2f m0123 = (m012 + m123) * 0.5f;

        s_bezier_to_biarcs(p1,    m01,  m012,  m0123, tolerance, depth-1, out);
        s_bezier_to_biarcs(m0123, m123, m23,   p2,    tolerance, depth-1, out);
    }
}

// ---------------------------------------------------------------------------
// Public static: decompose Bezier → biarcs
// ---------------------------------------------------------------------------
vector<GArc> ofxGCode::bezier_to_biarcs(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2,
                                         float tolerance, int max_depth)
{
    vector<GArc> out;
    s_bezier_to_biarcs(p1, c1, c2, p2, tolerance, max_depth, out);
    return out;
}

// ---------------------------------------------------------------------------
// bezier_arc() — draw a Bezier using biarcs
// ---------------------------------------------------------------------------
void ofxGCode::bezier_arc(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, float tolerance)
{
    // Transform control points to screen space (honours ofTranslate/Scale/Rotate)
    ofVec2f tp1 = getModelPoint(p1);
    ofVec2f tc1 = getModelPoint(c1);
    ofVec2f tc2 = getModelPoint(c2);
    ofVec2f tp2 = getModelPoint(p2);

    // --- Preview: add linearised curve to lines so draw() works as usual ---
    {
        vector<ofVec2f> pnts = get_bezier_pnts(tp1, tc1, tc2, tp2, 20);
        for (int i = 0; i < (int)pnts.size()-1; i++) {
            ofVec2f a = pnts[i], b = pnts[i+1];
            if (clip.clip(a, b)) {
                GLine l;
                l.set(a, b);
                lines.push_back(l);
            }
        }
    }

    // --- Arc segments for the G2/G3 save pipeline -------------------------
    vector<GArc> arcs = bezier_to_biarcs(tp1, tc1, tc2, tp2, tolerance);
    for (const GArc& a : arcs) {
        GSegment seg;
        if (a.isLine()) {
            seg.type = GSegment::Type::Line;
        } else {
            seg.type = a.clockwise ? GSegment::Type::ArcCW : GSegment::Type::ArcCCW;
        }
        seg.start  = a.start;
        seg.end    = a.end;
        seg.center = a.center;
        segments.push_back(seg);
    }
}

// ---------------------------------------------------------------------------
// arc() — add a single circular arc directly
// ---------------------------------------------------------------------------
void ofxGCode::arc(ofVec2f start, ofVec2f end, ofVec2f center, bool clockwise)
{
    ofVec2f ts = getModelPoint(start);
    ofVec2f te = getModelPoint(end);
    ofVec2f tc = getModelPoint(center);

    // Linearised preview → lines
    {
        vector<ofVec2f> pnts = get_arc_points_ijk(ts, te, tc, clockwise, 32);
        for (int i = 0; i < (int)pnts.size()-1; i++) {
            ofVec2f a = pnts[i], b = pnts[i+1];
            if (clip.clip(a, b)) {
                GLine l;
                l.set(a, b);
                lines.push_back(l);
            }
        }
    }

    // Arc segment for G2/G3 output
    GSegment seg;
    seg.type   = clockwise ? GSegment::Type::ArcCW : GSegment::Type::ArcCCW;
    seg.start  = ts;
    seg.end    = te;
    seg.center = tc;
    segments.push_back(seg);
}

// ---------------------------------------------------------------------------
// save_arcs() — pen-plotter save with G2/G3 arc commands
// ---------------------------------------------------------------------------
void ofxGCode::save_arcs(string name)
{
    const float ipp = 1.0f / pixels_per_inch;

    vector<string> commands;
    commands.push_back("M3 S0");
    commands.push_back("G0 X0 Y0");

    ofVec2f last_pos(0.f, 0.f);
    bool pen_is_down = false;

    for (const GSegment& seg : segments) {
        ofVec2f s(seg.start.x  * ipp, seg.start.y  * ipp);
        ofVec2f e(seg.end.x    * ipp, seg.end.y    * ipp);
        ofVec2f c(seg.center.x * ipp, seg.center.y * ipp);

        // Rapid to start of this segment if needed
        if (s != last_pos) {
            if (pen_is_down) {
                commands.push_back("M3 S0");
                pen_is_down = false;
            }
            commands.push_back("G0 X" + ofToString(s.x, 4) + " Y" + ofToString(s.y, 4));
        }

        // Pen down if not already
        if (!pen_is_down) {
            commands.push_back("M3 S" + ofToString(pen_down_value));
            pen_is_down = true;
        }

        // The move itself
        if (seg.type == GSegment::Type::Line) {
            commands.push_back("G1 X" + ofToString(e.x, 4) + " Y" + ofToString(e.y, 4));
        } else {
            // I, J are the arc-centre offset from the current position
            float I = c.x - s.x;
            float J = c.y - s.y;
            string cmd = (seg.type == GSegment::Type::ArcCW) ? "G2" : "G3";
            commands.push_back(cmd
                + " X" + ofToString(e.x, 4)
                + " Y" + ofToString(e.y, 4)
                + " I" + ofToString(I, 4)
                + " J" + ofToString(J, 4));
        }

        last_pos = e;
    }

    commands.push_back("M3 S0");
    commands.push_back("G0 X0 Y0");

    ofLogNotice("ofxGCode") << "save_arcs: " << commands.size() << " commands";

    ofFile file;
    file.open(name, ofFile::WriteOnly);
    for (const string& cmd : commands) file << cmd << "\n";

    ofLogNotice("ofxGCode") << "save_arcs: saved " << name;
}

