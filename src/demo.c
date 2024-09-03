#include"backend.h"
#include<SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char* path="FreeMono.ttf";

// Define a structure to hold text properties
typedef struct {
    const char *text;
    int x;
    int y;
    int fontSize;
    Color color; // color struct and constant already in backend.h
} TEXT;

// Function to initialize Text with default values
TEXT write(const char *text, int x, int y, int fontSize, Color color) {
    TEXT new;
    new.text = text;
    new.x = x;
    new.y = y;
    new.fontSize = fontSize;
    new.color = color;
    
    return new;
}

// Function to render text based on the Text structure
int renderText(const TEXT *new) {
    
    // Load font
    font = TTF_OpenFont(path, new->fontSize);
    // rendering
    SDL_Color sdl_color = {color.r, color.g, color.b, color.a};

    // Render text to surface
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, sdl_color);
    if (!textSurface) {
        printf("Unable to render text surface! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    // Create a Texture from the Surface
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
    if (!textTexture) {
        printf("Unable to create texture from rendered text! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return -1;
    }
	int w=10;
	int h=10;
	x=new->x;
	y=new->y;
    // Render the text
    SDL_Rect dst = { x, y, w, h };
    SDL_RenderCopy(ren, textTexture, NULL, &dst);

    // Clean up the surface and texture
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
	return 0;
}

int main() {
    init("Gui",500,500);
    // Initialize Text properties
    TEXT props = write("Hello, World!", 100, 150, 16, RED);
    
    // Modify properties if needed
    // Example: change the color
    props.color = WHITE; // New color (Green)

    // Render the text with updated properties
    renderText(&props);
	terminate();
    return 0;
}