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
#include "GCodeLineGroup.h"

class ofxGCode{
public:
    
    void setup(float _pixels_per_inch = 100);
    void set_size(int w, int h);
    void clear();
    
    void draw(int max_lines_to_show = -1);
    
    //void generate_gcode();
    void save(string name);
    
    void rect(ofRectangle box);
    void rect(float x, float y, float w, float h);
    
    void circle(float x, float y, float size);
    
    void begin_shape();
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
    
    //void point(float x, float y, float speed, float pressure);
    
    void text(string text, ofTrueTypeFont * font, float x, float y);
    
    //void translate(float x, float y);
    
    ofVec2f getModelPoint(ofVec3f pnt);
    ofVec2f getModelPoint(float x, float y);
    
    void sort();
    void simplify(float max_dist_to_combine_points = 0);
    //void remove_duplicate_points(float max_dist_to_count = 0);
    
    float measureTransitDistance();
    
    //functions for trimming
    static vector<GLine> trim_lines_inside_polygon(vector<GLine> lines, vector<ofVec2f> bounds);
    void trim_inside_polygon(vector<ofVec2f> bounds);
    static vector<GLine> trim_lines_inside_box(vector<GLine> lines, ofRectangle bounds);
    void trim_inside_box(ofRectangle bounds);
    
    static vector<GLine> trim_lines_outside_polygon(vector<GLine> lines, vector<ofVec2f> bounds);
    void trim_outside_polygon(vector<ofVec2f> bounds);
    static vector<GLine> trim_lines_outside_box(vector<GLine> lines, ofRectangle bounds);
    void trim_outside_box(ofRectangle bounds);
    
    
    ofPoint find_intersection(GCodePoint a, GCodePoint b, vector<ofVec2f> bounds);
    vector<ofPoint> find_intersections(GCodePoint a, GCodePoint b, vector<ofVec2f> bounds);
    
    bool are_points_the_same(ofVec2f a, ofVec2f b);
    
    //any lines outside of this bound will be forced to draw from the center out. 
    void set_outwards_only_bounds(ofRectangle safe_area);
    
    //code is a modified version of code by Randolph Franklin
    //from http://paulbourke.net/geometry/insidepoly/
    bool checkInPolygon(vector<ofVec2f> p, float x, float y);
    
    //these have been depricated
    void clip_outside(ofRectangle bounding_box);
    void clip_inside(ofRectangle bounding_box);
    void clip_inside(vector<ofVec2f> bounds);
    
    
    //VALUES
    
    float pixels_per_inch;    //converison unit
    
    int circle_resolution;
    int pen_down_value;
    
    float max_dist_to_consider_pnts_touching;
    
    vector<ofVec2f> shape_pnts; //used for begin_shape / end_shape
    
    vector<GLine> lines;
    
    Clipping clip;
    
    bool debug_show_point_numbers;
    
    //showing info
    bool show_transit_lines;
    bool show_path_with_color;
    bool show_do_not_reverse;
    bool do_not_draw_dots;
    
    ofColor demo_col;
    
    //kill me
    vector<GCodeLineGroup> line_groups;
    
};

#endif /* ofxGCode.hpp */
