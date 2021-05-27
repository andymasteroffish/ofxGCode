#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(250);
    
    //setup gcode
    gcode.setup();
    
    //default setting
    trim_mode = 0;
    
    //define our trim shapes
    //The trim functions can either take an ofRectangle or a vector of ofVec2f
    
    //square
    box.set(50,200,250,250);
    
    //polygon - making a blobby shape with noise
    polygon.clear();
    int num_pnts = 40;
    ofVec2f center = ofVec2f(300,200);
    for (int i=0; i<num_pnts; i++){
        float angle = i * (TWO_PI/num_pnts);
        float dist = 70 + ofNoise(angle) * 90;
        ofVec2f point;
        point.x = center.x + cos(angle) * dist;
        point.y = center.y + sin(angle) * dist;
        polygon.push_back(point);
    }
    
    
    //draw with the default settings
    refresh();
}

//--------------------------------------------------------------
//reset function to clear and redraw the gcode
//the drawing will change based on what we're trimming
void ofApp::refresh(){
    
    //clear whatever might be in the drawing now
    gcode.clear();
    
    //diagonal stripes
    for (float x=0; x<ofGetWidth()*2; x+=20){
        gcode.line(x,0, x-ofGetWidth(),ofGetHeight());
    }
    
    //how do we want to trim?
    
    // 0 - no trim
    
    // 1 - trim inside the box
    if (trim_mode == 1){
        gcode.trim_inside(box);
    }
    
    // 2 - trim outside the box
    if (trim_mode == 2){
        gcode.trim_outside(box);
    }
    
    // 3 - trim inside the polygon
    if (trim_mode == 3){
        gcode.trim_inside(polygon);
    }
    
    // 4 - trim outside the polygon
    if (trim_mode == 4){
        gcode.trim_outside(polygon);
    }
    
    // some combos - note: combining trim calls will always remove the maximum amount
    
    // 5 - trim inside both shapes
    if (trim_mode == 5){
        gcode.trim_inside(box);
        gcode.trim_inside(polygon);
    }
    
    // 6 - trim outside both shapes
    if (trim_mode == 6){
        gcode.trim_outside(box);
        gcode.trim_outside(polygon);
    }
    
    // 7 - trim inside the box and outside the polygon
    if (trim_mode == 7){
        gcode.trim_inside(box);
        gcode.trim_outside(polygon);
    }
    
    // 8 - trim outside the box and inside the polygon
    if (trim_mode == 8){
        gcode.trim_outside(box);
        gcode.trim_inside(polygon);
    }
    
    //sort and save
    //every time you refresh, it will overwrite the previous saved file
    gcode.sort();
    gcode.save("trim.nc");

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    //preview the drawing
    gcode.draw();
    
    //show the shapes (these are not part of the gcode drawing)
    
    ofNoFill();
    ofSetColor(255,100,100);
    
    //the box
    ofDrawRectangle(box);
    
    //the polygon
    ofBeginShape();
    for (int i=0; i<polygon.size(); i++){
        ofVertex(polygon[i]);
    }
    ofEndShape(true);
    
    //little faded box to write some text
    ofSetColor(255, 150);
    ofFill();
    ofDrawRectangle(10, 10, 280, 17);
    
    //some info about how to use this
    ofSetColor(0);
    string info = "0-8 to try different trim settings";
    ofDrawBitmapString(info, 12, 22);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    //changing the trim mode
    if (key == '0') trim_mode = 0;
    if (key == '1') trim_mode = 1;
    if (key == '2') trim_mode = 2;
    if (key == '3') trim_mode = 3;
    if (key == '4') trim_mode = 4;
    if (key == '5') trim_mode = 5;
    if (key == '6') trim_mode = 6;
    if (key == '7') trim_mode = 7;
    if (key == '8') trim_mode = 8;
    
    //redo the drawing
    refresh();
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
