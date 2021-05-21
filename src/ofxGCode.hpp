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

//ofxGCode is the core class of this library
//an ofxGCode object represents a single gcode file (typically one pass on the plotter)
//use multiple ofxGCode objects to create layered drawings (for multiple colors etc)

//The static functions are tools that can be used anywhere. They take all of the info they need and return some result (most often a vector of GLines)

class ofxGCode{
public:
    
    //----------------
    // Variables
    //----------------
    
    float pixels_per_inch;      //all the drawing tools use pixels (just like oF). This value is used to convert that to inches on the page.
    
    int circle_resolution;      //equivalent to the value set by ofSetCircleResolution()
    
    int pen_down_value;         //height value used when dropping the pen
    //NOTE: the axidraw python script I use currently ignores this value
    
    vector<ofVec2f> shape_pnts; //used for begin_shape / end_shape
    
    vector<GLine> lines;        //the collection of lines that make up this drawing
    
    Clipping clip;              //clipping mask to make sure we don't have lines out of bounds
    
    //showing info
    bool show_transit_lines;        //if true, the movement of the plotter head while the pen is up will be drawn
    bool show_path_with_color;      //if true, the line color will go from green to blue over the length of the plot. Useful to check optimization
    bool show_do_not_reverse;       //if true, lines with the do not reverse flag will be displayed along with arrows showing what direction they are going
    
    ofColor demo_col;           //the color used for drawing this gcode. Set this to whatever you want
    float demo_fade_prc;        //how faded the color will be. Typically overlapping pen storkes will appear darker so it is helpful have this be true in our display as well
    
    
    
    
    //----------------
    // Functions
    //----------------
    
    //--- Setup
    
    ///setup must be called before you can use a ofxGCode object. pixels_per_inch argument is optional and will default to 100 pixels per inch
    void setup(float _pixels_per_inch = 100);
    
    ///changes the canvas size. This is called in setup and you generally should not need to use it
    void set_size(int w, int h);
    
    ///clears all lines from this drawing
    void clear();
    
    
    //--- Demo Drawing
    
    ///draws a preview of the current lines to the screen using the demo_col and the various "show" variables.
    ///if max_lines_to_show is a possitive numberm the drawing will stop after that many lines. This can be useful to walk through your drawing
    void draw(int max_lines_to_show = -1);
    
    
    //--- Saving
    
    ///saves the file to the bin folder
    void save(string name);
    
    
    //--- Rectangles
    
    ///draws a rectangle with the given values
    void rect(ofRectangle box);
    ///draws a rectangle with the given values
    void rect(float x, float y, float w, float h);
    
    
    void rounded_rect(ofRectangle rect, float corner_size, int corner_resolution=10);
    ///draws a rectangle with rounded corners
    void rounded_rect(float x, float y, float w, float h, float corner_size, int corner_resolution=10);
    
    ///static function to get the points that make up a rounded rectangle
    ///these points are not evenly spaced
    static vector<ofVec2f> get_rounded_pnts(ofRectangle rect, float corner_size, int corner_resolution);
    ///static function to get the points that make up a rounded rectangle
    ///these points are not evenly spaced
    static vector<ofVec2f> get_rounded_pnts(float x, float y, float w, float h, float corner_size, int corner_resolution);
    
    
    //--- Circles
    
    ///draws a circle with the given values
    void circle(ofVec2f center, float size);
    ///draws a circle with the given values
    void circle(float x, float y, float size);
    ///static function to get the points that make up a circle. steps will be the resolution of the circle
    static vector<ofVec2f> get_circle_pnts(ofVec2f center, float size, int steps);
    
    
    //--- Polygons
    
    ///analogous to ofBeginShape()
    void begin_shape();
    ///analogous to ofVertex()
    void vertex(ofVec2f p);
    ///analogous to ofVertex()
    void vertex(float x, float y);
    ///analogous to ofEndShape()
    void end_shape(bool close);
    
    ///draws a polygon defined by a vector of points. if close_shape is true, the last pnt will be connected to the first
    void polygon(vector<ofVec2f> pnts, bool close_shape = true);
    
    
    //--- Lines
    ///draws a line that matches the GLine being passed in. (only the position will be used. Flag values will not be copied over)
    void line(GLine _line);
    ///draws a line with the given values
    void line(ofVec2f a, ofVec2f b);
    ///draws a line with the given values
    void line(float x1, float y1, float x2, float y2);
    
    ///adds multiple lines to the drawing
    void add_lines(vector<GLine> new_lines);
    
    ///creates a thick line by adding multiple close lines next to the defined line. spacing determines how far apart the extra lines will be and layers determines how many lines will be added
    void thick_line(float x1, float y1, float x2, float y2, float spacing, int layers);
    void thick_line(ofVec2f base_a, ofVec2f base_b, float spacing, int layers);
    
    //--- Line Tools
    
    ///takes a polygon defined as a vector of points and respampes it, returning another vector of points that should be the same shape but evenly spaced
    ///note, this translation is lossy
    static vector<ofVec2f> resample_lines(vector<ofVec2f> src_pnts, float sample_dist, bool close_shape, int steps_per_point=100);
    
    ///takes a vector of points and returns a vector of GLines that connect those points. if close is true, a final GLine will be created connecting the first and last point
    static vector<GLine> pnts_to_lines(vector<ofVec2f> pnts, bool close);
    
    
    //--- Bezier Curves
    
    ///draws a bezier curve using the given values. steps defines the number of points that will be used in the line
    void bezier(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, int steps = 50);
    ///static function that returns a vector of the ponts that make up a bezier curve with the given values. steps defines the number of points that will be used in the line
    static vector<ofVec2f> get_bezier_pnts(ofVec2f p1, ofVec2f c1, ofVec2f c2, ofVec2f p2, int steps);
    
    
    //--- Dot
    ///draws a line with 0 length. This may no longer work.
    void dot(float x, float y);
    
    //--- Font-based text
    ///draws text at the x and y position using the given font (which must be passed in as a pointer)
    void text(string text, ofTrueTypeFont * font, float x, float y);
    
    //--- Getting the screen point from inside a matrix
    //This was a long process of trial and error and only works in 2D
    //Almost all of the drawing functions call these at some point.
    //This is how we get the position when inside a matrix
    //there is probably a much better way to do this
    
    ///gets the position on the screen of a point, accounting for any 2D matrix transformations
    ofVec2f getModelPoint(ofVec3f pnt);
    ///gets the position on the screen of a point, accounting for any 2D matrix transformations
    ofVec2f getModelPoint(float x, float y);
    
    //--- Optimmizing the lines
    //When using a plotter, optimizing the path saves a lot of time
    //The sorting code comes from Trammell Hudson
    //https://github.com/osresearch/vst
    
    ///Takes all of the unlocked lines in the drawing and reorders them to try and produce the shortest possible travel distance
    void sort();
    
    ///unlocks all current lines in the drawing, allowing them to be trimmed or moved
    void unlock_lines();
    ///locks all current lines in the drawing, preventing them from being trimmed or moved
    void lock_lines();
    
    ///gets the total pen-down distance of the drawing
    float measureTransitDistance();
    
    //--- Trimming
    //These functions are used to mask the lines against a shape, removing parts of lines or removing the line entirely if it is inside or outside the shape (depending on the funciton called
    //They work but are not optimized and will slow things down if you do it a lot
    
    //The non-static version of these functions will perform the operation on all lines currently in the drawing
    
    //The static version will take a vector of lines to trim and will return a new vector of the trimmed lines. The original set of lines will not be changed
    //This is useful to trim sets of lines before adding them to the drawing (for instance, when there are already lines in the drawing that you do not want trimmed)
    
    ///takes a vector of source lines and a polygon defined by bounds. returns a  vector of lines with any line or line segment inside bounds removed
    static vector<GLine> trim_lines_inside(vector<GLine> lines, vector<ofVec2f> bounds);
    ///trims all lines of the drawing inside the given polygon
    void trim_inside(vector<ofVec2f> bounds);
    ///takes a vector of source lines and rectangle. returns a  vector of lines with any line or line segment inside bounds removed
    static vector<GLine> trim_lines_inside(vector<GLine> lines, ofRectangle bounds);
    ///trims all lines of the drawing inside the given rectangle
    void trim_inside(ofRectangle bounds);
    
    ///takes a vector of source lines and a polygon defined by bounds. returns a  vector of lines with any line or line segment outside bounds removed
    static vector<GLine> trim_lines_outside(vector<GLine> lines, vector<ofVec2f> bounds);
    ///trims all lines of the drawing outside the given polygon
    void trim_outside(vector<ofVec2f> bounds);
    ///takes a vector of source lines and rectangle. returns a  vector of lines with any line or line segment outside bounds removed
    static vector<GLine> trim_lines_outside(vector<GLine> lines, ofRectangle bounds);
    ///trims all lines of the drawing outside the given rectangle
    void trim_outside(ofRectangle bounds);
    
    ///static function that takes a vector of lines_to_trim and another vector of static_lines.  returns a copy of lines_to_trim that has been trimmed any time one of the lines intersected any of the lines in static_lines
    static vector<GLine> trim_intersecting_lines(vector<GLine> lines_to_trim, vector<GLine> static_lines);
    
    ///used to demo an area by trimming to a box and translating it to 0,0
    ///I almost always want to do this based on two points, so those are the arguments
    void demo_trim(float x1, float y1, float x2, float y2, bool do_translate=true);
    
    
    //--- Other tools
    
    ///Any lines outside of this bound will be forced to draw from the center out.
    ///This is useful when drawing right up to the edge of the page to make sure that the pen always moves from inside the page otwards the edge (instead of the other way around, which would cause the pen to catch on the edge of the paper)
    ///This will set do_not_reverse on the lines that are in the edge area
    void set_outwards_only_bounds(ofRectangle safe_area);
    
    ///moves all non-locked lines in the drawing by the specified x and y values
    void translate(float x, float y);
    
    ///static function that takes a point, a rectangle of the source area, and 4 points to map that source area to. returns a new point that has been warped into the new_bounds
    ///x_curve and y_curve are optional values that will apply an exponential curve when calculating the new point
    static ofVec2f perspective_warp(ofVec2f orig_pnt, ofRectangle src_bounds, ofVec2f new_bounds[4], float x_curve = 1, float y_curve = 1);
    
    
    //--- Saving / loading from files
    //this saves/load using a pretty wonky format
    //I only made it as an easy way to copy a bunch of line segments from one project to another
    //it was designed to be simple and won't work outside of this library
    
    ///loads a list of outlines
    static vector<vector<ofVec2f>> load_outlines(string file_path);
    ///saves a list of line points
    static vector<GLine> load_lines(string file_path);
    ///saves a list of line points
    void save_lines(string file_path);
    
    //--- other
    
    //code is a modified version of code by Randolph Franklin
    //from http://paulbourke.net/geometry/insidepoly/
    ///returns true if the point described by x & y is inside the polygon defined by p
    static bool checkInPolygon(vector<ofVec2f> p, float x, float y);
    
    
    
    
    
    
};

#endif /* ofxGCode.hpp */
