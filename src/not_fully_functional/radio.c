#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

// GLOBAL VARIABLES
const char* font_path = "assets/FreeMono.ttf";

typedef struct {
    SDL_Rect rect1;          // Rectangle for the radio button's position
    SDL_Color color;         // Color of the button when selected
    SDL_Color bcolor;        // Border color
    SDL_Color hover_color;   // Color when hovered
    SDL_Color text_color;    // Text color
    char *text;              // Text associated with the radio button
    int is_selected;         // Whether this button is selected
    int x, y;                // Position
    int radius;              // Radius of the circle
    bool is_hovered;         // Is the button hovered
    int font_size;           // Font size for the text
    bool is_end;             // Flag to indicate the end of a group
} CREATE;

// Function to draw a filled circle using SDL_Renderer
void sw_draw_circle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

// Function to create a radio button with smaller radius and font size parameter
CREATE sw_create_radio(int x, int y, int radius, SDL_Color color, SDL_Color bcolor, SDL_Color hover_color, SDL_Color text_color, const char *text, int font_size) {
    CREATE radio;
    radio.x = x;
    radio.y = y;
    radio.radius = radius; // Adjusted radius for smaller button size
    radio.color = color;
    radio.bcolor = bcolor;
    radio.hover_color = hover_color;
    radio.text_color = text_color;
    radio.text = strdup(text);
    radio.is_selected = 0;
    radio.is_hovered = false;
    radio.font_size = font_size; // Set font size
    radio.is_end = false; // By default, not the end of a group

    return radio;
}

// Modified rendering function with adjustable font size for radio buttons
void sw_render_radio(SDL_Renderer *renderer, CREATE *radio) {
    // Get mouse position
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    // Determine if hovered
    int dist = sqrt((mouse_x - radio->x) * (mouse_x - radio->x) + (mouse_y - radio->y) * (mouse_y - radio->y));
    if (dist <= radio->radius) {
        radio->is_hovered = true;
    } else {
        radio->is_hovered = false;
    }

    // Draw outer circle (border)
    sw_draw_circle(renderer, radio->x, radio->y, radio->radius, radio->bcolor);

    // Draw larger inner circle if selected (70% of the outer circle's radius)
    if (radio->is_selected) {
        sw_draw_circle(renderer, radio->x, radio->y, (int)(radio->radius * 0.7), radio->color);
    }

    // Draw hover effect if hovered
    if (radio->is_hovered) {
        sw_draw_circle(renderer, radio->x, radio->y, radio->radius - 2, radio->hover_color);
    }

    // Load font with the specific size for the radio button
    TTF_Font *font = TTF_OpenFont(font_path, radio->font_size);
    if (font == NULL) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return;
    }

    // Render text
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, radio->text, radio->text_color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect text_rect = {radio->x + radio->radius * 2, radio->y - text_surface->h / 2, text_surface->w, text_surface->h};
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    TTF_CloseFont(font); // Close font after rendering
}

// Function to handle radio button events
void sw_handle_radio_event(SDL_Event *event, CREATE *radio) {
    if (event->type == SDL_MOUSEBUTTONDOWN && radio->is_hovered) {
        radio->is_selected = 1; // Select this radio button
    }
}

// Function to handle grouped radio buttons with sentinel value
void sw_handle_radio_group_event(SDL_Event *event, CREATE radios[]) {
    for (int i = 0; !radios[i].is_end; i++) {
        if (event->type == SDL_MOUSEBUTTONDOWN && radios[i].is_hovered) {
            // Select this radio button and unselect others
            for (int j = 0; !radios[j].is_end; j++) {
                radios[j].is_selected = (i == j);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // "2" for anisotropic filtering

    // Initialize SDL and TTF
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    
    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow("SDL Radio Buttons", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Create individual radio buttons (ungrouped) with font size
    SDL_Color color = {100, 100, 250, 255};
    SDL_Color bcolor = {0, 0, 0, 255};
    SDL_Color hover_color = {150, 150, 255, 255};
    SDL_Color text_color = {0, 0, 0, 255};
    
    CREATE radio1 = sw_create_radio(100, 100, 12, color, bcolor, hover_color, text_color, "Option 1", 18);
    CREATE radio2 = sw_create_radio(100, 150, 12, color, bcolor, hover_color, text_color, "Option 2", 18);
    
    // Create grouped radio buttons with font size, and set sentinel value for last element
    CREATE group_radios[4]; // One extra for sentinel
    group_radios[0] = sw_create_radio(300, 100, 12, color, bcolor, hover_color, text_color, "Group Option A", 16);
    group_radios[1] = sw_create_radio(300, 150, 12, color, bcolor, hover_color, text_color, "Group Option B", 16);
    group_radios[2] = sw_create_radio(300, 200, 12, color, bcolor, hover_color, text_color, "Group Option C", 16);
    group_radios[3].is_end = true; // Set the sentinel flag for end of group

    // Main loop
    bool quit = false;
    SDL_Event event;
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            
            // Handle individual radio buttons
            sw_handle_radio_event(&event, &radio1);
            sw_handle_radio_event(&event, &radio2);
            
            // Handle grouped radio buttons
            sw_handle_radio_group_event(&event, group_radios);
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Render individual radio buttons
        sw_render_radio(renderer, &radio1);
        sw_render_radio(renderer, &radio2);
        
        // Render grouped radio buttons
        for (int i = 0; !group_radios[i].is_end; i++) {
            sw_render_radio(renderer, &group_radios[i]);
        }
        
        // Present the backbuffer
        SDL_RenderPresent(renderer);
    }
    
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    
    return 0;
}
