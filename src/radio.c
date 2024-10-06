#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

// GLOBAL VARIABLES
const char* font_path = "assets/FreeMono.ttf";

typedef struct {
    SDL_Rect rect;          // Rectangle for the radio button's position
    SDL_Color color;        // Color of the button when selected
    SDL_Color bcolor;       // Border color
    SDL_Color hover_color;  // Color when hovered
    SDL_Color text_color;   // Text color
    char *text;             // Text associated with the radio button
    int is_selected;        // Whether this button is selected
    int x, y;               // Position
    int radius;             // Radius of the circle
    bool is_hovered;        // Is the button hovered
} CREATE;

// Function to draw a filled circle using SDL_Renderer
void sw_draw_circle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

// Function to create a radio button
CREATE sw_create_radio(int x, int y, int radius, SDL_Color color, SDL_Color bcolor, SDL_Color hover_color, SDL_Color text_color, const char *text) {
    CREATE radio;
    radio.x = x;
    radio.y = y;
    radio.radius = radius;
    radio.color = color;
    radio.bcolor = bcolor;
    radio.hover_color = hover_color;
    radio.text_color = text_color;
    radio.text = strdup(text);
    radio.is_selected = 0;
    radio.is_hovered = false;

    return radio;
}

// Function to render a radio button
void sw_render_radio(SDL_Renderer *renderer, TTF_Font *font, CREATE *radio) {
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

    // Draw inner circle if selected
    if (radio->is_selected) {
        sw_draw_circle(renderer, radio->x, radio->y, radio->radius / 2, radio->color);
    }

    // Draw hover effect if hovered
    if (radio->is_hovered) {
        sw_draw_circle(renderer, radio->x, radio->y, radio->radius - 2, radio->hover_color);
    }

    // Render text next to the radio button
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, radio->text, radio->text_color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect text_rect = {radio->x + radio->radius * 2, radio->y - text_surface->h / 2, text_surface->w, text_surface->h};
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

// Function to handle radio button events
void sw_handle_radio_event(SDL_Event *event, CREATE *radio) {
    if (event->type == SDL_MOUSEBUTTONDOWN && radio->is_hovered) {
        radio->is_selected = 1; // Select this radio button
    }
}

// Function to handle grouped radio buttons
void sw_handle_radio_group_event(SDL_Event *event, CREATE radios[], int num_radios) {
    for (int i = 0; i < num_radios; i++) {
        if (event->type == SDL_MOUSEBUTTONDOWN && radios[i].is_hovered) {
            // Select this radio button and unselect others
            for (int j = 0; j < num_radios; j++) {
                radios[j].is_selected = (i == j);
            }
        }
    }
}

int main(int argc, char *argv[]) {
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
    
    TTF_Font *font = TTF_OpenFont(font_path, 24);
    if (font == NULL) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Create individual radio buttons (ungrouped)
    SDL_Color color = {100, 100, 250, 255};
    SDL_Color bcolor = {0, 0, 0, 255};
    SDL_Color hover_color = {150, 150, 255, 255};
    SDL_Color text_color = {0, 0, 0, 255};
    
    CREATE radio1 = sw_create_radio(100, 100, 15, color, bcolor, hover_color, text_color, "Option 1");
    CREATE radio2 = sw_create_radio(100, 150, 15, color, bcolor, hover_color, text_color, "Option 2");
    
    // Create grouped radio buttons
    CREATE group_radios[3];
    group_radios[0] = sw_create_radio(300, 100, 15, color, bcolor, hover_color, text_color, "Group Option A");
    group_radios[1] = sw_create_radio(300, 150, 15, color, bcolor, hover_color, text_color, "Group Option B");
    group_radios[2] = sw_create_radio(300, 200, 15, color, bcolor, hover_color, text_color, "Group Option C");

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
            sw_handle_radio_group_event(&event, group_radios, 3);
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Render individual radio buttons
        sw_render_radio(renderer, font, &radio1);
        sw_render_radio(renderer, font, &radio2);
        
        // Render grouped radio buttons
        for (int i = 0; i < 3; i++) {
            sw_render_radio(renderer, font, &group_radios[i]);
        }
        
        // Present the backbuffer
        SDL_RenderPresent(renderer);
    }
    
    // Cleanup
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
