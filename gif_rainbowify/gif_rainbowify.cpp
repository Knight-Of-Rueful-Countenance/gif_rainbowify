// gif_rainbowify.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#define TOUHOU_GIRL

#ifdef TOUHOU_GIRL
#define IMAGE "B:\\Users\\Idris\\Pictures\\1470984796120.gif"
#define IMAGE_OUT "B:\\Users\\Idris\\Pictures\\1470984796120_rainbowed.gif"
#define COL_FROM "blue"
#define COL_TO "cyan"

#else

#define COL_FROM "dim gray"
#define COL_TO "white"
#define IMAGE "B:\\Users\\Idris\\Pictures\\1489175712017a.gif"
#define IMAGE_OUT "B:\\Users\\Idris\\Pictures\\1489175712017a_cropped.gif"
#endif

#define RAINBOW_IMAGE_PATH "C:\\Users\\Idris\\source\\repos\\gif_rainbowify\\rainbow.png"
#define NUM_HUE_VALUES 200

#include <Magick++.h>
#include <iostream> 

using namespace std;
using namespace Magick;


int main(int argc, char** argv)
{
    InitializeMagick(*argv);

    list<Image> imageList;
    list<Image> newGif;

    // If this is ticked, all areas that were previously transparent will be rainbowified in addition to all areas that match the chroma key.
    // however, when false its still a little broken - the rainbowified areas from the previous frame aren't copied over to the current frame, meaning
    // they have the wrong hue.
    bool makeTransparentRainbow = false;
    
    try {
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
        double huePerImage = NUM_HUE_VALUES / imageList.size();

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
    catch (Exception& error_)
    {
        cout << "Caught exception: " << error_.what() << endl;
        return 1;
    }

    return 0;
}


// TODO: 
// animate the rainbow effect.
// this can be done by adjusting the hue of the rainbow image.
// Also give an option for repeating textures  (e.g. like the gold moving left to right) by moving left to right with them.
// then add Command line interface for easy access.
// figure out what to if we want the rainbow animation to have a different number of frames than the initial gif (e.g. for 1 frame gifs, etc.).
//  maybe repeat the gif if we keep the framerate the same, or add new frames.

// looks like you need to put more thought into this:
   // you need to keep trackof the rainbow positions in previous images that are being carried over to the current frame as we need to animate them there as well.