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
