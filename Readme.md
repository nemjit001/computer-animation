# BAMF (Beautifully Animated Mesh soFtware)

BAMF is a student project for the Computer Animation (MCANIM2023) course of Utrecht University.
It is part of the Game & Media Technology curriculum.

## Using BAMF

To use BAMF, first build your own binaries (ensure release mode to not wait an eternity for assets to load).

Note: For old versions of CMake, you may have to set the Startup Project to BAMF manually, as well as set the Debugging->Working Directory to "..\Glitter".

After creating your binaries, run BAMF in a folder where an "Assets" directory is available, with fbx models inside. These models will automatically be loaded into BAMF.

Animations will play by default in BAMF. To start and stop playback press "P".

The UI also allows changing light color and position, and switching between skinning and interpolation modes. Defaults are linear skinning and linear interpolation.

## Building BAMF

BAMF is completely CMake based. You can either generate a VS2022 solution based on the CMake project, or open the folder directly in VS2022 to
utilize its CMake project functionality.

All libraries needed for BAMF to function are included in the CMake project, meaning it should build fine out of the box.

BAMF is based on the [Glitter](https://github.com/Polytonic/Glitter) open source framework for OpenGL.

## Authors

- Tijmen Verhoef
- Panagiotis Vrettis
- Alkiviadis Pavlou
- August van Casteren

