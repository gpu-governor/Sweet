#!/bin/bash

# using the makefile already provided is recommended but this bash script works too
# Variables
COMPILER="clang"
SOURCE_FILE="main.c"
OUTPUT_FILE="main"
SDL_LIBS="-lSDL2 -lSDL2_image -lSDL2_ttf"
STD_LIBS="-lm" #maths.h

# Compile command
$COMPILER $SOURCE_FILE -o $OUTPUT_FILE $SDL_LIBS $STD_LIBS
