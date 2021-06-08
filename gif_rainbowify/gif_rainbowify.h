#pragma once

#ifndef GIF_RAINBOWIFY_H_
#define GIF_RAINBOWIFY_H_

#include <Magick++.h>
#include <iostream> 
#include <sstream>

using namespace std;
using namespace Magick;


/*******************************
 *******Structs and Enums*******
 *******************************/

enum class RainbowifyType {
    Rainbow,
    GoldPan,
    SilverPan,
    PixellatedRainbow,
};

enum class TransparencyType {
    // If this is ticked, all areas that were previously transparent will be rainbowified in addition to all areas that match the chroma key.
    // however, when false its still a little broken - the rainbowified areas from the previous frame aren't copied over to the current frame, meaning
    // they have the wrong hue.
    RainbowifyTransparentAreas,

    RainbowifyStartAndEndColourAreas,
    RainbowifyStartTransparentAndEndColourAreas,
};

struct RainbowifyOptions {
    RainbowifyType animationType;

    // The total length of the rainbow animation in ms. It is best if this divides/timeses evenly into the original gifs length.
    int rainbowAnimationLength;
    // The total number of frames of rainbow animation to have
    int rainbowAnimationNumFrames;

    // How we should go about deciding which areas should be rainbowified
    TransparencyType transparencyType;

    // The colours between which stuff should be rainbowified if the transparency type defines it.
    Color startColour;
    Color endColour;
    bool onlyUseStartColour;
    // animate at the same rate as the existing framerate
    bool onlyUseExistingFrames;

    const char* imageDir;
    const char* outDir;
};

/******************************
 *******Public Functions*******
 ******************************/

void legacyCode();

Image rainbowifyFrame(Image rainbowImage, Image gifImage, RainbowifyOptions options);

void gifRainbowify(RainbowifyOptions options);

RainbowifyOptions testPatterns();

Color RGBHexToColor(const char* h);

Quantum getColourValue(const char* hexcode, int i);

#endif