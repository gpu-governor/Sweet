#!/bin/bash

# Variables
COMPILER="g++"
SOURCE_FILE="showcase.cc"
OUTPUT_FILE="main"
SDL_LIBS="-lSDL2 -lSDL2_image -lSDL2_ttf"

# Compile command
$COMPILER $SOURCE_FILE -o $OUTPUT_FILE $SDL_LIBS
