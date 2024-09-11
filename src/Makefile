# Variables
COMPILER = g++
SOURCE_FILE = temp.cpp
OUTPUT_FILE = main
SDL_LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf

# Default target
all: $(OUTPUT_FILE)

# Rule to build the executable
$(OUTPUT_FILE): $(SOURCE_FILE)
	$(COMPILER) $(SOURCE_FILE) -o $(OUTPUT_FILE) $(SDL_LIBS)

# Clean up build files
clean:
	rm -f $(OUTPUT_FILE)
