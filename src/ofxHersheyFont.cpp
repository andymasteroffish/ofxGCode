/*
*  ofxHersheyFont.cpp
*
*  Created by Tobias Zimmer, August 2016.
*  www.tobiaszimmer.net
*
*  Font originally developed by Dr. Allen V. Hershey in 1967.
*  Font vectors made available by Paul Bourke.
*  paulbourke.net/dataformats/hershey/
*  
*  A simple single line font for machines like CNC, Lasercutter, ...
*  Available characters: ASCII codes 32 to 126.
*
*/

#include "ofxHersheyFont.h"
#include "simplexCharacterSet.h"


//--------------------------------------------------------------
ofxHersheyFont::ofxHersheyFont(){
	color = ofColor(0);
    line_height = 35;//33;
}

//--------------------------------------------------------------
//moves on to the next line if string would hit the width
//returns height
//passing a null gcode value will just give you the height
float ofxHersheyFont::draw(string stringValue, float x, float y, float scale, ofxGCode * gcode, float width){
    //break it up into words
    vector<string> words;
    string cur_word = "";
    for (int i=0; i<stringValue.length(); i++){
        if (stringValue.at(i) == ' ' || stringValue.at(i) == '\n' ){
            if (cur_word.length() > 0){
                words.push_back(cur_word);
                cur_word = "";
            }
            //add new lines as their own word
            if (stringValue.at(i) == '\n'){
                words.push_back("\n");
            }
        }
        else{
            cur_word += stringValue.at(i);
        }
    }
    if (cur_word.length() > 0){
        words.push_back(cur_word);
    }
    
    //add those words until we run out of space
    float cur_y = y;
    string cur_line = "";
    for (int i=0; i<words.size(); i++){
        if (words[i] == "\n"){
            if (cur_line.length() > 1){
                if (gcode != NULL) draw(cur_line, x, cur_y, scale, gcode);
            }
            cur_y += line_height * scale * 1.5;
            cur_line = "";
        }else{
            //cout<<"check word "<<words[i]<<endl;
            float line_w = getWidth(cur_line, scale);
            float word_w = getWidth(" "+words[i], scale);
            
            if (line_w + word_w < width){
                if (cur_line.length() > 0)  cur_line+=" ";
                cur_line += words[i];
                //cout<<"cur line: "<<cur_line<<endl;
            }else{
                //cout<<"print: "<<cur_line<<endl;
                if (gcode != NULL) draw(cur_line, x, cur_y, scale, gcode);
                cur_y += line_height * scale;
                cur_line = words[i];
            }
        }
    }
    
    //print the last line if we have one
    if (cur_line.length() > 0){
        if (gcode != NULL) draw(cur_line, x, cur_y, scale, gcode);
        //cur_y += line_height * scale;
    }
    
    return cur_y;
    
}

//--------------------------------------------------------------
void ofxHersheyFont::draw(string stringValue, float xPos, float yPos, float scale, ofxGCode * gcode) {
	draw(stringValue, xPos, yPos, scale, false, 0, gcode);
}

//--------------------------------------------------------------
void ofxHersheyFont::draw(string stringValue, float xPos, float yPos, float scale, bool centered, ofxGCode * gcode) {
	draw(stringValue, xPos, yPos, scale, centered, 0, gcode);
}

//--------------------------------------------------------------
void ofxHersheyFont::draw(string stringValue, float xPos, float yPos, float scale, bool centered, float angle, ofxGCode * gcode) {
	
	float characterXPos = 0;
	float center = 0;
	if (centered) center = getWidth(stringValue, scale) / 2;

	ofPushMatrix();
	ofTranslate(xPos, yPos);
	ofRotateRad(angle);
	ofTranslate(-center, 0);
    
    int line_num = 0;
	
    //iterate through each character of the input string
    for (int i = 0; i < stringValue.size(); i++)
    {
        ofPushMatrix();
        ofTranslate(characterXPos, line_num*line_height*scale);
        ofScale(scale, scale);  //ofScale(scale, -scale);

        //get ascii value of specific character from the input string
        int asciiValue = stringValue.at(i);
        
        //cout<<"char "<<stringValue.at(i)<<endl;
        //cout<<"ASCII "<<asciiValue<<endl;
        
        //is this a symbol?
//        bool circle_symbol = false; //mana and such
//        if (stringValue.at(i) == '{'){
//            char symbol_char = stringValue.at(i+1);
//            if (symbol_char == 'W') asciiValue = SYMBOL_W;
//            else if (symbol_char == 'U') asciiValue = SYMBOL_U;
//            else if (symbol_char == 'B') asciiValue = SYMBOL_B;
//            else if (symbol_char == 'R') asciiValue = SYMBOL_R;
//            else if (symbol_char == 'G') asciiValue = SYMBOL_G;
//            else if (symbol_char == 'T') asciiValue = SYMBOL_TAP;
//            else if (symbol_char == 'P') asciiValue = SYMBOL_PHYREXIAN;
//            else {
//                circle_symbol = true;
//                asciiValue = symbol_char;
//            }
//            i+=2;   //skip to the next real character
//        }
        
        float symbol_offset_x = 6;
        
        if (asciiValue == '\n'){
            line_num++;
            characterXPos = 0;
        }
        /*
         In the project I inititaly made this for, I had the option to draw vector symbols. Maybe I'll bring it bakc at some point
        else if (asciiValue < NUM_SYMBOLS){
            //this was drawing a little off so I adjusted it. Magic numebrs!
            symbol_draw->draw(asciiValue, -symbol_offset_x, 5, false, default_symbol_size, gcode);
            characterXPos += symbol_spacing * default_symbol_size * scale;
            
            //if the next symbol is also a char, we need ot give it some extra spacing
            if (i < stringValue.length()-1){
                char next = stringValue.at(i+1);
                if (next == '{'){
                    characterXPos += symbol_offset_x;
                }
            }
        }
         */
        else{

            //if character is not available, use questionmark
            if (asciiValue < 32 || asciiValue > 126) asciiValue = 63;

            //draw the character
            drawChar(asciiValue, gcode);

            //update xPos / starting position for the next character
            float charWidth = simplex[asciiValue - 32][1] * scale;
            characterXPos += charWidth;
        }

        ofPopMatrix();
    }

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofxHersheyFont::drawChar(int asciiValue, ofxGCode * gcode) {
	gcode->begin_shape();

	//iterate through points of the character
   
	for (int i = 2; i <= simplex[asciiValue - 32][0] * 2; i += 2)
	{
		float x = simplex[asciiValue - 32][i];
		float y = simplex[asciiValue - 32][i + 1];

        if (x != -1){
            gcode->vertex(x, -y);
            
        }

		//skip -1,-1 value -> equals pen up operation / end of a line
		//and move to next point
		if (x == -1) {
            gcode->end_shape(false);

            gcode->begin_shape();
		}
	}
    gcode->end_shape(false);
}

//--------------------------------------------------------------
//void ofxHersheyFont::drawSymbol(int symbolValue, ofxGCode * gcode){
//    symbol_draw->draw(symbolValue, 0, 0, false, default_symbol_size, gcode);
//}


//--------------------------------------------------------------
void ofxHersheyFont::setColor(ofColor c) {
	color = c;
}

//--------------------------------------------------------------
float ofxHersheyFont::getWidth(string stringValue, float scale){
	float stringWidth = 0;
    float longest_string_width;
    
	for (int i = 0; i < stringValue.size(); i++)
	{
		int asciiValue = stringValue.at(i);
        if (asciiValue < 32 || asciiValue > 126) asciiValue = 63;
        
        if (stringValue.at(i) == '\n'){
            if (longest_string_width < stringWidth){
                longest_string_width = stringWidth;
            }
            stringWidth = 0;
        }
        else{
            stringWidth += (float)simplex[asciiValue - 32][1] * scale;
        }
	}
    
    if (longest_string_width < stringWidth){
        longest_string_width = stringWidth;
    }

	return longest_string_width;
}

//--------------------------------------------------------------
//count the number of newlines and multiply by line height
float ofxHersheyFont::getHeight(string stringValue, float scale){
    int lines = 1;
    for (int i=0; i<stringValue.length(); i++){
        if (stringValue.at(i) == '\n'){
            lines ++;
        }
    }
    cout<<"num lines "<<lines<<endl;
    return  line_height * lines * scale;
}

//--------------------------------------------------------------
float ofxHersheyFont::getHeight(float scale) {
	//the height of a capital letter is 21px (scale 1)
	float stringHeight = (float) 21 * scale;

	return stringHeight;
}

float ofxHersheyFont::getLineHeight(float scale) {
    return line_height * scale;
}


