# gif_rainbowify [UNFINISHED]

Imagemagick based command line solution to add animated rainbow effects to gifs

![dancing rainbow skeletons](https://github.com/Knight-Of-Rueful-Countenance/gif_rainbowify/blob/master/rainbow_skeles.gif?raw=true)

## Building

- Install version 6.9.12 of the Magick++ libraries from here: https://download.imagemagick.org/ImageMagick/download/binaries/
  I used ImageMagick-6.9.12-14-Q16-HDRI-x64-dll.exe because the demos won't working cause they were looking for CORE*RL_magick*.lib
  rather than seems CORE*RL_MagickCore*.lib as is present in 7.0, so for simplicity I used the older version.
- Next, open up the visual studio project, goto Project > gif_rainbowify properties > Configuration Properties > VC++ Directories and change _C:\Program Files\ImageMagick-6.9.12-Q16-HDRI\lib_ or _C:\Program Files\ImageMagick-6.9.12-Q16-HDRI\include_ in external include directories and library directories to point to where you installed imageMagick.
- Make sure you are using Release. The default imageMagick .lib files are only kitted up for release mode - you have to compile them yourself if you want to use Debug.
- Press Play and cross your fingers!

## Todo list

- Command Line interface part of it
- Scrolling Texture mode
- option to select more than one colour/range of colours to rainbowify.
- option to use more than one rainbowify effect?
- Auto framerate determination for rainbow effect (e.g. to minimize framecount whilst still looking good)
- gif optimization option (with flexiGif)
