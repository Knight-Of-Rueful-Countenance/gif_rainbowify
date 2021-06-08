#pragma once

#ifndef FRAME_ITERATOR_H_
#define FRAME_ITERATOR_H_

#include <Magick++.h>

using namespace std;
using namespace Magick;

template <typename Frametype> class FrameIterator {
public:
    /// <summary>
    /// Fetches and returns the next frame of the sequence. If the last frame was the final frame, the first frame is returned.
    /// Should update lastFrame is the currentFrame is the last frame
    /// </summary>
    /// <returns>next frame of the sequence</returns>
    virtual Frametype nextFrame() = 0;
    int lastFrame() {
        return _lastFrame;
    }

    Frametype curFrame() {
        return _curFrame;
    }
protected:
    Frametype _curFrame;
    bool _lastFrame;
};


class RainbowFetcher : public FrameIterator<Image> {
public:
    /// <summary>
    /// Initializes the rainbow fetcher, fetching the rainbow image and defining parameters
    /// </summary>
    /// <param name="rainbowImagePath">The directory of the rainbow file</param>
    /// <param name="numFrames">number of frames per iteration of the lenght of the image</param>
    /// <param name="resizeParams">The Geometric of the gif to rainbowify. Can be generally retrieved from image.page()</param>
    RainbowFetcher(const char* rainbowImagePath, int numFrames, Geometry resizeParams);
    Image nextFrame() override;
private:
    Image rainbow;
    double hue = 0;
    double huePerImage;
};

class GifFetcher : public FrameIterator<Image> {
public:
    GifFetcher(const char* gifPath);
    Image nextFrame() override;
    int numFrames();
private:
    list<Image> gif;
    list<Image>::iterator imageIterator;
};

#endif