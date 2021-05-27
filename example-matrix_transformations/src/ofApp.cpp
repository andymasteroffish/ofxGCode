#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(250);
    
    //setup the drawing
    gcode.setup();
    
    //push & pop can be used outside of draw
    //as long as you only use 2D transformations, the lines you add will be affected by your matrix
    //it should behave more or less identically to how you would normally use them
    
    for (int i=0; i<7; i++){
        ofPushMatrix();
        
        //adjusting the scale, rotation & position
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofRotateRad(i * 0.1);
        ofScale(1 + i * 0.5);
        
        //drawing a square centered on the origin
        gcode.rect(-50, -50, 100, 100);
        
        ofPopMatrix();
    }
    
    //optimize the plot and save
    gcode.sort();
    gcode.save("plot.nc");

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    //demo the drawing
    gcode.draw();
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
