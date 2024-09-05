#include "backend.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include"create.h"



// Function to initialize Text with default values
CREATE text(const char *text, int x, int y, int fontSize, Color color, int style) {
    // Update window size automatically
    updateWindowSize();

    CREATE newText;
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
int renderText(const CREATE *textProps) {
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

/* //example 1
int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize!\n");
        return -1;
    }

    // Initialize Text properties with style
    CREATE props = text("Hello, World!", -1, -1, 16, RED, NORMAL);

    // Modify properties if needed
    props.color = GREEN; // Change the color to GREEN
    props.style = ITALIC;  // Change the style to ITALIC

    // Render the text with the modified properties
    renderText(&props);
    
    // Add another text with auto alignment
    CREATE text2 = text("Auto align instead of x and y like ImGui", -1, -1, 16, RED, NORMAL);
    renderText(&text2);

    // Present the rendered output
    present();
    SDL_Delay(3000); // Delay to view the result
    
    terminate();
    return 0;
}
*/


//example 2

typedef struct {
    int x; // X position
    int y; // Y position
    int width; // Width of the text box
    int height; // Height of the text box
    Color color; // Background color
    Color outlineColor; // Border color
    int outlineThickness; // Border thickness
    char text[256]; // Text content (max length 255 characters)
    int cursorPos; // Position of the cursor in the text
    bool active; // Whether the text box is currently active (focused)
} TextBox;

TextBox create_text_box(int x, int y, int width, int height, Color color, Color outlineColor) {
    TextBox newTextBox;
    newTextBox.x = x;
    newTextBox.y = y;
    newTextBox.width = width;
    newTextBox.height = height;
    newTextBox.color = color;
    newTextBox.outlineColor = outlineColor;
    newTextBox.outlineThickness = 2; // Default thickness
    newTextBox.text[0] = '\0'; // Start with empty text
    newTextBox.cursorPos = 0; // Start cursor at the beginning
    newTextBox.active = false; // Not focused by default
    return newTextBox;
}

void render_text_box(TextBox *textBox) {
    // Draw the background rectangle
    draw_rectangle(textBox->color, textBox->width, textBox->height, textBox->x, textBox->y, FILLED);

    // Draw the outline (border)
    draw_rectangle(textBox->outlineColor, textBox->width, textBox->height, textBox->x, textBox->y, OUTLINE);

    // Render the text inside the text box
    CREATE textProps = text(textBox->text, textBox->x + 5, textBox->y + 5, 16, BLACK, NORMAL); // Example using black color and normal style
    renderText(&textProps);
}

int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize!\n");
        return -1;
    }

    // Initialize Text properties with style
    CREATE props = text("Hello, World!", 100, 150, 16, RED, NORMAL);

    // Modify properties if needed
    props.color = GREEN; // Change the color to GREEN
    props.style = ITALIC;  // Change the style to BOLD

    // Render the text with the modified properties
    renderText(&props);
    CREATE text2=text("auto align instead of x and y like imgui",set,set,16,RED,NORMAL);
    renderText(&text2);
    TextBox boxy=create_text_box(299, 200, 50, 50, WHITE, RED);
    
render_text_box(&boxy);
    present();
    SDL_Delay(3000); // Delay to view the result
    
    terminate();
    return 0;
}

