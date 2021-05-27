#include "ofApp.h"

/*
 This example shows how to load a font and draw text
 Be aware the computer fonts describe outlines
 This often looks bad for smaller text
 Use a single-stroke font like Hershey Text for something that will look better on a plotter
 */

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(250);
    
    //setup our drawing
    gcode.setup();
    
    //load the font
    //you must make sure makeContours is set to true (it's the 3rd bool argument here)
    //we need the contours to trace the outline
    //see https://openframeworks.cc//documentation/graphics/ofTrueTypeFont/#!show_load
    font.load("verdana.ttf", 50, true, true, true);
    
    
    //draw something
    //the font must be passed as a pointer
    gcode.text("#PlotterTwitter", &font, 20, 100);
    
    
    //you can use transformations to rotate or scale the text
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, 400);
    ofRotateDeg(20);
    
    float text_width = font.stringWidth("#PlotterTwitter");
    gcode.text("#PlotterTwitter", &font, -text_width/2, 10);
    
    ofPopMatrix();
    
    
    //sort and save
    gcode.sort();
    gcode.save("text.nc");
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    //preview
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
