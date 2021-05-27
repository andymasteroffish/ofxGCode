#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofBackground(250);
    
    //all ofxGCode objects need to be setup
    for(int i=0; i<NUM_COLS; i++){
        gcode[i].setup();
    }
    
    //changing demo_col lets you tell them apart
    gcode[0].demo_col.set(255,0,0);     //red
    gcode[1].demo_col.set(0, 0, 255);   //blue
    
    //draw as you would normally, just be mindful of which gcode you're drawing into
    gcode[0].rect(50,50,500,500);
    gcode[1].rect(60,60,480,480);
    
    //some more shapes
    for (int i=0; i<=5; i++){
        int color = i%2;
        int pos = 150 + 60*i;
        gcode[color].circle(pos,pos, 70);
    }
    
    //sort and save, producing two .nc files
    for(int i=0; i<NUM_COLS; i++){
        gcode[i].sort();
        gcode[i].save("multicolor"+ofToString(i)+".nc");
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    //draw the previews
    for(int i=0; i<NUM_COLS; i++){
        gcode[i].draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
