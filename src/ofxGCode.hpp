//
//  ofxGCode.hpp
//  Created by Andrew Wallace on 11/7/19.
//

//A fair chunk of code in this project was ported from the Processing VST library by Trammell Hudson
//https://github.com/osresearch/vst


#ifndef ofxGCode_hpp
#define ofxGCode_hpp

#include "ofMain.h"
#include "Clipping.hpp"
#include "GLine.hpp"
#include "GCodeLineGroup.h"

class ofxGCode{
public:
    
    void setup(float _pixels_per_inch = 100);
    void set_size(int w, int h);
    void clear();
    
    void draw(int max_lines_to_show = -1);
    
    void save(string name);
    
    void rect(ofRectangle box);
    void rect(float x, float y, float w, float h);
    
    void rounded_rect(ofRectangle rect, float corner_size, int corner_resolution=10);
    void rounded_rect(float x, float y, float w, float h, float corner_size, int corner_resolution=10);
    static vector<ofVec2f> get_rounded_pnts(ofRectangle rect, float corner_size, int corner_resolution);
    static vector<ofVec2f> get_rounded_pnts(float x, float y, float w, float h, float corner_size, int corner_resolution);
    
    void circle(ofVec2f center, float size);
    void circle(float x, float y, float size);
    static vector<ofVec2f> get_circle_pnts(ofVec2f center, float size, int steps);
    
    void begin_shape();
    void vertex(ofVec2f p);
    void vertex(float x, float y);
    void end_shape(bool close);
    
    void polygon(vector<ofVec2f> pnts, bool close_shape = true);
    
    void line(GLine _line);
    void line(ofVec2f a, ofVec2f b);
    void line(float x1, float y1, float x2, float y2);
    
    void add_lines(vector<GLine> new_lines);
    
    void thick_line(float x1, float y1, float x2, float y2, float spacing, int layers);
    void thick_line(ofVec2f base_a, ofVec2f base_b, float spacing, int layers);
    
    static vector<ofVec2f> resample_lines(vector<ofVec2f> src_pnts, float sample_dist, bool close_shape, int steps_per_point=100);
    
    static vector<GLine> pnts_to_lines(vector<ofVec2f> pnts, bool close);
    
    void bezier(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, int steps = 50);
    static vector<ofVec2f> get_bezier_pnts(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, int steps);
    
    void dot(float x, float y);
    
    void text(string text, ofTrueTypeFont * font, float x, float y);
    
    ofVec2f getModelPoint(ofVec3f pnt);
    ofVec2f getModelPoint(float x, float y);
    
    void sort();
    
    void unlock_lines();
    void lock_lines();
    
    float measureTransitDistance();
    
    //functions for trimming
    static vector<GLine> trim_lines_inside(vector<GLine> lines, vector<ofVec2f> bounds);
    void trim_inside(vector<ofVec2f> bounds);
    static vector<GLine> trim_lines_inside(vector<GLine> lines, ofRectangle bounds);
    void trim_inside(ofRectangle bounds);
    
    static vector<GLine> trim_lines_outside(vector<GLine> lines, vector<ofVec2f> bounds);
    void trim_outside(vector<ofVec2f> bounds);
    static vector<GLine> trim_lines_outside(vector<GLine> lines, ofRectangle bounds);
    void trim_outside(ofRectangle bounds);
    
    static vector<GLine> trim_intersecting_lines(vector<GLine> lines_to_trim, vector<GLine> static_lines);
    
    //used to demo an area by trimming to a box and translating to 0,0
    //I almost always want to do this based on two points, so those are the arguments
    void demo_trim(float x1, float y1, float x2, float y2, bool do_translate=true);
    
    //any lines outside of this bound will be forced to draw from the center out. 
    void set_outwards_only_bounds(ofRectangle safe_area);
    
    //moving the lines
    void translate(float x, float y);
    
    //some tools for saving/loading from files
    static vector<vector<ofVec2f>> load_outlines(string file_path);
    static vector<GLine> load_lines(string file_path);
    void save_lines(string file_path);
    
    //code is a modified version of code by Randolph Franklin
    //from http://paulbourke.net/geometry/insidepoly/
    static bool checkInPolygon(vector<ofVec2f> p, float x, float y);
    
    //other helpers
    static ofVec2f perspective_warp(ofVec2f orig_pnt, ofRectangle src_bounds, ofVec2f new_bounds[4], float x_curve = 1, float y_curve = 1);
    
    //these have been depricated
    void clip_outside(ofRectangle bounding_box);
    void clip_inside(ofRectangle bounding_box);
    void clip_inside(vector<ofVec2f> bounds);
    
    
    //VALUES
    
    float pixels_per_inch;    //converison unit
    
    int circle_resolution;
    int pen_down_value;
    
    vector<ofVec2f> shape_pnts; //used for begin_shape / end_shape
    
    vector<GLine> lines;
    
    Clipping clip;
    
    
    
    //showing info
    bool show_transit_lines;
    bool show_path_with_color;
    bool show_do_not_reverse;
    
    ofColor demo_col;
    float demo_fade_prc;
    
};

#endif /* ofxGCode.hpp */
