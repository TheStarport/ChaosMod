# ChaosMod
ChaosMod is a mod that provides a suite of tools and features for the 2003 video game Freelancer. 
These features and tools enable us to make the game more whacky, random, and overall more chaotic.
As one would expect, these do not always make the game more enjoyable, 
but do provide a new and unique experience for this age old game.

## Features

ChaosMod's suite contains a lot of different features. All are togglable and highly configurable through the provided
configurator tool in game. By default, the mod's settings and menu can be accessed through the 'wheel menu' using the
middle mouse button (Mouse 3), but this button can be remapped like any other key if desired.

### Chaos
The 'Chaos' feature, for which the entire mod is named, is a periodic timer that will trigger random effects while in space.
This could be beneficial like giving you OP weapons, or it could be detrimental where we spawn hostile NPCs. 
It could be neither where the effect just makes the universe different or annoying.

Chaos supports Twitch integration thanks to the work done on the GTAV chaos mod (linked in the credits). This allows
Twitch chat to vote for what the next effect will be. Please see the docs for instructions on setting this up!

### Patch Notes
The 'Patch Notes' feature allows the game to retroactively patch itself to alter equipment stats, good prices, 
global constants, and everything in between. These patches are permanent and not tied to the save state of the game. 

The scope of these changes is based on the current value of whatever is being changed, and it's totally random. When a 
patch is generated it could decide the price of Oxygen is now globally set to 42, or it could decide that justice mk1 
now has a refire rate of 39 shots per second.

### Auto Saves
Due to the instability caused by the very nature of this mod, the 'Auto Save' feature provides regular in-space auto saves
that are separate from the normal saves that occur during play.

*Disclaimer: Chaos saves created during missions can break save states, and generally should not be trusted*

## Why?
Why would you install this when you're told it's main design goal is to make your experience worse? Because it's funny.

## Installing
If you are just looking to play the mod, there is two ways of doing so. 
Within the release section there are nightly releases published (provided changes have occurred),
these releases contain two 7zip files: an installer and an assets package.

### Installer
The installer contains *everything* the mod is going to require, outside of a vanilla install of Freelancer.
Running the installer requires a copy of vanilla Freelancer, but it will create a build of Freelancer HD Edition and
then apply ChaosMod over the top of it.

*Disclaimer: the installer's progress bar does not work correctly, and it takes a long time. It may look like it's 
frozen, but be patient.*

### Assets
If you do not want to use HD Edition as the base, you can download the assets zip instead.
This contains an EXE folder and a DATA folder. Simply copy these two whatever Freelancer install you wish to use,
replacing any files that are found. Please note that, unlike the installer, no support will be provided if issues are found
when working with a non-vanilla/modified install of Freelancer.

## Building
The project is built using the Clang compiler, MSVC toolchain, and CMake.
As we use CMake presets, building it via the command line is fairly trivial.

*Disclaimer: Only builds with Visual Studio 2022 + Clang 16 have been tested and confirmed to work.*

### Cloning

VCPKG is required in order to install various dependencies that ChaosMod has. 
If you already have VCPKG installed, you can skip this step. The VCPKG_ROOT environment variable must be set
and must point to the where ever VCPKG is cloned to.

```cmd
git clone https://github.com/microsoft/vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
cd ..
```

VCPKG_ROOT:

If you are on Windows, you can set the ENV with the setx command.

CMD:
```cmd
setx VCPKG_ROOT %cd%\vcpkg
```

Powershell:
```ps
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", $pwd.Path + "\vcpkg", "User")
```

(On Windows, the terminal must be restarted in order to use updated environment variables)

Run the following commands to setup the environment (it is assumed Visual Studio and CMake are installed):
```bash
git clone --recurse-submodules https://github.com/TheStarport/ChaosMod
cd ChaosMod
```

Run the following commands in order to build debug (for release, just change 'debug' to 'release'):
```bash
cmake --preset vs2022-msvc-debug -S . -B ./build/vs2022-msvc-debug
cmake --build ./build/vs2022-msvc-debug --target ChaosMod --config Debug
```

If you prefer to use an IDE:
- CLion supports presets out of the box and should automatically allow one to select them from the CMake profile window.
- VSCode has a CMake extension that can handle presets. Documentation for it is available
[here](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/cmake-presets.md).
- Visual Studio itself supports CMake. Documentation for is available 
[here](https://learn.microsoft.com/en-us/cpp/build/cmake-presets-vs?view=msvc-170).

## Contributing

See [contributing guidelines](CONTRIBUTING.md) for more information.

## Docs
Documentation for various features pending!!

## Credits

### Direct
- [Lazrius](https://github.com/Lazrius) - Hi it me. I've done all the code and many other things. 
- [IrateRedKite](https://github.com/IrateRedKite) - Project management, THN intro, INI work, build scripts, workflows, ideas, etc. The list can go on for hours!
- [Aingar](https://github.com/Aingar) - Assistance with bug fixing crashes
- [Haste](https://github.com/HasteDC) - Help with custom models.
- Pillow - Work on AI pilot improvements.
- KamiFaby - Large amounts of play testing to find and remove crashes.
- [BC46](https://github.com/BC46/freelancer-hd-edition) - BC's work on HD edition which ChaosMod is directly built on top of and assistance with reverse engineering.
- [Ven](https://the-starport.com/forums/user/venemon) - Assistance with reverse engineering and offset finding.
- [NekuraMew](https://the-starport.com/forums/user/nekuramew) - ImGui style improvements (looks more Freelancer-y)
- [Beagle](https://www.twitch.tv/beagsandjam) - The inspiration for the project.
- [The Entire Starport Community](https://the-starport.com) - Everyone within the Starport contributues to the continued survival and development of this game, and I am forever thankful.

### External

- [pongo1231](https://github.com/gta-chaos-mod/ChaosModV) - Creator of GTAV Chaos Mod, whose ideas and Twitch Integration have been used
- [Naveen Himal](https://sketchfab.com/3d-models/christ-the-redeemer-d6f51d0fad7142a08ed26455f84f72ca) - Christ The Redeemer Model
- [CATENCODE](https://sketchfab.com/3d-models/the-flying-spaghetti-monster-7b4dd5fa121a4021875d9b72c7aa52c8) - Flying Spaghetti Monster Model
- [savounited](https://sketchfab.com/3d-models/alduin-dragon-2242802d82f14846bd4a2337a1794701) - Alduin Dragon Model
- [Milaein](https://sketchfab.com/3d-models/low-poly-pirate-blunderbuss-gun-075bb97c059046e1b3c8447b54651d1b) - Blunderbuss Model
