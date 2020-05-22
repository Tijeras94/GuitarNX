# GuitarNX
A Guitar Hero like game for the Nintendo Switch.	You can add music from the format of "Frets on fire '(visit http://fretsonfire.wikidot.com/custom-songs)

## Compiling GuitarNX in Windows

* Follow instructions under the Windows Section in https://switchbrew.org/wiki/Setting_up_Development_Environment
* Try building the examples to make sure all libraries are installed. Launch MSYS terminal and type the following:
    ```Shell
    cd /c/devkitpro/examples/switch
    make
    ```
    
* When compiling the examples in a clean Devkit installation the some libraries/tools where not installed.
   I was able to complile all examples with the following libraries/tools
   ```Shell
   pacman -S switch-pkg-config
   pacman -S switch-opusfile
   pacman -S switch-libopus
   pacman -S switch-sdl2
   pacman -S switch-sdl2_mixer
   pacman -S switch-glm
   pacman -S switch-glad
   pacman -S switch-freetype
   pacman -S switch-curl
   ```

* Clone&Build GuitarNX 
   ```Shell
   #install git in you dont have it
   pacman -S git
   cd /c
   mkdir GuitarNX
   cd GuitarNX
   git clone https://github.com/Tijeras94/GuitarNX.git .
   make
    ```

# How to Play

GuitarNX is a game based on the Guitar Hero video game in which we must play the colored notes that come towards us, the difficulty of the songs vary but contains 4 levels for each of which are ; Super Easy, Easy, Medium, Expert, all of them depending on the properties of the song.

## Screenshots

![](/Screenshoots.png)

## Credits

* The game uses Wallpapers from https://unsplash.com/@narrowedge
* The game contains code and assets from GuitarStar v1.99(Guitar Hero for PSP) by festi


