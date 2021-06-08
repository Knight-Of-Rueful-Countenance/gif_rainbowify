// gif_rainbowify.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define IMAGE "B:\\Users\\Idris\\Pictures\\1489175712017a.gif"
#define IMAGE_OUT "B:\\Users\\Idris\\Pictures\\1489175712017a_cropped.gif"
#define RAINBOW_IMAGE_PATH "C:\\Users\\Idris\\source\\repos\\gif_rainbowify\\rainbow.png"



#include <Magick++.h>
#include <iostream> 

using namespace std;
using namespace Magick;

int main(int argc, char** argv)
{
    InitializeMagick(*argv);

    // Construct the image object. Seperating image construction from the 
    // the read operation ensures that a failure to read the image file 
    // doesn't render the image object useless. 
    list<Image> imageList;

    // TODO define rainbow image based on page size here, and crop it where it is currently defined.
    
    try {
        // Read each frame of the GIF 
        readImages(&imageList, IMAGE);

        // build a new image but with the different height
        list<Image> newGif;

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
        Color startColour = Color("dim gray"), endColour = Color("white");
        double hue = 100;
        
        for (Image image : imageList) {
            // We want the sections that were originally transparent to remain transparent.
            // We can do this by CopyOpacityCompositeOp to copy the opacity from the original image 
            Image originalImage = image;

            // Make the sections of image we want transparent
            image.transparentChroma(startColour, endColour);

            // Define the rainbow image. Note that the page size and the number of columns and rows are different. 
            Image rainbow_image = base_rainbow;
            // Chagne the hue by 20%
            rainbow_image.modulate(100, 100, hue);
            hue+=20;

            //rainbow_image.crop(image.page());

            // Copy the animation delay from the previous image
            rainbow_image.animationDelay(originalImage.animationDelay());

            // Crop/Scale the rainbow image to the size of the image.
            rainbow_image.composite(image, SouthEastGravity, OverCompositeOp);

            // Copy the original image opacity back so those sections aren't blue.
            rainbow_image.composite(originalImage, SouthEastGravity, CopyOpacityCompositeOp);

            // Copy the original image page information so the image is drawn in the right place.
            rainbow_image.page(originalImage.page());
            rainbow_image.repage();

            newGif.push_back(rainbow_image);
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