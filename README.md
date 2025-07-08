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
then apply ChaosMod over the top of it. In the release package, you'll need to download each of the Installer.7z.00\* 
files. The archive is split into parts due to Github's file upload limit. Once you have downloaded each of the parts,
open Installer.7z.001 with 7Zip in order to extract it.

*Disclaimer: the installer's progress bar does not work correctly, and it takes a long time. It may look like it's 
frozen, but be patient.*

### Assets
If you do not want to use HD Edition as the base, you can download the assets zip instead.
This contains an EXE folder and a DATA folder. Simply copy these two whatever Freelancer install you wish to use,
replacing any files that are found. Please note that, unlike the installer, no support will be provided if issues are found
when working with a non-vanilla/modified install of Freelancer.

## Building
The project is built with CMake using the MSVC toolchain, 
and can be built using either the MSVC compiler or the clang compiler. 
As we use CMake presets, building it via the command line is fairly trivial.

*Disclaimer: Only builds with Visual Studio 2022 + Clang 19 have been tested and confirmed to work.*

### Cloning & Building Locally

The following tools are required in order to setup ChaosMod on all operating systems:
- 7z (7-Zip Archive Manager)
- Python 3.11+
- CMake
- MSVC Compiler supporting C++20 onwards ([msvc-wine](https://github.com/mstorsjo/msvc-wine) if on Linux)

Conan is used to setup the project and manage dependencies within ChaosMod. 
The `FL_CHAOS_MOD` environment variable can be set to inform chaos mod to copy any output DLL files after a build
to the specified directory.

On both Linux and Windows (bash or powershell) run the following commands to get setup:
```sh
git clone --recurse-submodules https://github.com/TheStarport/ChaosMod
cd ChaosMod
python -m venv .venv

# Pick next command depending on your environment:
# Bash (Linux): source ./.venv/bin/activate
# Powershell (Linux): ./.venv/bin/activate.ps1
# Bash (Windows): source ./.venv/Scripts/activate
# Powershell (Windows): ./.venv/Scripts/activate.ps1

pip install -r requirements.txt
```

After running the above commands you should be able to use the `cli.py` file in order to manage the repo.
In order run the following commands to generate a build:
```
python cli.py requirements
python cli.py build
```

The requirements command will download needed dependencies, while the build command will install conan packages and
prepare CMake presets. `-r` can be passed to the build command to prepare a release build on Windows. 
On the first run, a build will automatically be run to ensure everything works as intended.
If you wish to use the clang compiler, rather than MSVC, which has more curated diagnostics, 
append `--clang` to the build command.

After running the CLI, builds can be run manually via the following commands, like the previous, append `-clang` for
building using the clang compiler (for release, just change 'debug' to 'release'):
```bash
cmake --preset debug -S . -B ./build/Debug
cmake --build ./build/Debug --target ChaosMod --config Debug
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

Note: *These docs are not very fleshed out and provide the minimum amount of info*.

When running with Chaos Mod for the first time, no features are enabled. Everything must be configured in order for it
to have any effect. By default, the mod options menu is bound to middle mouse. In some cases this doesn't occur, but
the key for it can be rebound in the options menu.

From the mod options menu you can enable the various features and tweak them to your liking. 

### Twitch Integration

If using the Twitch Integration with the Chaos mode, a few more steps are required. First you open the Twitch Voting
Proxy application that ships with this mod, it should be located in the EXE folder. You need to run it once, and it should
then generate an empty JSON config file in the current working directory. Open this config file up for editing. It should
look something similar to this:

```json
{
  "votingMode": "Majority",
  "retainVotes": false,
  "twitchUsername": "YourTwitchName",
  "twitchChannelName": "YourTwitchName",
  "twitchToken": "Token",
  "permittedUsernames": [],
  "votingPrefix": null,
  "sendMessageUpdates": true
}
```

Briefly summarised:
- votingMode, can be either "Majority" or "Percentage". Majority is which ever has the most votes, if two options 
have the same amount it's 50/50. Percentage turns votes into ratios, e.g. if 100 people vote, 75 for option 1 and 25 for 
option 2. Option 1 has a 75% chance of being picked.
- retainVotes, votes for options that are not selected give that person 2 votes next round (does not stack beyond 2).
- twitchUsername, the name of the bot/user that will be in the channel counting votes
- twitchChannelName, the name of the channel that people will be typing their votes in
- twitchToken, the token for the aforementioned user/bot; users can obtain their token [here](https://twitchapps.com/tmi/)
- votingPrefix, by default votes are done by typing a number 1-8, this is the behaviour when this is set to null. You can
if desired set it to something like "!", which would mean to vote you'd type "!4", etc.
- sendMessageUpdates, when true a message will be piped into the Twitch Chat informing people when a voting round changes,
or when an option is successfully picked.

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
- [Kvemon](https://sketchfab.com/3d-models/gravity-gun-low-poly-pixel-art-8269194af2e547bf8662c8e4e3abd56e) - Gravity Gun Model
