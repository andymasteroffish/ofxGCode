//
//  GLine.hpp
//
//  Created by Andrew Wallace on 2/16/20.
//

#ifndef GLine_hpp
#define GLine_hpp

#include "ofMain.h"

//helper class for dealing with lines
//A GLine is a line between two 2D points (a & b)

class GLine{
public:
    
    //----------------
    // Variables
    //----------------
    
    ///the two points that make up the line
    ofVec2f a, b;
    
    ///if true, this line will not be drawn
    bool skip_me;
    
    ///if true, this line will not be flipped in the sorting process.
    ///this is typically set automaticly in ofxGCode for things like set_outwards_only_bounds()
    bool do_not_reverse;
    
    ///if true, this line will not be trimmed
    bool is_locked;
    
    
    //----------------
    // Functions
    //----------------
    
    //--- Constructors
    
    /// initializes an empty GLine
    GLine();
    
    /// initializes a GLine with starting A and B points as ofVec2f
    GLine(ofVec2f _a, ofVec2f _b);
    
    /// intitalizes a GLine with starting A and B points as floats
    GLine(float x1, float y1, float x2, float y2);
    
    
    //--- Set functions
    //these functions overide the A and B values of the line
    //the line will keep other values (do_not_reverse etc)
    
    /// sets the A and B points to new values
    void set(ofVec2f _a, ofVec2f _b);
    
    /// sets the A and B points to new values
    void set(float x1, float y1, float x2, float y2);
    
    /// sets the A and B points to match another line
    void set(GLine other);
    
    /// sets is_locked. Locked lines will not be trimmed
    void set_locked(bool val);
    
    
    //--- Drawing
    
    /// draws the line to the screen. If the line has no distance, a small circle is drawn.
    /// This will use the current drawing settings (ofSetColor, ofSetLineWidth etc)
    /// This is for testing. Calling this will not add the line to the plot
    void draw();
    
    
    //--- Utilities
    //Some helpful functions that don't really fit into other catagories
    
    /// returns a new line with the same length and angle of the current line, but offset along X and Y. This will not change the line it is called on
    GLine get_offset(ofVec2f offset);
    
    /// Breaks the line into evenly spaced segments and returns them as a vector of GLines. No change is made to the line.
    vector<GLine> get_segments(int num_segments);
    
    /// returns the length of this line
    float get_length();
    
    /// Returns a vector of 4 points forming a box around the line that is padding distance from the line
    vector<ofVec2f> get_bounds(float padding);
    
    
    //--- Intersection
    
    /// returns true if this line intersects the other
    bool intersects(GLine other);
    
    /// returns true if this line intersects the other. If they do intersect and intersect_pnt is provided, it will be set the the x/y position of the intersection
    bool intersects(GLine other, ofVec2f &intersect_pnt);
    
    //--- Clipping
    //These functions cut a line to end where it touches another line
    
    /// if this line intersects other, the B position of this line will be moved to the intersection pooint
    bool clip_to_other_line(GLine other);
    
    /// if this line intersects the line described by other_a & other_b, the B position of this line will be moved to the intersection pooint
    bool clip_to_other_line(ofVec2f other_a, ofVec2f other_b);
    
    /// if this line intersects the line described by the four points, the B position of this line will be moved to the intersection pooint
    bool clip_to_other_line(float other_a_x, float other_a_y, float other_b_x, float other_b_y);
    
    /// Swaps the values of A and B on this line. The clipping functions always assume that the A point will stay the same and the B point will be moved, so this is useful to do if you want the A side to change
    void swap_a_and_b();
    
    
    //--- Trimming lines
    //These are mostly helper functions for ofxGCode::trim_lines_inside & ofxGCode::trim_lines_outside
    //If the line passes through the provided shape, it will be trimmed.
    //However, this sometimes requires that new lines are created (for instance, we trim a circle in the middle of the line, we now have a line on either side)
    //For this reason a list value is passed in where any new lines will be put.
    //of the trimming functions in ofxGCode will deal with this automatically
    
    /// trims all points of the line inside the rectangle. if a list is provided, any new lines that need to be created will be added there
    /// generally speaking, you should use ofxGCode::trim_lines_inside instead of calling this on an individual line
    void trim_inside(ofRectangle rect, vector<GLine>* list = NULL);
    /// trims all points of the line inside the polygon described by pnts. if a list is provided, any new lines that need to be created will be added there
    /// generally speaking, you should use ofxGCode::trim_lines_inside instead of calling this on an individual line
    void trim_inside(vector<ofVec2f> pnts, vector<GLine>* list = NULL);
    
    /// trims all points of the line inside the rectangle. if a list is provided, any new lines that need to be created will be added there
    /// generally speaking, you should use ofxGCode::trim_lines_outside instead of calling this on an individual line
    void trim_outside(ofRectangle rect, vector<GLine>* list = NULL);
    /// trims all points of the line outside of the polygon described by pnts. if a list is provided, any new lines that need to be created will be added there
    /// generally speaking, you should use ofxGCode::trim_lines_outside instead of calling this on an individual line
    void trim_outside(vector<ofVec2f> pnts, vector<GLine>* list = NULL);
    
    
    /// returns true if the point defined by x and y is inside the polygon defined by p
    bool checkInPolygon(vector<ofVec2f> p, float x, float y);
    
    /// returns true if point t is on the line defined by p1 & p2
    static bool check_point_on_line(ofVec2f t, ofVec2f p1, ofVec2f p2);
    
    
    
private:
    
    //I don't want this to show up in autocomplete. just call trim_inside_polygon or trim_outside_polygon.
    void trim_flexible(vector<ofVec2f> pnts, bool trim_inside, vector<GLine>* list = NULL);
    
    
};

#endif /* GLine_hpp */
