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
CREATE sw_text_entry(int x, int y, int w, int h, const char *placeholder, int font_size, SDL_Color text_color, SDL_Color placeholder_color, SDL_Color bg_color, SDL_Color outline_color, SDL_Color outline_color_selected, int style) {
    CREATE text_entry;
    text_entry.rect1.x = x;
    text_entry.rect1.y = y;
    text_entry.rect1.w = w;
    text_entry.rect1.h = h;
    text_entry.text = malloc(256);  // Allocate memory for the text
    text_entry.text[0] = '\0';      // Start with an empty string
    text_entry.font_size = font_size;
    text_entry.text_color = text_color;
    text_entry.placeholder_color = placeholder_color;
    text_entry.bg_color = bg_color;
    text_entry.outline_color = outline_color;
    text_entry.outline_color_selected = outline_color_selected;
    text_entry.style = style;
    text_entry.padding = 5;         // Default padding
    text_entry.width = w;
    text_entry.height = h;
    text_entry.place_holder = (char *)placeholder;
    text_entry.active = false;
    text_entry.cursor_position = 0;
    text_entry.show_cursor = true;
    text_entry.last_cursor_blink_time = SDL_GetTicks();
    text_entry.text_scroll_offset = 0;  // Initialize scroll offset
    return text_entry;
}

// Function to render the text box widget
void render_text_entry(CREATE *text_entry) {
    // Draw the background (the box)
    SDL_SetRenderDrawColor(renderer, text_entry->bg_color.r, text_entry->bg_color.g, text_entry->bg_color.b, text_entry->bg_color.a);
    SDL_RenderFillRect(renderer, &text_entry->rect1);

    // Draw the outline (light for inactive, dark for active)
    SDL_Color currentOutlineColor = text_entry->active ? text_entry->outline_color_selected : text_entry->outline_color;
    SDL_SetRenderDrawColor(renderer, currentOutlineColor.r, currentOutlineColor.g, currentOutlineColor.b, currentOutlineColor.a);
    SDL_RenderDrawRect(renderer, &text_entry->rect1);

    const char *displayText;
    SDL_Color currentColor;

    // If no text has been entered, display the placeholder text in pale color
    if (strlen(text_entry->text) == 0) {
        displayText = text_entry->place_holder;
        currentColor = text_entry->placeholder_color;
    } else {
        displayText = text_entry->text;
        currentColor = text_entry->text_color;
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
        text_entry->rect1.x + text_entry->padding,
        text_entry->rect1.y + text_entry->padding,
        text_entry->width - 2 * text_entry->padding,
        text_entry->height - 2 * text_entry->padding
    };
    SDL_RenderSetClipRect(renderer, &clipRect);  // Set the clipping region

    // Scroll the text horizontally if it exceeds the text box width
    int max_text_visible_width = text_entry->width - 2 * text_entry->padding;
    if (text_width > max_text_visible_width) {
        text_entry->text_scroll_offset = text_width - max_text_visible_width;
    } else {
        text_entry->text_scroll_offset = 0;
    }

    SDL_Rect textRect = {
        text_entry->rect1.x + text_entry->padding - text_entry->text_scroll_offset,  // Apply scroll offset
        text_entry->rect1.y + (text_entry->height - text_height) / 2,  // Vertically center the text
        text_width, text_height
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Render the blinking cursor if the text box is active
    if (text_entry->active && text_entry->show_cursor) {
        // Cursor should be positioned after the visible text, relative to scroll offset
        int cursor_x;
        if (strlen(text_entry->text) == 0) {
            cursor_x = text_entry->rect1.x + text_entry->padding;  // Start at the beginning if no text
        } else {
            int cursor_position_x = text_entry->rect1.x + text_entry->padding + text_width;
            cursor_x = cursor_position_x - text_entry->text_scroll_offset;  // Adjust based on scroll
        }
        int cursor_y = textRect.y;  // Align with the y-position of the text
        SDL_SetRenderDrawColor(renderer, text_entry->text_color.r, text_entry->text_color.g, text_entry->text_color.b, 255);
        SDL_RenderDrawLine(renderer, cursor_x, cursor_y, cursor_x, cursor_y + text_height);  // Adjust cursor height to match text height
    }

    SDL_RenderSetClipRect(renderer, NULL);  // Disable clipping

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}


// Function to handle text box input
void handle_input(CREATE *text_entry, SDL_Event *e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        // Check if mouse is within the text box
        int mouseX = e->button.x;
        int mouseY = e->button.y;
        if (mouseX >= text_entry->rect1.x && mouseX <= text_entry->rect1.x + text_entry->width &&
            mouseY >= text_entry->rect1.y && mouseY <= text_entry->rect1.y + text_entry->height) {
            text_entry->active = true;
        } else {
            text_entry->active = false;
        }
    } else if (e->type == SDL_TEXTINPUT && text_entry->active) {
        // Append new text to the text box's text
        strcat(text_entry->text, e->text.text);
        text_entry->cursor_position++;

        // Update the scroll offset if needed
        int text_width;
        TTF_SizeText(font, text_entry->text, &text_width, NULL);
        int max_text_visible_width = text_entry->width - 2 * text_entry->padding;
        if (text_width > max_text_visible_width) {
            text_entry->text_scroll_offset = text_width - max_text_visible_width;
        }
    } else if (e->type == SDL_KEYDOWN && text_entry->active) {
        // Handle backspace
        if (e->key.keysym.sym == SDLK_BACKSPACE && text_entry->cursor_position > 0) {
            text_entry->text[text_entry->cursor_position - 1] = '\0';
            text_entry->cursor_position--;
        }
    }

    // Handle cursor blinking
    if (SDL_GetTicks() - text_entry->last_cursor_blink_time >= 500) {
        text_entry->show_cursor = !text_entry->show_cursor;
        text_entry->last_cursor_blink_time = SDL_GetTicks();
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
	CREATE textBox=sw_text_entry(150, 150, 500, 50, "Enter text here...",24, textColor, placeholderColor, bgColor, outlineColor, outlineColorSelected, NORMAL);
	
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
        render_text_entry(&textBox);

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
