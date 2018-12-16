# XNFS-Benchmark
This is an addon for Black Box's NFS games which adds Quake timedemo style benchmarking functionality to NFS games.

# Game support
Currently supports only:
- Carbon
- Most Wanted
- Underground 2 (unfinished)

Planned support for other games

# Usage
Place the .asi and .ini file in the scripts folder and run the game (preferably with a console enabled).
The game itself should be able to report the results through its dialog boxes, but you can get same information through the console window.

If you want to bring back the game to normal, you need to remove the plugin from the game.

# Accuracy
The accuracy *is* debatable, currently it's relying on killing as much as RNG as possible to get reproducible results, but this isn't properly implemented yet.
This needs to be reworked with JoyLog functionality. Right now it's relying on AI's driving.

Framerate/frametime counting code can be implemented a bit better as well, this is just a basic average calculation.

# Downloads
You should be able to download the files necessary in the Releases tab.

# Credits
ThirteenAG - for the injector and inireader.
EA Black Box - the game code itself
