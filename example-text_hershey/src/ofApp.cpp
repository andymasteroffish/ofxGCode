#include "ofApp.h"

/*
 This example uses the Hershey Font tool build into ofxGCode
 This is an extra class you need to include in a project that is using it
 
 The Hershey Font was created in 1967 by Dr. Allen V. Hershey
 It uses single strokes to write text, which makes it perfect for things like plotters
 
 This Hershey Text class is a modified version of ofxHersheyFont by Tobias Zimmer
 https://github.com/tobiaszimmer/ofxHersheyFont
 
 That library uses the 'Simplex' characters of the Hershey Font (ASCII codes 32 – 126), which were made available by Paul Bourke.
 http://paulbourke.net/dataformats/hershey/
 
 A lot of folks have helped to make this possible! Thank you everybody!
*/

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(250);
    
    //setup our drawing
    gcode.setup();
    
    //create a hershey text object
    ofxHersheyFont hershey;
    
    //write some text!
    //the arguments are as follows:
    //  string   - the text you want to write
    //  float    - x position of the text
    //  float    - y position of the text
    //  scale    - how big to draw the text
    //  ofxGCode - the gcode you want to draw into, passed as a pointer
    hershey.draw("#PlotterTwitter", 100, 70, 2, &gcode);
    
    //A scale of 1 means a capital H will be 21 pixels tall
    //I'm using a scale of 2 here, so a capital will be 42 pixels tall
    
    //there are a number of drawing options
    //Take a look at ofxHersheyFont.h for an explanation of all the drawing functions
    
    //You can provide a set width which will cause the text to wrap when it hits that point
    float max_width = 200;
    hershey.draw("This is a long enough line that it will get split up.", 20, 130, 0.75, &gcode, max_width);
    
    //you can center the text
    hershey.draw("Centered", 300, 300, 2, true, &gcode);
    
    //you can center and rotate
    hershey.draw("Rotated", 150, 450, 2, true, PI/4, &gcode);
    
    //you can get the width and height of some text in case you need the bounding box
    //you need to pass the scale in to these functions so make sure you are using the same value when you draw it
    string sample_text = "#PlotterTwitter";
    float sample_scale = 1.5;
    float text_width = hershey.getWidth(sample_text, sample_scale);
    float text_height = hershey.getCapitalHeight(sample_scale);           //we only need the scale because the text is a set height
    
    //draw the text
    float x_pos = 220;
    float y_pos = 400;
    hershey.draw(sample_text, x_pos, y_pos, sample_scale, &gcode);
    
    //and a box around it
    float padding = 10;
    gcode.rect(x_pos-padding, y_pos-text_height-padding, text_width+padding*2, text_height+padding*2);
    
    
    //optimize and save
    gcode.sort();
    gcode.save("hershey.nc");
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
