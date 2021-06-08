# gif_rainbowify

Imagemagick based command line and dynamic linked library solution to add animated rainbow effects to gifs

## Building

- You can get the Magick++ libraries from here: https://download.imagemagick.org/ImageMagick/download/binaries/
  I got this one ImageMagick-6.9.12-14-Q16-HDRI-x64-dll.exe because the demos won't working cause they were looking for CORE*RL_magick*.lib
  rather than seems CORE*RL_MagickCore*.lib as is present in 7.0, so for simplicity I used the older version.
- Next, open up the visual studio project, and point it towards the directories where the lib and header files for imagemagick are included
