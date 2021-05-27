# ofxGCode
#### version 1.0

![ofxGCode drawings](https://github.com/andymasteroffish/ofxGCode/blob/master/topper.jpg)

This is a G-Code library for openFrameworks 0.11.0, mostly made with my AxidDaw pen-plotter.

This creates 2 dimensional G-code instructions, primarily for plotters.

You could probably use it for laser cutters or CNC machines but you might need to edit it a little bit. Please let me know if you do!

This repo is constantly evolving. I add new features as I need them. This project is by me and for me, but I think it can be helpful to other folks too!

There are parts of this that are wildly inefficient. The trimming functions in particular will really slow things down when done in large numbers. Luckily you don't need to hit 90FPS when making a still image.

# Structure

The goal of this library is to create an environment that feels very similar to drawing with oF and other creative coding tools. It has functions to do things like draw a line or a circle or a polygon.

It also includes a lot of tools to do helpful things like trim lines that are inside or outside a certain shape, and to optimize the order of lines to make a shorter overall path.

The most important class is ofxGCode. This represents a single drawing and will produce a single G-code file (typically .nc).

The second most important class is GLine. Everything in this library is a line and this class has a lot of helpful tools for dealing with them.

See ofxGCode.hpp and ofxGLine.h for explanations. These two classes make up the bulk of the functionality and I have tried to comment them effectively.

# Examples

In Lieu of proper documentation, I have provided a lot of example projects that cover just about everything I do with this library.

You can use a site like https://ncviewer.com/ to test the output.


# Using G-code with the AxiDraw

If you use an AxiDraw you may be scratching your head because the AxiDraw does not natively support G-code.

To get around this, I have a python script that reads G-code and controls the AxiDraw.

https://github.com/andymasteroffish/axidraw_gcode_reader



# Stray Thoughts

I'm sure typos abound. Sorry about that.

If you like my plotter work, you can buy some of it here: https://shop.andymakes.com/

Everything on that page was made with this library!

If you use this library, please let me know. I'd love to see what you make!

You can email me at andy[at]andymakes[dot]com or hit me up on twitter @andy_makes


# Acknowledgements

This library incorporates code from these repos:

### ofxHersheyFont by Tobias Zimmer
https://github.com/tobiaszimmer/ofxHersheyFont

### VST by Trammell Hudson
https://github.com/osresearch/vst

Thank you so much for your contributions!