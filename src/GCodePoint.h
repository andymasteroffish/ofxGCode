//
//  GCodePoint.h
//  gcode_lib_test
//
//  Created by Andrew Wallace on 11/25/19.
//

#ifndef GCodePoint_h
#define GCodePoint_h

class GCodePoint{
public:
    
    float x;
    float y;
    float speed;
    float pressure;
    
    bool do_not_reverse;
    
    inline GCodePoint( float _x, float _y, float _speed, float _pressure ):x(_x), y(_y), speed(_speed), pressure(_pressure) {
        do_not_reverse = false;
    }
    
    inline GCodePoint() {
        do_not_reverse = false;
    }
    
    void set_from_point(GCodePoint other){
        x = other.x;
        y = other.y;
        speed = other.speed;
        pressure = other.pressure;
        do_not_reverse = other.do_not_reverse;
    }
    
    //TODO: MAKE SURE THIS WORKS NOW THAT YOU TURNED IT TO FLOATS
    bool equals(GCodePoint other){
        return x==other.x && y==other.y && speed==other.speed && pressure==other.pressure;
    }
    
    bool equals_pos_only(GCodePoint other){
        return x==other.x && y==other.y;
    }
};


#endif /* GCodePoint_h */
