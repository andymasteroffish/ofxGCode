#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(250);
    
    //setup the gcode object
    gcode.setup();
    
    //set our initial drawing settings
    gcode.show_do_not_reverse = true;
    gcode.show_transit_lines = true;
    gcode.show_path_with_color = true;
    
    //some rules to use when refreshing
    set_do_not_reverse_on_edges = true;
    sort_lines = true;

    need_refresh = false;
    
    //setup our drawing
    refresh();
}

//--------------------------------------------------------------
//refresh function that will clear the drawing and draw a new one
void ofApp::refresh(){
    
    //clear any existing drawing
    gcode.clear();
    
    //draw a bunch of random lines
    float padding = 40;
    for (int i=0; i<600; i++){
        //random starting point
        float x1 = ofRandom(-padding, ofGetWidth()+padding );
        float y1 = ofRandom(-padding, ofGetHeight()+padding );
        
        //select a point a set distance but random angle away
        float length = 30;
        float angle = ofRandom(0, TWO_PI);
        
        //create a line
        float x2 = x1 + cos(angle) * length;
        float y2 = y1 + sin(angle) * length;
        gcode.line(x1,y1, x2,y2);
    }
    
    //set lines on the edge to only draw from the inside out so that the pen does not catch on the edge of the paper
    //lines on the edge will be oriented to go outward and will have do_not_reverse set
    if (set_do_not_reverse_on_edges){
        ofRectangle safe_zone;
        safe_zone.x = 20;
        safe_zone.y = 20;
        safe_zone.width = ofGetWidth() - 40;
        safe_zone.height = ofGetHeight() - 40;
        gcode.set_outwards_only_bounds(safe_zone);
    }
    
    //sort the drawing to reduce travel time
    if (sort_lines){
        gcode.sort();
    }

    need_refresh = false;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    //preview the drawing
    gcode.draw();
    
    //some info about what we're doing
    int x = 70;
    int y = 70;
    int y_spacing = 14;
    
    //fade out rectangle
    ofSetColor(0, 180);
    ofDrawRectangle(x-8, y-8, 320, 170);
    
    //the text
    ofSetColor(255);
    if (need_refresh){
        ofSetColor(255, 0, 0);
    }
    ofDrawBitmapString("N - refresh image", x, y+=y_spacing);
    
    y+=5;
    
    ofSetColor(255);
    ofDrawBitmapString("Keys to change preview settings", x, y+=y_spacing);
    
    string path_setting = gcode.show_path_with_color ? "true" : "false";
    ofDrawBitmapString(" C - show path with color: " + path_setting, x, y+=y_spacing);
    
    string transit_setting = gcode.show_transit_lines ? "true" : "false";
    ofDrawBitmapString(" T - show transit lines  : " + transit_setting, x, y+=y_spacing);
    
    string do_not_reverse_setting = gcode.show_do_not_reverse ? "true" : "false";
    ofDrawBitmapString(" R - show do not reverse : " + do_not_reverse_setting, x, y+=y_spacing);
    
    y+=5;
    
    ofDrawBitmapString("Keys to change refresh settings", x, y+=y_spacing);
    
    string sort_setting = sort_lines ? "true" : "false";
    ofDrawBitmapString(" S - sort lines: " + sort_setting, x, y+=y_spacing);
    
    string edge_setting = set_do_not_reverse_on_edges ? "true" : "false";
    ofDrawBitmapString(" e - outward only on edges: " + edge_setting, x, y+=y_spacing);
    
    if (need_refresh){
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("refresh for changes to take effect", x, y+=y_spacing);
    }
        
    y+=5;
    ofSetColor(255);
    ofDrawBitmapString("Press space to save drawing", x, y+=y_spacing);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    //refreshing the drawing
    if (key == 'n'){
        refresh();
    }
    
    //toggling showing the path order with color
    //if true, the lines will go from green to blue with green parts being drawn first
    if (key == 'c'){
        gcode.show_path_with_color = !gcode.show_path_with_color;
    }
    
    //toggling showing transit lines
    //these lines show the movement where the pen will be up
    if (key == 't'){
        gcode.show_transit_lines = !gcode.show_transit_lines;
    }
    
    //toggling showing outwards only lines
    //lines with the do_not_reverse flag will be shown in pink with a small arrow showing their direction
    if (key == 'r'){
        gcode.show_do_not_reverse = !gcode.show_do_not_reverse;
    }
    
    //changing the settings
    //these will only take effect once you refresh
    
    if (key == 's'){
        sort_lines = !sort_lines;
        need_refresh = true;
    }
    if (key == 'e'){
        set_do_not_reverse_on_edges = !set_do_not_reverse_on_edges;
        need_refresh = true;
    }
    
    
    //saving
    if (key == ' '){
        gcode.save("lines.nc");
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
