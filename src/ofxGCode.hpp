//
//  ofxGCode.hpp
//  Created by Andrew Wallace on 11/7/19.
//

//A fair chunk of code in this project was ported from the Processing VST library by Trammell Hudson
//https://github.com/osresearch/vst

//I'M REDOING THIS TO PLAY NICE WITH THE AXIDRAW, WHICH USES THE SAME COORDINATE PLANE AS oF!

#ifndef ofxGCode_hpp
#define ofxGCode_hpp

#include "ofMain.h"
#include "Clipping.hpp"
#include "GCodePoint.h"
#include "GLine.hpp"

class ofxGCode{
public:
    
    void setup(float _pixels_per_inch = 100);
    void set_size(int w, int h);
    void clear();
    
    void draw(int max_lines_to_show = -1);
    
    void generate_gcode();
    //ofVec2f screen_point_to_plotter(float x, float y);
    void print();
    void save(string name);
    
    void set_pressure(float val);       //currently not used
    void set_speed(float val);
    
    void rect(ofRectangle box);
    void rect(float x, float y, float w, float h);
    
    void circle(float x, float y, float size);
    
    void begin_shape();
    void start_shape();
    void vertex(ofVec2f p);
    void vertex(float x, float y);
    void end_shape(bool close);
    
    void polygon(vector<ofVec2f> pnts);
    
    void line(GLine _line);
    void line(ofVec2f a, ofVec2f b, bool lift_pen=false);
    void line(float x1, float y1, float x2, float y2);
    void line(float x1, float y1, float x2, float y2, bool lift_pen);
    
    void thick_line(float x1, float y1, float x2, float y2, float spacing, int layers);
    void thick_line(ofVec2f base_a, ofVec2f base_b, float spacing, int layers);
    
    void bezier(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, int steps = 50);
    
    void dot(float x, float y);
    
    void point(float x, float y, float speed, float pressure);
    
    void text(string text, ofTrueTypeFont * font, float x, float y);
    
    void translate(float x, float y);
    
    ofVec2f getModelPoint(ofVec3f pnt);
    ofVec2f getModelPoint(float x, float y);
    
    void sort();
    void simplify(float max_dist_to_combine_points = 0);
    //void remove_duplicate_points(float max_dist_to_count = 0);
    
    float measureTransitDistance();
    
    void clip_outside(ofRectangle bounding_box);
    
    void clip_inside(ofRectangle bounding_box);
    void clip_inside(vector<ofVec2f> bounds);
    ofPoint find_intersection(GCodePoint a, GCodePoint b, vector<ofVec2f> bounds);
    vector<ofPoint> find_intersections(GCodePoint a, GCodePoint b, vector<ofVec2f> bounds);
    
    //any lines outside of this bound will be forced to draw from the center out. 
    void set_outwards_only_bounds(ofRectangle safe_area);
    
    //code is a modified version of code by Randolph Franklin
    //from http://paulbourke.net/geometry/insidepoly/
    bool checkInPolygon(vector<ofVec2f> p, float x, float y);
    
    
    
    //float plotter_x_limit, plotter_y_limit;
    float pixels_per_inch;    //converison unit
    
    float speed;
    float max_speed;
    float pressure; //maybe getting rid of this
    
    int circle_resolution;
    
    vector<ofVec2f> shape_pnts;
    
    vector<string> commands;
    //ofFbo fbo;
    
    Clipping clip;
    
    int last_x; //TODO: make these floats and check if values are very, very close
    int last_y;
    int last_pressure;
    int last_speed;
    
    vector<GCodePoint> list;
    
    int last_translate_id;
    
    bool debug_show_point_numbers;
    
    //showing info
    bool show_transit_lines;
    bool show_path_with_color;
    bool show_do_not_reverse;
    bool do_not_draw_dots;
    
    ofColor demo_col;
    
};

#endif /* ofxGCode.hpp */
