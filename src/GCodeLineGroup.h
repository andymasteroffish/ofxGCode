//
//  GCodeLineGroup.h
//  ofxgcode_testing
//
//  Created by Andrew Wallace on 11/3/20.
//

#ifndef GCodeLineGroup_h
#define GCodeLineGroup_h

#include "GLine.hpp"
#include "ofMain.h"

class GCodeLineGroup{
public:
    ofVec2f start_pos, end_pos;
    vector<GLine> lines;
    bool do_not_reverse;
    
    void clear(){
        lines.clear();
        do_not_reverse = false;
    }
    void add_to_front(GLine line){
        lines.insert( lines.begin(), line);
        start_pos = line.a;
        if (lines.size() == 1)  end_pos = line.b;
        
        if (line.do_not_reverse)    do_not_reverse = true;
    }
    
    void add_to_back(GLine line){
        lines.push_back(line);
        end_pos = line.b;
        if (lines.size() == 1)  start_pos = line.a;
        
        if (line.do_not_reverse)    do_not_reverse = true;
    }
    
    
};


#endif /* GCodeLineGroup_h */
