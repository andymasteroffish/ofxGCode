# ofxGCode
GCode library for oF 0.11.0, mostly made with my axidraw in mind

The gcode is converted into something the axidraw can use via my python script: https://github.com/andymasteroffish/axidraw_gcode_reader

I did some pretty major updates after Inktober 2020. The new version is not completely backwards compatible, but you can find it in the "classic" branch of this repo.

This is a work in progress. Some parts are a bit buggy. I'll add features as I need 'em.

There are parts of this that are wildly innificient. Trimming functions in particular will really slow things down when done in large numbers. Luckily you don't need to hit 30FPS when making a still image.

See ofxGCode.hpp and ofxGLine.h for explanations. These two classes make up the bulk of the fnctionality and I have tried to comment them nicely.

If you like my plotter work, you can buy some of it here: https://andymakes.bigcartel.com/

This library incorporates some features from these repos:
* https://github.com/tobiaszimmer/ofxHersheyFont
* https://github.com/osresearch/vst