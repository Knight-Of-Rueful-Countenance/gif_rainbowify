# gif_rainbowify

Imagemagick based command line solution to add animated rainbow effects to gifs

## Building

- Install version 6.9.12 of the Magick++ libraries from here: https://download.imagemagick.org/ImageMagick/download/binaries/
  I used ImageMagick-6.9.12-14-Q16-HDRI-x64-dll.exe because the demos won't working cause they were looking for CORE*RL_magick*.lib
  rather than seems CORE*RL_MagickCore*.lib as is present in 7.0, so for simplicity I used the older version.
- Next, open up the visual studio project, goto Project > gif_rainbowify properties > Configuration Properties > VC++ Directories and change *C:\Program Files\ImageMagick-6.9.12-Q16-HDRI\lib* or *C:\Program Files\ImageMagick-6.9.12-Q16-HDRI\include* in external include directories and library directories to point to where you installed imageMagick.


## Todo list
