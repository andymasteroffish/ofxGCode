#include "ofApp.h"


/*
 This example shows how you can use the static functions to generate shapes and trim lines
 This is often useful for more complex plots where you want to trim lines before adding them to the drawing
 A common reason why I do this is because I am masking different shapes and do not want to trim the lines already in the drawing
 
 In this example, I am drawing three circles, and in each one I want vertical stripes
 For each circle I need to trim the vertical stripes to be inside, but I do not want to affect the lines that were added in previous circles
 */

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(250);
    
    //setup the gcode
    gcode.setup();
    
    //a loop to go through and create multiple circles with stripes
    for (int i=0; i<3; i++){
        
        //the center of the circle
        ofVec2f circle_pos;
        circle_pos.x = 150+i*150;
        circle_pos.y = 150+i*150;

        //the size of the circle
        float circle_size = 100;
        
        //get the points that make up the circle
        //doing this instead of just drawing the circle because I will want to use these same points when trimming
        vector<ofVec2f> circle_pnts = ofxGCode::get_circle_pnts(circle_pos, circle_size, 70);
        //the last argument in the above code is the circle resolution: how many points will make up the circle
        
        //draw the circle
        gcode.polygon(circle_pnts);
        
        //create a vector of GLines for the vertical lines
        //At its core a GLine just stores two points to describe a line. It has a lot of useful functions built in though
        //All lines inside of ofxGCode are stored as GLines
        //check GLine.h for more detailed info
        vector<GLine> lines;
        
        //the lines get closer with each circle
        float spacing = 18 - i * 5;
        
        //go through and make vertical lines across the whole screen
        //(the trim process would be more efficient if we only drew lines that would be over the circle, but that's OK. One of the nice things about doing plotter art is that you can often afford to be a little inefficient)
        for (float x=0; x<ofGetWidth(); x += spacing){
            //create a vertical line
            GLine line = GLine(x, 0, x, ofGetWidth());
            //add it to the vector
            lines.push_back(line);
        }
        
        //at this point, the lines have not been added to the drawing, they only exist in the lines vector
        
        //I'm going to use the static trim function to trim any part of these lines outside of our circle
        //Because I do not care about keeping the original lines, I am overwriting my vector with the vector of GLines returned by this function
        //the static trim functions do not modify anything in our gcode drawing
        lines = ofxGCode::trim_lines_outside(lines, circle_pnts);
        
        //now that I've trimmed the lines, I will add them to my drawing
        gcode.add_lines(lines);
    }
    
    
    
    //optimize and save
    gcode.sort();
    gcode.save("trim_advanced.nc");

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
