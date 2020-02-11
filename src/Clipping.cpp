//
//  Clipping.cpp
//  vectrex_test
//
//  Created by Andrew Wallace on 4/8/19. Based on code by Trammel Hudson.
//

#include "Clipping.hpp"

void Clipping::setup(ofVec2f p0, ofVec2f p1) {
    min.set(MIN(p0.x, p1.x), MIN(p0.y, p1.y));
    max.set(MAX(p0.x, p1.x), MAX(p0.y, p1.y));
}

int Clipping::compute_code(ofVec2f p) {
    int code = INSIDE;
    
    if (p.x < min.x)
        code |= LEFT;
    if (p.x > max.x)
        code |= RIGHT;
    if (p.y < min.y)
        code |= BOTTOM;
    if (p.y > max.y)
        code |= TOP;
    
    return code;
}

float Clipping::intercept(float y, float x0, float y0, float x1, float y1) {
    return x0 + (x1 - x0) * (y - y0) / (y1 - y0);
}

// Clip a line segment from p0 to p1 by the
// rectangular clipping region min/max.
// p0 and p1 will be modified to be in the region
// returns true if the line segment is visible at all
bool Clipping::clip(ofVec2f &p0, ofVec2f &p1) {
    int code0 = compute_code(p0);
    int code1 = compute_code(p1);
    
    while (true) {
        // both are inside the clipping region.
        // accept them as is.
        if ((code0 | code1) == 0)
            return true;
        
        // both are outside the clipping region
        // and do not cross the visible area.
        // reject the point.
        if ((code0 & code1) != 0)
            return false;
        
        // At least one endpoint is outside
        // the region.
        int code = code0 != 0 ? code0 : code1;
        float x = 0, y = 0;
        
        if ((code & TOP) != 0) {
            // point is above the clip rectangle
            y = max.y;
            x = intercept(y, p0.x, p0.y, p1.x, p1.y);
        } else if ((code & BOTTOM) != 0) {
            // point is below the clip rectangle
            y = min.y;
            x = intercept(y, p0.x, p0.y, p1.x, p1.y);
        } else if ((code & RIGHT) != 0) {
            // point is to the right of clip rectangle
            x = max.x;
            y = intercept(x, p0.y, p0.x, p1.y, p1.x);
        } else if ((code & LEFT) != 0) {
            // point is to the left of clip rectangle
            x = min.x;
            y = intercept(x, p0.y, p0.x, p1.y, p1.x);
        }
        
        // Now we move outside point to intersection point to clip
        // and get ready for next pass.
        if (code == code0) {
            p0.x = x;
            p0.y = y;
            code0 = compute_code(p0);
        } else {
            p1.x = x;
            p1.y = y;
            code1 = compute_code(p1);
        }
    }
}

bool Clipping::check_point(ofVec2f pnt){
    return compute_code(pnt) == INSIDE;
}
