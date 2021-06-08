// gif_rainbowify.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "gif_rainbowify.h"
#include "frame_iterator.h"

#define TOUHOU_GIRL

#ifdef TOUHOU_GIRL
#define IMAGE "B:\\Users\\Idris\\Pictures\\1470984796120.gif"
#define IMAGE_OUT "B:\\Users\\Idris\\Pictures\\1470984796120_rainbowed_new.gif"
#define COL_FROM "blue"
#define COL_TO "cyan"

#else

#define COL_FROM "dim gray"
#define COL_TO "white"
#define IMAGE "B:\\Users\\Idris\\Pictures\\1489175712017a.gif"
#define IMAGE_OUT "B:\\Users\\Idris\\Pictures\\1489175712017a_test.gif"
#endif

#define RAINBOW_IMAGE_PATH "C:\\Users\\Idris\\source\\repos\\gif_rainbowify\\rainbow.png"
#define NUM_HUE_VALUES 200

int main(int argc, char** argv)
{
    InitializeMagick(*argv);

    RainbowifyOptions options = testPatterns();
    gifRainbowify(options);
    /*try {
        //legacyCode();
        RainbowifyOptions options = testPatterns();
        gifRainbowify(options);
    }
    catch (Exception& error_)
    {
        cout << "Caught exception: " << error_.what() << endl;
        return 1;
    }*/

    return 0;
}

RainbowifyOptions testPatterns() {

    // wth apparently they are only adding . based structure inialization to cpp in C++20 :( https://stackoverflow.com/questions/11516657/c-structure-initialization
    return {
        /*.animationType = */RainbowifyType::Rainbow,
        // Note that the animation standards are in hundreths of seconds, rather than milliseconds
        /*.rainbowAnimationLength=*/200,
        /*.rainbowAnimationFrames=*/40,
        /*.transparencyType=*/TransparencyType::RainbowifyStartAndEndColourAreas,
        /*.startColour=*/RGBHexToColor("6AFCFF"), // Color(COL_FROM),
        /*.endColour=*/Color(COL_TO),
        /*.onlyUseStartColour=*/true,
#ifdef TOUHOU_GIRL
        /*.onlyUseExistingFrames=*/true,
#else
        /*.onlyUseExistingFrames=*/false,
#endif
        /*.imageDir=*/IMAGE,
        /*.imageOut=*/IMAGE_OUT,
    };
}

void legacyCode() {
    list<Image> imageList;
    list<Image> newGif;

    // If this is ticked, all areas that were previously transparent will be rainbowified in addition to all areas that match the chroma key.
    // however, when false its still a little broken - the rainbowified areas from the previous frame aren't copied over to the current frame, meaning
    // they have the wrong hue.
    bool makeTransparentRainbow = true;

    // Read each frame of the GIF 
    readImages(&imageList, IMAGE);

    // Load a rainbow image from a file.
    // Resize it to match the image dimensions of the gif image we are loading.
    Image base_rainbow;
    base_rainbow.read(RAINBOW_IMAGE_PATH);

    // Scale to fit the existing image.
    Geometry resizeParams = imageList.front().page();
    resizeParams.aspect(true);
    base_rainbow.resize(resizeParams);

    // Define chosen colour range for transparency (e.g. which pixels will be rainbowified:
    // NOTE: transparencyChroma is only really suitable for colours that are similar (e.g. are monotonic across channels).
    Color startColour = Color(COL_FROM), endColour = Color(COL_TO);
    double hue = 100;
    int huePerImage = NUM_HUE_VALUES / imageList.size();

    Image prevImage = imageList.front();

    for (Image image : imageList) {
        Image combinedImage;
        // To do this, we want to effectively play back the gif so we don't rainbowify bits we don't want to.
        // Animated Gifs have a setting called dispose type that defines how the layers are going to be combined. http://www.theimage.com/animation/pages/disposal2.html
        // Hence, to 'play' back the gif, we need to check the dispose type of the previous frame, and combine it based on the 
        // defined dispose type:
        switch (image.gifDisposeMethod()) {
        case NoneDispose:
        case UndefinedDispose:
            // No disposal occurs. Each layer we need to do the union with the prevImage.
            combinedImage = prevImage;
            combinedImage.composite(image, SouthEastGravity, OverCompositeOp);
            break;
        case PreviousDispose:
            // We dispose of the previous layer - we only keep this layer.
            combinedImage = image;
            break;
        case BackgroundDispose:
            // The background is set to a specified background colour (e.g. background transparency colour) before the image is overlayed on top of it.
            combinedImage = Image(image.geometry(), image.backgroundColor());
            combinedImage.composite(image, SouthEastGravity, OverCompositeOp);
            break;
        }

        // Make the sections of image we want transparent
        Image diffImage = combinedImage;
        combinedImage.transparentChroma(startColour, endColour);

        // Define the rainbow image. Note that the page size and the number of columns and rows are different. 
        Image rainbow_image = base_rainbow;
        // increment hue by number of frames
        rainbow_image.modulate(100, 100, hue);
        hue += huePerImage;

        // Copy the animation delay from the previous image
        rainbow_image.animationDelay(image.animationDelay());

        // Put the combined image on top of the rainbow image
        rainbow_image.composite(combinedImage, SouthEastGravity, OverCompositeOp);

        // Copy the opacity values from the old combined Image so previously transparent areas aren't rainbowified (unless desired)
        if (!makeTransparentRainbow) {
            rainbow_image.composite(diffImage, SouthEastGravity, CopyOpacityCompositeOp);
        }

        // Copy the original image page information so the image is drawn in the right place.
        rainbow_image.gifDisposeMethod(image.gifDisposeMethod());
        rainbow_image.page(image.page());
        rainbow_image.repage();

        newGif.push_back(rainbow_image);

        // Update prevImage
        prevImage = combinedImage;
    }

    // Write the image to a file 
    writeImages(newGif.begin(), newGif.end(), IMAGE_OUT);
}

Image rainbowifyFrame(Image rainbowImage, Image gifImage, RainbowifyOptions options) {
    Image compositeImage = rainbowImage;
    Image diffImage = gifImage;

    if (options.transparencyType != TransparencyType::RainbowifyTransparentAreas) {
        // Make the sections of image we want transparent
        if (options.onlyUseStartColour) {
            gifImage.transparent(options.startColour);
        }
        else {
            gifImage.transparentChroma(options.startColour, options.endColour);
        }

    }

    // Copy the animation delay from the previous image

    // Put the combined image on top of the rainbow image
    compositeImage.composite(gifImage, SouthEastGravity, OverCompositeOp);

    // Retore the areas that were originally transparent since the user doesn't want them to be transparent.
    if (options.transparencyType == TransparencyType::RainbowifyStartAndEndColourAreas) {
        compositeImage.composite(diffImage, SouthEastGravity, CopyOpacityCompositeOp);
    }

    // Copy the original image page information so the image is drawn in the right place.
    //compositeImage.animationDelay(5); // placeholder number that should not be needed as the animationDelay is set in gifRainbowify for the prev Frame TODO  figure out what animation delay to have for the final frame of the 
    compositeImage.gifDisposeMethod(gifImage.gifDisposeMethod());
    compositeImage.page(gifImage.page());
    compositeImage.repage();

    return compositeImage;
}

void gifRainbowify(RainbowifyOptions options) {
    list<Image> newGif;

    int rainbowIncrement = options.rainbowAnimationLength / options.rainbowAnimationNumFrames;

    int lastRainbowFrameTime = 0;
    int lastGifFrameTime = 0;

    GifFetcher gif = GifFetcher(options.imageDir);
    Geometry resizeParams = gif.curFrame().page();
    RainbowFetcher rainbow = RainbowFetcher(RAINBOW_IMAGE_PATH, options.onlyUseExistingFrames ? gif.numFrames() : options.rainbowAnimationNumFrames, resizeParams);

    int maxFrames = max(gif.numFrames(), 50);
    int curFrame = 0;
    bool end = false;
    do {
        curFrame++;
        if (curFrame >= maxFrames) {
            break;
        }

        Image rainbowFrame = rainbowifyFrame(rainbow.curFrame(), gif.curFrame(), options);
        end = rainbow.lastFrame() && gif.lastFrame();

        // Note: sometimes weebs will specify animation delays of 0 or 1. In this case, most viewers are just going to render
        // At the default animation rate of 20? (15 too fast) TODO find out if this is actually the default.
        int animationDelay = gif.curFrame().animationDelay() > 1 ? gif.curFrame().animationDelay() : 10;
        int nextGifTime = animationDelay + lastGifFrameTime;
        int nextRainbowTime = options.onlyUseExistingFrames ? nextGifTime : rainbowIncrement + lastRainbowFrameTime;
        int lastFrameTime = max(lastRainbowFrameTime, lastGifFrameTime);

        if (nextGifTime >= nextRainbowTime) {
            rainbowFrame.animationDelay(nextRainbowTime - lastFrameTime);
            rainbow.nextFrame();
            lastRainbowFrameTime = nextRainbowTime;
        }

        if (nextGifTime <= nextRainbowTime) {
            rainbowFrame.animationDelay(nextGifTime - lastFrameTime);
            gif.nextFrame();
            lastGifFrameTime = nextGifTime;
        }

        newGif.push_back(rainbowFrame);
    } while (!end);
    writeImages(newGif.begin(), newGif.end(), options.outDir);
}

Quantum getColourValue(const char* hexcode, int i) {
    unsigned int value;
    stringstream ss;
    ss << hex << hexcode[i] << hexcode[i + 1];
    ss >> value;
    // Rescale to quantum range
    return value * (QuantumRange / 255);
};

Color RGBHexToColor(const char* h) {
    // if the first character
    
    Color c = Color(getColourValue(h, 0),
                    getColourValue(h, 2),
                    getColourValue(h, 4));
    return c;
}

// TODO: 
//  Some images (such as touhou rainbow) are slightly off centre (perhaps caused by repage?) figure out and fix
// Also give an option for repeating textures  (e.g. like the gold moving left to right) by moving left to right with them.
// then add Command line interface for easy access.
// come up with an automated method for deciding a suitable framerate for the rainbow (e.g. the closest multilpe of the existing frames that will mean the rainbow animation completes every second)