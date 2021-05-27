#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(250);
    
    //call setup before drawing anything
    gcode.setup();  //no arguments means we're using the default 100px per inch
    
    //the size of the page will be set to the size of our sketch
    
    //unlike normal creative coding where we redraw every frame, we draw everything once
    //making a line will add it to the drawing
    //for this reason, you typically want to do your drawing in setup or in a function that gets called once
    
    //basic shapes
    gcode.circle(ofGetWidth()/2, ofGetHeight()/2, 100);
    
    gcode.rect(20,20, 100, 150);
    
    //lines
    for (int i=0; i<10; i++){

        //x1,y1 , x2,y2
        gcode.line(250, 100+i*20, 450, 100+i*20);
    }
    
    //polygons
    gcode.begin_shape();
    gcode.vertex(50, 300);
    gcode.vertex(130, 350);
    gcode.vertex(50, 400);
    gcode.end_shape(true);  //true here means the shape will be closed
    
    //if any part of a line is outside of the bounds it will be clipped
    //only a quarter of this circle is in bounds, so the rest will not be present in the final file
    gcode.circle(500,500, 100);
    
    
    //when you are done drawing you can optimize and save
    
    //sort() will do its best to reduce the travel time to draw your drawing
    //this will maintain all of the lines, but will change the order that they are drawn in
    //it will almost always speed up the plot, but if you have a specific order you want to draw your lines in you may want to skip this step
    //for drawings with thousands and thousands of lines, this may take a few seconds so you might not want to do this or save while you're still working out the design
    gcode.sort();

    //save will take your drawing and convert it to G-Code, saving it in the bin/data folder
    //You can use a site like https://ncviewer.com/ to test the output
    gcode.save("my_first_drawing.nc");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    //this will demo the drawing
    //you can change the color by modifying gcode.demo_col
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
