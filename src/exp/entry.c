#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// GLOBAL VARIABLES
const char* font_path = "assets/FreeMono.ttf";
const int NORMAL = TTF_STYLE_NORMAL;
const int BOLD = TTF_STYLE_BOLD;
const int ITALIC = TTF_STYLE_ITALIC;
const int UNDERLINE = TTF_STYLE_UNDERLINE;
const int STRIKETHROUGH = TTF_STYLE_STRIKETHROUGH;

typedef struct {
    SDL_Rect rect1;     // Rectangle for the widget
    char *text;         // Text displayed inside the widget
    int x, y;           // Position of the widget
    int font_size;      // Font size for the text
    SDL_Color text_color;   // Text color
    SDL_Color placeholder_color;  // Color for the placeholder text
    SDL_Color bg_color;     // Background color for the text box
    SDL_Color outline_color;  // Outline color for the text box
    SDL_Color outline_color_selected; // Outline color when selected
    int style;          // Text style (NORMAL, BOLD, etc.)
    int padding;        // Padding around the text inside the box
    int width, height;  // Dimensions of the box
    char *place_holder; // Placeholder text for the text box
    bool active;        // Track if the text box is active or clicked
    int cursor_position; // Cursor position for text input
    bool show_cursor;    // Flag to control blinking cursor
    Uint32 last_cursor_blink_time; // Time tracking for cursor blinking
    int text_scroll_offset;  // Horizontal offset to enable text scrolling
} CREATE;

SDL_Renderer *renderer;
TTF_Font *font = NULL;

// Function to create a text box widget
CREATE sw_text_box(int x, int y, int w, int h, const char *placeholder, int font_size, SDL_Color text_color, SDL_Color placeholder_color, SDL_Color bg_color, SDL_Color outline_color, SDL_Color outline_color_selected, int style) {
    CREATE textBox;
    textBox.rect1.x = x;
    textBox.rect1.y = y;
    textBox.rect1.w = w;
    textBox.rect1.h = h;
    textBox.text = malloc(256);  // Allocate memory for the text
    textBox.text[0] = '\0';      // Start with an empty string
    textBox.font_size = font_size;
    textBox.text_color = text_color;
    textBox.placeholder_color = placeholder_color;
    textBox.bg_color = bg_color;
    textBox.outline_color = outline_color;
    textBox.outline_color_selected = outline_color_selected;
    textBox.style = style;
    textBox.padding = 5;         // Default padding
    textBox.width = w;
    textBox.height = h;
    textBox.place_holder = (char *)placeholder;
    textBox.active = false;
    textBox.cursor_position = 0;
    textBox.show_cursor = true;
    textBox.last_cursor_blink_time = SDL_GetTicks();
    int text_scroll_offset;  // Horizontal offset to enable text scrolling
    textBox.text_scroll_offset = 0;  // Initialize scroll offset
    return textBox;
}

// Function to render the text box widget
void render_text_box(CREATE *textBox) {
    // Draw the background (the box)
    SDL_SetRenderDrawColor(renderer, textBox->bg_color.r, textBox->bg_color.g, textBox->bg_color.b, textBox->bg_color.a);
    SDL_RenderFillRect(renderer, &textBox->rect1);

    // Draw the outline (light for inactive, dark for active)
    SDL_Color currentOutlineColor = textBox->active ? textBox->outline_color_selected : textBox->outline_color;
    SDL_SetRenderDrawColor(renderer, currentOutlineColor.r, currentOutlineColor.g, currentOutlineColor.b, currentOutlineColor.a);
    SDL_RenderDrawRect(renderer, &textBox->rect1);

    const char *displayText;
    SDL_Color currentColor;

    // If no text has been entered, display the placeholder text in pale color
    if (strlen(textBox->text) == 0) {
        displayText = textBox->place_holder;
        currentColor = textBox->placeholder_color;
    } else {
        displayText = textBox->text;
        currentColor = textBox->text_color;
    }

    // Render the text (or placeholder)
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, displayText, currentColor);
    if (!textSurface) {
        printf("Failed to create text surface: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        printf("Failed to create text texture: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }
    int text_width, text_height;
    SDL_QueryTexture(textTexture, NULL, NULL, &text_width, &text_height);

    // Define the clipping area to ensure text doesn't overflow the box
    SDL_Rect clipRect = {
        textBox->rect1.x + textBox->padding,
        textBox->rect1.y + textBox->padding,
        textBox->width - 2 * textBox->padding,
        textBox->height - 2 * textBox->padding
    };
    SDL_RenderSetClipRect(renderer, &clipRect);  // Set the clipping region

    // Scroll the text horizontally if it exceeds the text box width
    int max_text_visible_width = textBox->width - 2 * textBox->padding;
    if (text_width > max_text_visible_width) {
        textBox->text_scroll_offset = text_width - max_text_visible_width;
    } else {
        textBox->text_scroll_offset = 0;
    }

    SDL_Rect textRect = {
        textBox->rect1.x + textBox->padding - textBox->text_scroll_offset,  // Apply scroll offset
        textBox->rect1.y + (textBox->height - text_height) / 2,  // Vertically center the text
        text_width, text_height
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Render the blinking cursor if the text box is active
    if (textBox->active && textBox->show_cursor) {
        // Cursor should be positioned after the visible text, relative to scroll offset
        int cursor_x;
        if (strlen(textBox->text) == 0) {
            cursor_x = textBox->rect1.x + textBox->padding;  // Start at the beginning if no text
        } else {
            int cursor_position_x = textBox->rect1.x + textBox->padding + text_width;
            cursor_x = cursor_position_x - textBox->text_scroll_offset;  // Adjust based on scroll
        }
        int cursor_y = textRect.y;  // Align with the y-position of the text
        SDL_SetRenderDrawColor(renderer, textBox->text_color.r, textBox->text_color.g, textBox->text_color.b, 255);
        SDL_RenderDrawLine(renderer, cursor_x, cursor_y, cursor_x, cursor_y + text_height);  // Adjust cursor height to match text height
    }

    SDL_RenderSetClipRect(renderer, NULL);  // Disable clipping

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}


// Function to handle text box input
void handle_input(CREATE *textBox, SDL_Event *e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        // Check if mouse is within the text box
        int mouseX = e->button.x;
        int mouseY = e->button.y;
        if (mouseX >= textBox->rect1.x && mouseX <= textBox->rect1.x + textBox->width &&
            mouseY >= textBox->rect1.y && mouseY <= textBox->rect1.y + textBox->height) {
            textBox->active = true;
        } else {
            textBox->active = false;
        }
    } else if (e->type == SDL_TEXTINPUT && textBox->active) {
        // Append new text to the text box's text
        strcat(textBox->text, e->text.text);
        textBox->cursor_position++;

        // Update the scroll offset if needed
        int text_width;
        TTF_SizeText(font, textBox->text, &text_width, NULL);
        int max_text_visible_width = textBox->width - 2 * textBox->padding;
        if (text_width > max_text_visible_width) {
            textBox->text_scroll_offset = text_width - max_text_visible_width;
        }
    } else if (e->type == SDL_KEYDOWN && textBox->active) {
        // Handle backspace
        if (e->key.keysym.sym == SDLK_BACKSPACE && textBox->cursor_position > 0) {
            textBox->text[textBox->cursor_position - 1] = '\0';
            textBox->cursor_position--;
        }
    }

    // Handle cursor blinking
    if (SDL_GetTicks() - textBox->last_cursor_blink_time >= 500) {
        textBox->show_cursor = !textBox->show_cursor;
        textBox->last_cursor_blink_time = SDL_GetTicks();
    }
}
// Main function
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        printf("Failed to initialize SDL or SDL_ttf: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2 Text Box with Cursor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load font
    font = TTF_OpenFont(font_path, 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return -1;
    }

    SDL_Color textColor = {0, 0, 0, 255};  // Black text
    SDL_Color placeholderColor = {200, 200, 200, 255};  // Pale gray for placeholder
    SDL_Color bgColor = {255, 255, 255, 255};  // White background
    SDL_Color outlineColor = {150, 150, 150, 255};  // Light outline when not selected
    SDL_Color outlineColorSelected = {50, 50, 50, 255};  // Dark outline when selected
   // CREATE textBox = sw_text_box(100, 100, 300, 50, "Enter text here...", 24, textColor, placeholderColor, bgColor, NORMAL);
	CREATE textBox=sw_text_box(150, 150, 500, 50, "Enter text here...",24, textColor, placeholderColor, bgColor, outlineColor, outlineColorSelected, NORMAL);
	
    SDL_StartTextInput();
    bool quit = false;
    SDL_Event e;
    
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            handle_input(&textBox, &e);
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderClear(renderer);

        // Render the text box
        render_text_box(&textBox);

        // Update screen
        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();

    return 0;
}
