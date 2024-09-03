#include "backend.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

const char* fontPath = "FreeMono.ttf";

int set = -1;

// Define constants for text styles
const int NORMAL = TTF_STYLE_NORMAL;
const int BOLD = TTF_STYLE_BOLD;
const int ITALIC = TTF_STYLE_ITALIC;
const int UNDERLINE = TTF_STYLE_UNDERLINE;
const int STRIKETHROUGH = TTF_STYLE_STRIKETHROUGH;

// Define a structure to hold button properties
typedef struct {
    const char *text;
    int x;
    int y;
    int fontSize;
    Color color; // Color struct and constants already in backend.h
    Color bcolor; // Background color for the button
    Color outlineColor; // Border color for the button
    int style;   // Text style (NORMAL, BOLD, etc.)
    int padding; // Padding around the text
    int outlineThickness; // Thickness of the button border
} BUTTON;

// Function to initialize Button with default values
BUTTON createButton(const char *text, int x, int y, int fontSize, Color color, Color bcolor, Color outlineColor, int style) {
    // Update window size automatically
    updateWindowSize();

    BUTTON newButton;
    newButton.text = text;
    newButton.fontSize = fontSize;
    newButton.color = color;
    newButton.bcolor = bcolor;
    newButton.outlineColor = outlineColor;
    newButton.style = style;
    newButton.padding = 10;  // Set a default padding value
    newButton.outlineThickness = 2;  // Set default border thickness

    // Handle auto positioning
    if (x == -1) {  // -1 represents 'auto' for x
        newButton.x = layoutContext.cursorX;
    } else {
        newButton.x = x;
    }

    if (y == -1) {  // -1 represents 'auto' for y
        newButton.y = layoutContext.cursorY;
    } else {
        newButton.y = y;
    }

    // Update the layout context cursor for the next button
    layoutContext.cursorY += fontSize + layoutContext.padding; // Move down by the height of the font and padding

    return newButton;
}

// Function to render button
int renderButton(const BUTTON *buttonProps) {
    // Load font
    TTF_Font *font = TTF_OpenFont(fontPath, buttonProps->fontSize);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    // Set font style
    TTF_SetFontStyle(font, buttonProps->style);

    SDL_Color sdl_color = {buttonProps->color.r, buttonProps->color.g, buttonProps->color.b, buttonProps->color.a};

    // Render text to surface
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, buttonProps->text, sdl_color);
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

    // Calculate button dimensions with padding
    int padding = buttonProps->padding;
    int buttonWidth = textSurface->w + 2 * padding;
    int buttonHeight = textSurface->h + 2 * padding;

    // Draw the button background rectangle
    draw_rectangle(buttonProps->bcolor, buttonWidth, buttonHeight, buttonProps->x, buttonProps->y, FILLED);

    // Draw the button outline (border)
    draw_rectangle(buttonProps->outlineColor, buttonWidth, buttonHeight, buttonProps->x, buttonProps->y, OUTLINE);

    // Set the text position with padding
    SDL_Rect dst = {
        buttonProps->x + padding,       // x position with left padding
        buttonProps->y + padding,       // y position with top padding
        textSurface->w,                 // width of the text
        textSurface->h                  // height of the text
    };
    
    // Copy text texture to renderer
    SDL_RenderCopy(ren, textTexture, NULL, &dst);

    // Clean up the surface, texture, and font
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);

    return 0;
}

int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }
    background(GRAY);

    BUTTON button1 = createButton("click me", 30, 30, 50, RED, YELLOW, WHITE, BOLD);
    
    renderButton(&button1);

    // Present the rendered output
    present();
    SDL_Delay(10000);
    
    terminate();
    return 0;
}