# Variables
COMPILER = clang
SOURCE_FILE = entry.c
OUTPUT_FILE = main
SDL_LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf
STD_LIBS= -lm #maths.h
# Default target
all: $(OUTPUT_FILE)

# Rule to build the executable
$(OUTPUT_FILE): $(SOURCE_FILE)
	$(COMPILER) $(SOURCE_FILE) -o $(OUTPUT_FILE) $(SDL_LIBS) $(STD_LIBS)

# Clean up build files
clean:
	rm -f $(OUTPUT_FILE)
