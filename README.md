<div align=center style="display:inline-block"><img src=./branding/logo-cracked-splash-st.png><br><img src="https://img.shields.io/github/workflow/status/CrossCraft/CrossCraft-SurvivalTest/CrossCraft?style=for-the-badge&logo=github&label=SurvivalTest" height=36px> <img src="https://forthebadge.com/images/badges/made-with-c-plus-plus.svg"> <img src="https://forthebadge.com/images/badges/open-source.svg"></div>

---

CrossCraft-SurvivalTest is a project that aims to implement Minecraft Java Edition: Classic Survival Test. It is not a clone. CrossCraft does not use any of the source code of the original game, nor do the developers have any knowledge of the source code. CrossCraft does not use the original copyrighted assets. All knowledge is figured out with documentation and observation of in-game mechanics.

Textures are a combination of handmade and adaptations from [Unity Pack](https://www.curseforge.com/minecraft/texture-packs/unity) under CC0

CrossCraft's goal is to be platform-independent and work across all PC platforms and many major game consoles. CrossCraft supports Windows, Mac, Linux, PSP, and PS Vita. Further support will be added over time as systems change and upgrade. The next planned platform is the PS2.

We are not associated with Mojang or Microsoft.

<br>
<div align=center><img src=./branding/Screenshot.png></div>
<br>

## Version Compatibility

Each individual Survival Test update is not meant to be reflective of every step of the 4 main Survival Test phases. It is a rapid development phase to reach 0.28 Survival Test.

## Download

Check the [releases panel](https://github.com/CrossCraft/CrossCraft-SurvivalTest/releases) on the side or use the link.

The most up-to-date version (potentially containing more bugs or half-implemented features) will be in the latest Actions build. You can find these in the [actions tab](https://github.com/CrossCraft/CrossCraft-SurvivalTest/actions). At the bottom of each action run is the published artifacts for download.

## How to Build

Ubuntu/Debian Linux:

`git clone https://github.com/CrossCraft/CrossCraft-SurvivalTest.git --recursive`

`sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev libasound2-dev libjack-jackd2-dev libsdl2-dev oss4-dev libpulse-dev`

`cd CrossCraft-SurvivalTest`

`mkdir -p build`

`cd build`

`cmake ..`

`make`

Also copy your audio and texturepacks folder into the build directory.

## License

CrossCraft uses the Mozilla Public License.
If you make modifications to this program, you must publish changes to the source code. 
