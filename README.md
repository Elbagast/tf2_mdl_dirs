tf2_mdl_dirs
============

Simple program to solve a stupid problem with hlmv.exe when working with TF2.

Problem:
-----------------------------------------
TF2's model viewer HLMV (hlmv.exe) is a bit dumb, it can access all the model (.mdl) files found in the game's .vpk 
archives, but for some reason it can't open and browse the folders becasue they aren't real.

Solution:
-----------------------------------------
Wait for Valve to fix it.

Solution that doesn't rely on Valve Time:
-----------------------------------------
Make a program that scans the .vpk files and identifies all of the .mdl files, then creates a set of empty directories
in the 'custom' folder so that HLMV can browse the folders.

Usage
-----------------------------------------
Since this does a simple niche job the program is configured only to work when run from a specific directory:

[Steam Library]\SteamApps\common\Team Fortress 2\tf\custom

When run from this directory, it will look for TF2's version of 'vpk.exe' and the file 'tf2_misc_dir.vpk', then use
the former to generate a list of files inside the latter which is then analysed to create the appropriate directories.
Once all the .mdl files present in tf2_misc_dir.vpk have been identified, an empty directory structure is created 
inside the directory:

[as above]\tf\custom\mdl_directories

By creating them here, the game considers this directory to be a TF2 mod which happens to contain no files. HLMV can 
browse any folders found in TF2 mods, so this allows HLMV to browse all of the places it should be able to anyway.
