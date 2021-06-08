#include "frame_iterator.h"

#define NUM_HUE_VALUES 200
#include <iostream>

RainbowFetcher::RainbowFetcher(const char* rainbowImagePath, int numFrames, Geometry resizeParams) {
    // Load a rainbow image from a file.
    rainbow.read(rainbowImagePath);
    //Resize to fit gif.
    resizeParams.aspect(true);
    rainbow.resize(resizeParams);
    huePerImage = (double)NUM_HUE_VALUES / (double)numFrames;
    _curFrame = rainbow;
    _lastFrame = false;
    // load in the first frame values
    nextFrame();
}

Image RainbowFetcher::nextFrame() {
    // Define the rainbow image. Note that the page size and the number of columns and rows are different. 
    _curFrame = rainbow;
    // increment hue by number of frames
    _curFrame.modulate(100, 100, 100 + hue);
    int prevHue = hue;
    hue += huePerImage;
    if (hue >= NUM_HUE_VALUES) {
        _lastFrame = true;
        hue = 0;
    }
    return _curFrame;
}

Image GifFetcher::nextFrame() {
    // To do this, we want to effectively play back the gif so we don't rainbowify bits we don't want to.
    // Animated Gifs have a setting called dispose type that defines how the layers are going to be combined. http://www.theimage.com/animation/pages/disposal.html
    // Hence, to 'play' back the gif, we need to check the dispose type of the previous frame, and combine it based on the 
    // defined dispose type:
    Image nextFrame = *imageIterator;

    switch (nextFrame.gifDisposeMethod()) {
    case NoneDispose:
    case UndefinedDispose: {
        // No disposal occurs. Each layer we need to do the union with the prevImage.
        Image combinedImage = _curFrame;
        combinedImage.composite(nextFrame, SouthEastGravity, OverCompositeOp);
        combinedImage.animationDelay(nextFrame.animationDelay());
        combinedImage.gifDisposeMethod(nextFrame.gifDisposeMethod());
        _curFrame = combinedImage;
    }
        break;
    case PreviousDispose:
        // We dispose of the previous layer - we only keep this layer.
        _curFrame = nextFrame;
        break;
    case BackgroundDispose: {
        // The background is set to a specified background colour (e.g. background transparency colour) before the image is overlayed on top of it.
        Image combinedImage = Image(nextFrame.geometry(), nextFrame.backgroundColor());
        combinedImage.composite(nextFrame, SouthEastGravity, OverCompositeOp);
        combinedImage.animationDelay(nextFrame.animationDelay());
        combinedImage.gifDisposeMethod(nextFrame.gifDisposeMethod());
        _curFrame = combinedImage;
    }
        break;
    }

    imageIterator++;

    if (imageIterator == gif.end()) {
        _lastFrame = true;
        imageIterator = gif.begin();
    }
    else {
        _lastFrame = false;
    }

    return _curFrame;
}

GifFetcher::GifFetcher(const char* gifPath) {
    readImages(&gif, gifPath);
    imageIterator = gif.begin();
    _curFrame = (*imageIterator);
    _lastFrame = false;

    // load in the first frame values
    nextFrame();
}

int GifFetcher::numFrames() {
    return gif.size();
}