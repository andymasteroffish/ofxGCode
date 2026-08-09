#include "ofApp.h"

// ============================================================================
//  example-bezier_arcs
//
//  Demonstrates bezier_arc() — a biarc approximation of cubic Bezier curves
//  that produces G2/G3 circular-arc commands instead of many short G1 lines.
//
//  The screen preview looks identical for both methods; the difference is
//  visible by opening the two saved .nc files in a G-code viewer such as
//  https://ncviewer.com/
//
//  Key API:
//      gcode.bezier(p1, c1, c2, p2)          → many G1 line segments
//      gcode.bezier_arc(p1, c1, c2, p2, tol) → few G2/G3 arc commands
//      gcode.save("name.nc")                  → G1-only output
//      gcode.save_arcs("name.nc")             → G1 + G2/G3 output
// ============================================================================

void ofApp::setup(){
    ofBackground(250);

    // 100 pixels per inch is the default for AxiDraw-style plotters
    gcode.setup(100);

    const float W = ofGetWidth();
    const float H = ofGetHeight();

    // -------------------------------------------------------------------------
    // Section 1 — Side-by-side S-curve comparison
    //
    //  Left  (x ≈ 50–350)  : drawn with bezier()      → G1 straight lines
    //  Right (x ≈ 450–750) : drawn with bezier_arc()  → G2/G3 arc commands
    //
    //  Both look the same on screen; open the saved .nc files to see the
    //  difference in the G-code.
    // -------------------------------------------------------------------------

    ofVec2f p1(50,  220), c1(160,  60), c2(280, 380), p2(370, 220);

    // Left: classic tessellation (50 line segments)
    gcode.bezier(p1, c1, c2, p2, 50);

    // Right: biarc approximation, tolerance = 1 pixel
    ofVec2f shift(400, 0);
    gcode.bezier_arc(p1 + shift, c1 + shift, c2 + shift, p2 + shift, 1.0f);

    // Control-point handles (drawn with plain lines — appear in both saves)
    ofSetColor(180);
    gcode.line(p1,       c1);
    gcode.line(p2,       c2);
    gcode.line(p1+shift, c1+shift);
    gcode.line(p2+shift, c2+shift);


    // -------------------------------------------------------------------------
    // Section 2 — Petal / rosette shape
    //
    //  A realistic creative use-case: a loop of connected Bezier curves.
    //  Using bezier_arc() keeps the G-code compact and arc-smooth.
    // -------------------------------------------------------------------------

    ofVec2f centre(W * 0.5f, H * 0.62f);
    const int petals = 7;
    const float r_tip = 110.f;
    const float r_cp  = 65.f;

    for (int i = 0; i < petals; i++){
        float a0 = TWO_PI * (float)i       / petals - HALF_PI;
        float a1 = TWO_PI * (float)(i + 1) / petals - HALF_PI;

        ofVec2f tip0 = centre + ofVec2f(cos(a0), sin(a0)) * r_tip;
        ofVec2f tip1 = centre + ofVec2f(cos(a1), sin(a1)) * r_tip;

        // Control points angled inward toward the centre
        float mid_angle = (a0 + a1) * 0.5f;
        ofVec2f cp0 = centre + ofVec2f(cos(mid_angle - 0.45f), sin(mid_angle - 0.45f)) * r_cp;
        ofVec2f cp1 = centre + ofVec2f(cos(mid_angle + 0.45f), sin(mid_angle + 0.45f)) * r_cp;

        gcode.bezier_arc(tip0, cp0, cp1, tip1, 0.5f);
    }


    // -------------------------------------------------------------------------
    // Section 3 — Tolerance comparison
    //
    //  Same curve drawn at three tolerances. Tighter tolerance → more arcs,
    //  closer to the true Bezier. Looser → fewer arcs, more visible deviation.
    //
    //  Left  : 0.25 px  (very tight — usually 4-8 arcs per curve)
    //  Centre: 2.0  px  (typical plotter quality)
    //  Right : 8.0  px  (loose — may be visually noticeably different)
    // -------------------------------------------------------------------------

    ofVec2f q1(50, H - 100), qc1(140, H - 200), qc2(250, H),   q2(340, H - 100);

    struct { float tol; ofVec2f offset; } cases[] = {
        { 0.25f, ofVec2f(  0, 0) },
        { 2.0f,  ofVec2f(230, 0) },
        { 8.0f,  ofVec2f(460, 0) },
    };
    for (auto& c : cases){
        gcode.bezier_arc(q1  + c.offset, qc1 + c.offset,
                         qc2 + c.offset, q2  + c.offset, c.tol);
    }


    // -------------------------------------------------------------------------
    // Save both formats so you can compare them in a G-code viewer
    // -------------------------------------------------------------------------

    // Traditional output (all G1 straight-line segments)
    gcode.save("bezier_lines_g1.nc");

    // Arc-aware output (G2/G3 where bezier_arc() or arc() was used, G1 elsewhere)
    gcode.save_arcs("bezier_arcs_g2g3.nc");

    ofLogNotice("example-bezier_arcs")
        << "Saved bezier_lines_g1.nc  and  bezier_arcs_g2g3.nc\n"
        << "Open both in https://ncviewer.com/ to compare arc vs line output.";
}

//--------------------------------------------------------------
void ofApp::draw(){

    // Both bezier() and bezier_arc() add a linearised preview to gcode.lines,
    // so draw() looks identical for both — the difference is only in the file.
    gcode.draw();

    // Labels
    ofSetColor(80);
    ofDrawBitmapString("bezier()  G1 lines",        60,  30);
    ofDrawBitmapString("bezier_arc()  G2/G3 arcs",  460, 30);

    ofDrawBitmapString("tolerance: 0.25 px",  60,  ofGetHeight() - 120);
    ofDrawBitmapString("tolerance: 2.0 px",   290, ofGetHeight() - 120);
    ofDrawBitmapString("tolerance: 8.0 px",   520, ofGetHeight() - 120);

    ofSetColor(140);
    ofDrawBitmapString("Open output/bezier_lines_g1.nc and output/bezier_arcs_g2g3.nc\n"
                       "in ncviewer.com to compare G-code output.",
                       20, ofGetHeight() - 18);
}

//--------------------------------------------------------------
void ofApp::update(){}
void ofApp::keyPressed(int key){}
void ofApp::keyReleased(int key){}
void ofApp::mouseMoved(int x, int y){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::mouseEntered(int x, int y){}
void ofApp::mouseExited(int x, int y){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){}
