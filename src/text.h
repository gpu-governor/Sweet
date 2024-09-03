#include "backend.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

const char* fontPath = "FreeMono.ttf";

int set=-1;

// Define constants for text styles
const int NORMAL = TTF_STYLE_NORMAL;
const int BOLD = TTF_STYLE_BOLD;
const int ITALIC = TTF_STYLE_ITALIC;
const int UNDERLINE = TTF_STYLE_UNDERLINE;
const int STRIKETHROUGH = TTF_STYLE_STRIKETHROUGH;

// Define a structure to hold text properties
typedef struct {
    const char *text;
    int x;
    int y;
    int fontSize;
    Color color; // Color struct and constants already in backend.h
    int style;   // Text style (NORMAL, BOLD, etc.)
} TEXT;



// Function to initialize Text with default values
TEXT write(const char *text, int x, int y, int fontSize, Color color, int style) {
    // Update window size automatically
    updateWindowSize();

    TEXT newText;
    newText.text = text;
    newText.fontSize = fontSize;
    newText.color = color;
    newText.style = style;

    // Handle auto positioning
    if (x == -1) {  // -1 represents 'auto' for x
        newText.x = layoutContext.cursorX;
    } else {
        newText.x = x;
    }

    if (y == -1) {  // -1 represents 'auto' for y
        newText.y = layoutContext.cursorY;
    } else {
        newText.y = y;
    }

    // Update the layout context cursor for the next text
    layoutContext.cursorY += fontSize + layoutContext.padding; // Move down by the height of the font and padding

    return newText;
}

// Function to render text based on the Text structure
int renderText(const TEXT *textProps) {
    // Load font
    TTF_Font *font = TTF_OpenFont(fontPath, textProps->fontSize);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    // Set font style
    TTF_SetFontStyle(font, textProps->style);

    SDL_Color sdl_color = {textProps->color.r, textProps->color.g, textProps->color.b, textProps->color.a};

    // Render text to surface
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, textProps->text, sdl_color);
    if (!textSurface) {
        printf("Unable to render text surface! TTF_Error: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return -1;
    }

    // Create a Texture from the Surface
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
    if (!textTexture) {
        printf("Unable to create texture from rendered text! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        return -1;
    }

    // Set the rectangle based on the surface size
    SDL_Rect dst = { textProps->x, textProps->y, textSurface->w, textSurface->h };
    SDL_RenderCopy(ren, textTexture, NULL, &dst);

    // Clean up the surface, texture, and font
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);

    return 0;
}
/* example 1

int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize!\n");
        return -1;
    }

    // Initialize Text properties with style
    TEXT props = write("Hello, World!", -1, -1, 16, RED, NORMAL);

    // Modify properties if needed
    props.color = GREEN; // Change the color to GREEN
    props.style = ITALIC;  // Change the style to ITALIC

    // Render the text with the modified properties
    renderText(&props);
    
    // Add another text with auto alignment
    TEXT text2 = write("Auto align instead of x and y like ImGui", -1, -1, 16, RED, NORMAL);
    renderText(&text2);

    // Present the rendered output
    present();
    SDL_Delay(3000); // Delay to view the result
    
    terminate();
    return 0;
}

*/

/* example 2



int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize!\n");
        return -1;
    }

    // Initialize Text properties with style
    TEXT props = write("Hello, World!", 100, 150, 16, RED, NORMAL);

    // Modify properties if needed
    props.color = GREEN; // Change the color to GREEN
    props.style = ITALIC;  // Change the style to BOLD

    // Render the text with the modified properties
    renderText(&props);
    TEXT text2=write("auto align instead of x and y like imgui",set,set,16,RED,NORMAL);
    renderText(&text2);
    present();
    SDL_Delay(3000); // Delay to view the result
    
    terminate();
    return 0;
}

*/