//
//  ofxGCode
//
//  Created by Andrew Wallace on 11/7/19.
//

#include "ofxGCode.hpp"

void ofxGCode::setup(float _pixels_per_inch){
    //set some defaults
    max_speed = 10000;
    speed = 5000;
    pressure = 60;
    circle_resolution = 50;
    
    //inches for axidraw
    pixels_per_inch = _pixels_per_inch;
    
    show_transit_lines = true;
    show_path_with_color = true;
    show_do_not_reverse = false;
    do_not_draw_dots = false;
    
    set_size(ofGetWidth(), ofGetHeight());
    
    debug_show_point_numbers = false;
    
    demo_col.set(0,0,0);
}

void ofxGCode::set_size(int w, int h){
    clip.setup(ofVec2f(0, 0), ofVec2f(w,h));
    
    fbo.allocate(w,h, GL_RGBA);
    
    clear();
}

void ofxGCode::clear(){
    fbo.begin();
    ofClear(255,255,255);
    ofSetColor(255);
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
    fbo.end();
    
    list.clear();
    
    last_x = -9999;
    last_y = -9999;
    last_speed = -9999;
    last_pressure = -9999;
    
    commands.clear();
}

void ofxGCode::draw(int max_lines_to_show){
    //ofSetColor(255);
    //fbo.draw(0,0);
    
    int draw_count = 0;
    
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
            bool is_dot = prev_x == pnt.x && prev_y == pnt.y && pnt.pressure > 0;
            
            if (do_not_draw_dots)   is_dot = false;
            
            if (!is_dot){
                ofDrawLine(prev_x, prev_y, pnt.x, pnt.y);
            }else{
                ofDrawCircle(pnt.x, pnt.y, 1);
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
}

void ofxGCode::generate_gcode(){
    commands.clear();
    
    commands.push_back("M3 S0");
    commands.push_back("G0 X0 Y0");
    
    ofVec2f prev_pnt = ofVec2f(0,0);
    float prev_speed = max_speed;
    float prev_pressure = 0;
    
    float inches_per_pixel = 1.0 / pixels_per_inch;
    cout<<"inches per pixel "<<inches_per_pixel<<endl;
    
    for (int i=0; i<list.size(); i++){
        GCodePoint pnt = list[i];
        //ofVec2f plotter_pos = screen_point_to_plotter(list[i].x,list[i].y);
        ofVec2f plotter_pos = ofVec2f(list[i].x * inches_per_pixel, list[i].y * inches_per_pixel);
        
        
        string move_command = "";
        //if pen is up, go as fast as possible
        if (pnt.pressure == 0){
            move_command = "G0 X"+ofToString(plotter_pos.x)+" Y"+ofToString(plotter_pos.y) ;
        }
        //otherwise move at the specified speed
        else{
            //move_command = "G1 F"+ofToString(pnt.speed)+"\n";   //not sure that this is working
            move_command += "G1 X"+ofToString(plotter_pos.x)+" Y"+ofToString(plotter_pos.y);
        }
        
        //make sure that we're not just lifting the pen up and putting it back down
        bool skip = false;
        if (commands.size() > 2){
            if (pnt.pressure < 1){
                //cout<<"check "<<move_command.substr(2)<<" VS "<<commands[commands.size()-1].substr(2)<<endl;
                if(move_command.substr(2) == commands[commands.size()-1].substr(2)){
                    skip = true;
                    //cout<<"fuck this skip "<<move_command<<endl;
                }
            }
        }
        
        if (!skip){
            //write the pressure command first
            if (pnt.pressure != prev_pressure){
                commands.push_back("M3 S"+ofToString(pnt.pressure));
            }
            
            //just make sure that this is not identical to the last one
            if (move_command != commands[commands.size()-1]){
                commands.push_back(move_command);
            }else{
                //cout<<"duplicate: "<<move_command<<endl;
            }
            
            
            
            prev_pnt.set(plotter_pos);
            prev_speed = pnt.speed;
            prev_pressure = pnt.pressure;
        }
    }
    
    //add some closing steps
    commands.push_back("M3 S0");
    commands.push_back("G0 X0 Y0");
    
    cout<<"transit distance: "<<measureTransitDistance()<<endl;
}

//ofVec2f ofxGCode::screen_point_to_plotter(float x, float y){
//    ofVec2f val(0,0);
//    val.x = ofMap(x,0,ofGetWidth(), -plotter_x_limit, plotter_x_limit);
//    val.y = ofMap(y,0,ofGetWidth(), plotter_y_limit, -plotter_y_limit);
//    return val;
//}

void ofxGCode::print(){
    for (int i=0; i<commands.size(); i++){
        cout<<commands[i]<<endl;
    }
}



void ofxGCode::save(string name){
    generate_gcode();
    
    cout<<"saving "<<commands.size()<<" commands"<<endl;
    ofFile myTextFile;
    myTextFile.open(name,ofFile::WriteOnly);
    for (int i=0; i<commands.size(); i++){
        myTextFile<<commands[i]<<endl;
    }
    
    cout<<"SAVED"<<endl;
}

void ofxGCode::set_pressure(float val){
    if (val < 0 || val > 100){
        cout<<"speed shuld be between 0 and 100"<<endl;
        return;
    }
    pressure = val;
}
void ofxGCode::set_speed(float val){
    if (val < 1 || val > max_speed){
        cout<<"speed should be between 1 and "<<max_speed<<endl;
        return;
    }
    speed = val;
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

void ofxGCode::begin_shape(){
    shape_pnts.clear();
}
void ofxGCode::start_shape(){
    cout<<"HEY DON'T USE THIS. start_shape IS A BAD NAME AND I SHOULD REMOVE IT. USE begin_shape"<<endl;
    begin_shape();
}
void ofxGCode::vertex(ofVec2f p){
    shape_pnts.push_back(p);
}
void ofxGCode::vertex(float x, float y){
    shape_pnts.push_back(ofVec2f(x,y));
}

// TODO: make this just add points instead of using line
// right now every point is added twice because you make a bunch of lines
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

void ofxGCode::line(GLine _line){
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
    
    if (!clip.clip(p1, p2)) {
        //cout<<"no part of this line is on screen"<<endl;
        return;
    }
    
    fbo.begin();
    
    float speed_prc = ofMap(speed, 1000, max_speed, 1, 0);  //setting an arbitrary min
    speed_prc = powf(speed_prc, 0.5);                       //smoothing this out since a medium speed still looks pretty black
    ofSetColor(0, speed_prc * 255);
    ofDrawLine(p1.x, p1.y, p2.x, p2.y);
    fbo.end();
    
    if (lift_pen){
        point(p1.x, p1.y, speed, 0);
    }else{
        point(p1.x, p1.y, speed, pressure);
    }
    point(p2.x, p2.y, speed, pressure);
}

void ofxGCode::dot(float x, float y){
    ofVec2f pnt = getModelPoint(x,y);
    
    if (clip.check_point(pnt) == false){
        return;
    }
    
    fbo.begin();
    ofSetColor(0);
    ofDrawCircle(pnt.x, pnt.y, 1);
    fbo.end();
    
    //transit line
    point(x, y, speed, 0);
    //drop the pen
    point(x, y, speed, pressure);
    
}

void ofxGCode::point(float x, float y, float speed, float pressure){
    
    if (x == last_x && y == last_y && last_speed == speed && last_pressure == pressure ) {
        //cout<<"same point as last. ignored. "<<x<<","<<y<<endl;
        return;
    }
    last_x = x;
    last_y = y;
    last_speed = speed;
    last_pressure = pressure;
    
    //cout<<"adding at "<<x<<","<<y<<endl;
    
    GCodePoint pnt = GCodePoint(x,y,speed,pressure);
    list.push_back(pnt);
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
                line(polylines[j][k].x,polylines[j][k].y, polylines[j][next_id].x,polylines[j][next_id].y, k==0);
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
    //cout<<"---sorting---"<<endl;
    vector<GCodePoint> destination;
    destination.clear();
    vector<GCodePoint> src(list);    //clones the vector
    
    //in my ofxVST port, these were called frames
    GCodePoint lastFrame = GCodePoint(9999, 9999, 0, 0);
    GCodePoint nearestFrame = lastFrame;
    
    while (src.size() != 0) {
        int startIndex = 0;
        int endIndex = 0;
        float nearestDistance = 100000;
        int i = 0;
        bool reverseOrder = false;
        
        while (i < src.size()) {
            int j = i;
            while (j < src.size() - 1 && src[j + 1].pressure > 0) {
                j++;
            }
            
            //cout<<"go from "<<i<<" to "<<j<<endl;
            
            GCodePoint startFrame = src[i];
            GCodePoint endFrame = src[j];    // j = index of inclusive right boundary
            float startDistance = ofDist(lastFrame.x, lastFrame.y, startFrame.x, startFrame.y);
            float endDistance = ofDist(lastFrame.x, lastFrame.y, endFrame.x, endFrame.y);
            
            if (startDistance < nearestDistance) {
                startIndex = i;
                endIndex = j;
                nearestDistance = startDistance;
                nearestFrame = startFrame;
                reverseOrder = false;       //if this fixes it, add it to your vector code
                //cout<<"less "<<i<<"  "<<j<<endl;
            }
            if (!startFrame.equals(endFrame) && endDistance < nearestDistance) {
                startIndex = i;
                endIndex = j;
                nearestDistance = endDistance;
                nearestFrame = endFrame;
                reverseOrder = true;
            }
            i = j + 1;
            //cout<<"i is now "<<i<<endl;
        }
        
        //cout<<"start index "<<startIndex<<"  end index "<<endIndex<<endl;
        
        
        //reverseOrder = false;   //andy edit
        GCodePoint startFrame = src[startIndex];
        GCodePoint endFrame = src[endIndex];
        
        if (startFrame.do_not_reverse){
            //cout<<"I am told not to revers "<<startFrame.x<<","<<startFrame.y<<endl;
            reverseOrder = false;
        }
        
        if (reverseOrder) {
            //cout<<"reverse the one at "<<startFrame.x<<" , "<<startFrame.y<<endl;
            // Swap commands of first and last segment if in reverse order
            // THIS NEEDED TO BE POINTERS to actually change the data
            GCodePoint * t0 = &src[startIndex];
            GCodePoint * t1 = &src[endIndex];
            int temp_pressure = t0->pressure;
            int temp_speed = t0->speed;
            t0->pressure = t1->pressure;
            t0->speed = t1->speed;
            t1->pressure = temp_pressure;
            t1->speed = temp_speed;
            
            lastFrame = startFrame;
            for (int index = endIndex; index >= startIndex; index--) {
                //cout<<"add reverse point "<<src[index].x<<","<<src[index].y<<endl;
                destination.push_back(src[index]);
            }
        } else {
            lastFrame = endFrame;
            for (int index = startIndex; index <= endIndex; index++) {
                //cout<<"add point "<<src[index].x<<","<<src[index].y<<endl;
                destination.push_back(src[index]);
            }
        }
        
        //cout<<"last frame  x: "<<lastFrame.x<<" y: "<<lastFrame.y<<endl;
        
        src.erase(src.begin()+startIndex, src.begin()+endIndex + 1);
    }
    
    list.clear();
    for (int i=0; i<destination.size(); i++){
        list.push_back(destination[i]);
    }
}

//void ofxEleksDrawremove_duplicate_points(float max_dist_to_count = 0){
//
//}

//trying to go thorugh and find lines that secretly connect
//TODO: have it consider speed
void ofxGCode::simplify(float max_dist_to_combine_points){
    float max_dist_sq = powf(max_dist_to_combine_points, 2);
    int test_count = 0;
//    cout<<"--start list--"<<endl;
//    for (int i=0; i<list.size(); i++){
//        cout<<i<<":"<<list[i].x<<","<<list[i].y<<"  "<<list[i].pressure<<endl;
//    }
    
    vector<GCodePoint> destination;
    destination.clear();
    vector<GCodePoint> src(list);
    
    int start_id = 0;
    
    //pulling out all multi-line segments (like rectanges) because I don't want to think about how to test them just yet
    while (start_id < src.size()-1){
        int end_id = start_id;
        while (end_id < src.size()-1 && src[end_id+1].pressure > 0){
            end_id++;
        }
        
        //cout<<"from "<<start_id<<" to "<<end_id<<endl;
        
        //multiline segments should just be added to the final list.
        if (end_id > start_id+1){
            for (int index = start_id; index <= end_id; index++) {
                //cout<<"add point "<<src[index].x<<","<<src[index].y<<endl;
                destination.push_back(src[index]);
            }
            //cout<<"start: "<<start_id<<"  end: "<<end_id<<endl;
            src.erase(src.begin()+start_id, src.begin()+end_id + 1);
            cout<<"found a multiline starting at "<<start_id<<endl;
        }
        //keep single line segments in src
        else{
            start_id = end_id+1;
        }
    }
    
    
    for (int index = 0; index<src.size(); index++){
        //cout<<"index "<<index<<endl;
        GCodePoint pnt_a = src[index];
        
    
       
        for (int k=index+1; k<src.size(); k++){
            //cout<<" k "<<k<<endl;
            GCodePoint pnt_b = src[k];
            //cout<<"check "<<pnt_a.x<<","<<pnt_a.y<<"  against  "<<pnt_b.x<<","<<pnt_b.y<<endl;
            
            
            if (ofDistSquared(pnt_a.x, pnt_a.y, pnt_b.x, pnt_b.y) <= max_dist_sq){
            
            //if (pnt_a.equals_pos_only(pnt_b)){
                //cout<<"match at "<<pnt_b.x<<","<<pnt_b.y<<endl;
                //cout<<"match "<<index<<" and "<<k<<endl;
                
                //style 1
                //if A is a pen down move and B is pen up, then we can just nix B and bring its next point over
                if (pnt_a.pressure > 0 && pnt_b.pressure == 0){
                    GCodePoint end_pnt = src[k+1];
                    cout<<"connect style(1) "<<end_pnt.x<<","<<end_pnt.y<<endl;
                    
                    //add the sequence to destination
                    if (index-1 < 0)    cout<<"BIG TROUBLE!!"<<endl;
                    destination.push_back( src[index-1] );
                    destination.push_back(pnt_a);
                    destination.push_back(end_pnt);

                    //remove both segments
                    src.erase(src.begin()+k, src.begin()+k+2);
                    src.erase(src.begin()+index-1, src.begin()+index+1);
                    //put the down segment back directly after index
                    //src.insert(src.begin()+index+1, end_pnt);
                    
                    test_count++;
                    break;
                }
                
                //style 2
                //if A is pen up and B is also pen up
                if (pnt_a.pressure == 0 && pnt_b.pressure == 0){
                    cout<<"connect style 2"<<endl;
                    
                    //flip the first line segment
                    int next_index = index+1;
                    GCodePoint * t0 = &src[index];
                    GCodePoint * t1 = &src[next_index];
                    int temp_x = t0->x;
                    int temp_y = t0->y;
                    t0->x = t1->x;
                    t0->y = t1->y;
                    t1->x = temp_x;
                    t1->y = temp_y;
                    
                    //in the next sweep this should be caught as a style 1
                    //index = 0;
                    
                    break;
                }
                
                //style 3
                //if A is pen down and B is also pen down
                if (pnt_a.pressure > 0 && pnt_b.pressure > 0){
                    cout<<"connect style 3 on "<<index<<endl;
                    
                    //flip the second line segment
                    GCodePoint * t0 = &src[k];
                    GCodePoint * t1 = &src[k-1]; //need to go back since pnt_b is the down position
                    int temp_x = t0->x;
                    int temp_y = t0->y;
                    t0->x = t1->x;
                    t0->y = t1->y;
                    t1->x = temp_x;
                    t1->y = temp_y;
                    
                    //need to rewind a bit so that in the next sweep this should be caught as a style 1
                    index--;
                    //index = 0;
                    break;
                }
                
                //style 4
                //if A is pen up and B is pen down
                if (pnt_a.pressure == 0 && pnt_b.pressure > 0){
                    cout<<"conect style 4"<<endl;
                    
                    //flip the first line segment
                    int next_index = index+1;
                    GCodePoint * t0 = &src[index];
                    GCodePoint * t1 = &src[next_index];
                    int temp_x = t0->x;
                    int temp_y = t0->y;
                    t0->x = t1->x;
                    t0->y = t1->y;
                    t1->x = temp_x;
                    t1->y = temp_y;
                    
                    //flip the second segment
                    t0 = &src[k];
                    t1 = &src[k-1];    //need to go back since pnt_b is the down position
                    temp_x = t0->x;
                    temp_y = t0->y;
                    t0->x = t1->x;
                    t0->y = t1->y;
                    t1->x = temp_x;
                    t1->y = temp_y;
                    
                    //in the next sweep this should be caught as a style 1
                    //index = 0;
                    break;
                }
                
            }
        }
        
    }
    
    
    //add what's left
    for (int i=0; i<src.size(); i++){
        destination.push_back(src[i]);
    }
    
    //transfer it over
    //cout<<"--list--"<<endl;
    list.clear();
    for (int i=0; i<destination.size(); i++){
        //cout<<i<<":"<<destination[i].x<<","<<destination[i].y<<"  "<<destination[i].pressure<<endl;
        list.push_back(destination[i]);
    }
    
    cout<<"lines simplified: "<<test_count<<endl;
}

float ofxGCode::measureTransitDistance(){
    float distance = 0.0;
    GCodePoint last = GCodePoint(0, 0, 0, 0);
    for (GCodePoint f : list) {
        if (f.pressure == 0){
            distance += ofDist(f.x, f.y, last.x, last.y);
        }
        last = f;
    }
    return distance;
}

//this would be more efficient if you used Trammel's clipping class
//this thing is wildly inefficient
void ofxGCode::clip_inside(ofRectangle bounding_box){
    
    //get a list of points for the bounds
    vector<ofVec2f> bounds;
    bounds.push_back( ofVec2f(bounding_box.x,bounding_box.y));
    bounds.push_back( ofVec2f(bounding_box.x+bounding_box.width, bounding_box.y));
    bounds.push_back( ofVec2f(bounding_box.x+bounding_box.width, bounding_box.y+bounding_box.height));
    bounds.push_back( ofVec2f(bounding_box.x, bounding_box.y+bounding_box.height));
    
    //this should really just be a helper for the polygon funciton
    clip_inside(bounds);
    
    
//    //go through all points to see if they make up a line that passes through the bounds
//    //this probably will not work with weird shapes that create more than 2 intersections
//    for (int i=0; i<list.size()-1; i++){
//        GCodePoint * pnt = &list[i];
//        GCodePoint * next = &list[i+1];
//
//        //we only care if the pen is down
//        if (next->pressure > 0){
//            vector<ofPoint> intersects = find_intersections(*pnt, *next, bounds);
//            if (intersects.size() >= 2){
//
//                //figure out which intersect is closer to eahc point
//                ofPoint close_to_pnt = intersects[0];
//                ofPoint close_to_next = intersects[0];
//                float min_dist_to_pnt = ofDistSquared(close_to_pnt.x, close_to_pnt.y, pnt->x, pnt->y);
//                float min_dist_to_next = ofDistSquared(close_to_pnt.x, close_to_pnt.y, next->x, next->y);
//
//                for (int k=1; k<intersects.size(); k++){
//                    float dist_to_pnt = ofDistSquared(intersects[k].x, intersects[k].y, pnt->x, pnt->y);
//                    float dist_to_next = ofDistSquared(intersects[k].x, intersects[k].y, next->x, next->y);
//                    if (dist_to_pnt < min_dist_to_pnt){
//                        close_to_pnt = intersects[k];
//                        min_dist_to_pnt = dist_to_pnt;
//                    }
//                    if (dist_to_next < min_dist_to_next){
//                        close_to_next = intersects[k];
//                        min_dist_to_next = dist_to_next;
//                    }
//                }
//
//                //insert them as new points
//                GCodePoint new_pnt = GCodePoint(close_to_pnt.x,close_to_pnt.y, next->speed, next->pressure);
//                GCodePoint new_next = GCodePoint(close_to_next.x,close_to_next.y, next->speed, 0);
//
//                list.insert(list.begin()+i+1, new_next);
//                list.insert(list.begin()+i+1, new_pnt);
//            }
//        }
//    }
//
//    //go though all points to see if they make up a line partially inside the bounds
//    for (int i=0; i<list.size(); i++){
//        GCodePoint * pnt = &list[i];
//        if (bounding_box.inside(pnt->x, pnt->y)){
//
//            //if pen was down to get here, we need to consider what came before
//            bool preserve_point = false;
//            if (pnt->pressure > 0 && i > 0){
//                GCodePoint prev = list[i-1];
//
//                if (!bounding_box.inside(prev.x, prev.y)){
//                    ofPoint intersect = find_intersection(*pnt, prev, bounds);
//                    //move this point to the intersect
//                    if (intersect.x != -1){
//                        pnt->x = intersect.x;
//                        pnt->y = intersect.y;
//                        preserve_point = true;
//                    }else{
//                        //if we fucked up, just lift up the pen
//                        pnt->pressure = 0;
//                    }
//                }
//            }
//
//            //what comes next?
//            if (i < list.size()-1){
//                GCodePoint * next = &list[i+1];
//                //if the next one is also inside, we can just kill this one
//                //also if the next point is a pen-up move
//                if (!preserve_point && (bounding_box.inside(next->x, next->y) || next->pressure ==0) ){
//                    list.erase(list.begin()+i);
//                    i--;
//                }
//                //if it is outside we should clip
//                else if (!preserve_point && !bounding_box.inside(next->x, next->y)){
//                    ofPoint intersect = find_intersection(*pnt, *next, bounds);
//                    //move this point to the intersect
//                    if (intersect.x != -1){
//                        pnt->x = intersect.x;
//                        pnt->y = intersect.y;
//                    }
//                }
//                else{
//                    next->pressure = 0;
//                }
//            }
//        }
//    }
    
}

void ofxGCode::clip_inside(vector<ofVec2f> bounds){
    if (list.size() < 2){
        return;
    }
    
    //go through all points to see if they make up a line that passes through the bounds
    //this probably will not work with weird shapes that create more than 2 intersections
    for (int i=0; i<list.size()-1; i++){
        GCodePoint * pnt = &list[i];
        GCodePoint * next = &list[i+1];
        
        //we only care if the pen is down
        if (next->pressure > 0){
            vector<ofPoint> intersects = find_intersections(*pnt, *next, bounds);
            if (intersects.size() >= 2){
                
                //cout<<"double intersect"<<endl;
                
                //figure out which intersect is closer to each point
                ofPoint close_to_pnt = intersects[0];
                ofPoint close_to_next = intersects[0];
                float min_dist_to_pnt = ofDistSquared(close_to_pnt.x, close_to_pnt.y, pnt->x, pnt->y);
                float min_dist_to_next = ofDistSquared(close_to_pnt.x, close_to_pnt.y, next->x, next->y);
                
                for (int k=1; k<intersects.size(); k++){
                    float dist_to_pnt = ofDistSquared(intersects[k].x, intersects[k].y, pnt->x, pnt->y);
                    float dist_to_next = ofDistSquared(intersects[k].x, intersects[k].y, next->x, next->y);
                    if (dist_to_pnt < min_dist_to_pnt){
                        close_to_pnt = intersects[k];
                        min_dist_to_pnt = dist_to_pnt;
                    }
                    if (dist_to_next < min_dist_to_next){
                        close_to_next = intersects[k];
                        min_dist_to_next = dist_to_next;
                    }
                }
                
                //insert them as new points
                GCodePoint new_pnt = GCodePoint(close_to_pnt.x,close_to_pnt.y, next->speed, next->pressure);
                GCodePoint new_next = GCodePoint(close_to_next.x,close_to_next.y, next->speed, 0);
                
                list.insert(list.begin()+i+1, new_next);
                list.insert(list.begin()+i+1, new_pnt);
            }
        }
    }
    
    //go though all points to see if they make up a line partially inside the bounds
    //cout<<"size: "<<list.size()<<endl;
    for (int i=0; i<list.size(); i++){
        GCodePoint * pnt = &list[i];
        //cout<<"--checking "<<i<<"  pres: "<<pnt->pressure<<"   x: "<<pnt->x<<"  y: "<<pnt->y<<endl;
        if (checkInPolygon(bounds, pnt->x, pnt->y)){
            //cout<<"inside at "<<pnt->x<<" , "<<pnt->y<<endl;
            
            //if pen was down to get here, we need to consider what came before
            bool preserve_point = false;
            if (pnt->pressure > 0 && i > 0){
                GCodePoint prev = list[i-1];
                
                if (!checkInPolygon(bounds, prev.x, prev.y)){
                    //cout<<"bad boy "<<i<<" at "<<pnt->x<<","<<pnt->y<<endl;
                    ofPoint intersect = find_intersection(*pnt, prev, bounds);
                    //move this point to the intersect
                    if (intersect.x != -1){
                        //cout<<"  move to "<<intersect.x<<" "<<intersect.y<<endl;
                        pnt->x = intersect.x;
                        pnt->y = intersect.y;
                        preserve_point = true;
                    }else{
//                        cout<<"  we fucked up at "<<i<<endl;
//                        cout<<"  pnt "<<pnt->x<<","<<pnt->y<<endl;
//                        cout<<"  prev "<<prev.x<<","<<prev.y<<endl;
                        //if we fucked up, just lift up the pen
                        pnt->pressure = 0;
                    }
                }
            }
            
            //what comes next?
            if (i < list.size()-1){
                GCodePoint * next = &list[i+1];
                //if the next one is also inside, we can just kill this one
                //also if the next point is a pen-up move
                if (!preserve_point && (checkInPolygon(bounds, next->x, next->y) || next->pressure ==0) ){
                    //cout<<"kill "<<i<<" at "<<list[i].x<<" , "<<list[i].y <<endl;
                    
                    //if pen was up to get here, we should lift the pen up for the next move
                    if (pnt->pressure == 0){
                        next->pressure = 0;
                    }
                    
                    list.erase(list.begin()+i);
                    i--;
                    //next->pressure = 0;
                }
                //if it is outside we should clip
                else if (!preserve_point && !checkInPolygon(bounds, next->x, next->y)){
                    //cout<<"next bad boy "<<i<<" at "<<next->x<<","<<next->y<<endl;
                    ofPoint intersect = find_intersection(*pnt, *next, bounds);
                    //move this point to the intersect
                    if (intersect.x != -1){
                        //cout<<"  move to "<<intersect.x<<" "<<intersect.y<<endl;
                        pnt->x = intersect.x;
                        pnt->y = intersect.y;
                        pnt->pressure = 0;
                    }else{
//                        cout<<"  late fucked up at "<<i<<endl;
//                        cout<<"  pnt "<<pnt->x<<","<<pnt->y<<endl;
//                        cout<<"  prev "<<next->x<<","<<next->y<<endl;
                    }
                }
                else{
                    //cout<<"setting pressure for "<<i<<" to 0"<<endl;
                    next->pressure = 0;
                }
                
            }
        }
    }
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
//any lines outside of this boun will be forced to draw from the center out.
void ofxGCode::set_outwards_only_bounds(ofRectangle safe_area){
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
    
    
    /*
    for (int i=0; i<list.size()-1; i++){
        GCodePoint * pnt_a = &list[i];
        GCodePoint * pnt_b = &list[i+1];
        
        bool next_segment_is_good = true;
        if (i<list.size()-2){
            next_segment_is_good = list[i+2].pressure == 0;
        }
        
        //this only works on lines, so we need to identify when we're looking at a single line segment
        if (pnt_a->pressure == 0 && pnt_b->pressure > 0 && next_segment_is_good){
            //cout<<"line from "<<pnt_a->x<<","<<pnt_a->y<<"  to "<<pnt_b->x<<","<<pnt_a->y<<endl;
            
            //is at least one point outside?
            if (!safe_area.inside(pnt_a->x, pnt_a->y) || !safe_area.inside(pnt_b->x, pnt_b->y)){
                //cout<<"  a point is outside"<<endl;
                
                //if they are in the right order, just keep them that way
                if (ofDistSquared(center.x, center.y, pnt_a->x, pnt_a->y) < ofDistSquared(center.x, center.y, pnt_b->x, pnt_b->y)){
                   // cout<<"  we are in the right direction"<<endl;
                    //do nothing
                }else{
                    //cout<<"  we need to flip"<<endl;
                    float a_x = pnt_a->x;
                    float a_y = pnt_a->y;
                   
                    pnt_a->x = pnt_b->x;
                    pnt_a->y = pnt_b->y;
                    pnt_b->x = a_x;
                    pnt_b->y = a_y;
                    

                    
                }
                
                //either way, thise line is not allowed to be reversed in sort()
                pnt_a->do_not_reverse = true;
                pnt_b->do_not_reverse = true;
                
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





