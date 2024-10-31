#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

// GLOBAL VARIABLES
const char* font_path = "assets/FreeMono.ttf";

// Slider struct with required properties
typedef struct {
    int font_size;
    SDL_Rect panel_rect;    // Rect for the slider panel (background)
    SDL_Rect thumb_rect;    // Rect for the slider thumb
    SDL_Color panel_color;  // Color of the panel
    SDL_Color thumb_color;  // Color of the thumb
    SDL_Color hover_color;  // Color of the thumb when hovered
    SDL_Color text_color;   // Color of the text
    int range;              // Range of the slider (e.g., 0-100)
    float value;            // Current slider value
    int x, y;               // Position of the slider
    int width, height;      // Dimensions of the slider
    bool is_hovered;        // Is the thumb hovered
    bool is_dragging;       // Is the thumb being dragged
} Slider;

// Function to create a slider
Slider sw_slider(int x, int y, int width, int height, float range, SDL_Color panel_color, SDL_Color thumb_color, SDL_Color hover_color, SDL_Color text_color, int font_size) {
    Slider slider;
    slider.x = x;
    slider.y = y;
    slider.width = width;
    slider.height = height;
    slider.range = range;
    slider.value = 0.0f; // Start from 0 (beginning)
    slider.font_size=font_size;
    
    // Initialize panel (background)
    slider.panel_rect.x = x;
    slider.panel_rect.y = y;
    slider.panel_rect.w = width;
    slider.panel_rect.h = height;
    slider.panel_color = panel_color;
    
    // Initialize thumb
    slider.thumb_rect.x = x; // Start at the beginning
    slider.thumb_rect.y = y;
    slider.thumb_rect.w = height; // Make thumb a square
    slider.thumb_rect.h = height;
    slider.thumb_color = thumb_color;
    slider.hover_color = hover_color;

    slider.text_color = text_color;
    slider.is_hovered = false;
    slider.is_dragging = false;

    return slider;
}

// Function to render the slider
void sw_render_slider(SDL_Renderer *renderer, Slider *slider) {
    // Load the font each time you render
    TTF_Font *font = TTF_OpenFont(font_path, slider->font_size);
    if (font == NULL) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return; // Exit the function if the font fails to load
    }

    // Draw the panel (background)
    SDL_SetRenderDrawColor(renderer, slider->panel_color.r, slider->panel_color.g, slider->panel_color.b, slider->panel_color.a);
    SDL_RenderFillRect(renderer, &slider->panel_rect);

    // Determine if thumb is hovered
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    bool thumb_hovered = (mouse_x >= slider->thumb_rect.x && mouse_x <= (slider->thumb_rect.x + slider->thumb_rect.w) &&
                          mouse_y >= slider->thumb_rect.y && mouse_y <= (slider->thumb_rect.y + slider->thumb_rect.h));

    if (thumb_hovered || slider->is_dragging) {
        slider->is_hovered = true;
    } else {
        slider->is_hovered = false;
    }

    // Draw the thumb (change color if hovered)
    if (slider->is_hovered) {
        SDL_SetRenderDrawColor(renderer, slider->hover_color.r, slider->hover_color.g, slider->hover_color.b, slider->hover_color.a);
    } else {
        SDL_SetRenderDrawColor(renderer, slider->thumb_color.r, slider->thumb_color.g, slider->thumb_color.b, slider->thumb_color.a);
    }
    SDL_RenderFillRect(renderer, &slider->thumb_rect);

    // Render text showing the value
    char text[32];
    sprintf(text, "%d", (int)slider->value);

    // Set font to bold
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);

    // Use Blended text for sharper rendering
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, slider->text_color);
    if (text_surface == NULL) {
        printf("TTF_RenderText Error: %s\n", TTF_GetError());
        TTF_CloseFont(font);  // Make sure to close the font before returning
        return;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (text_texture == NULL) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    } else {
        SDL_Rect text_rect = {slider->x + slider->width / 2 - text_surface->w / 2, slider->y + slider->height / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
        SDL_DestroyTexture(text_texture);
    }

    SDL_FreeSurface(text_surface);
    
    // Close the font after rendering
    TTF_CloseFont(font);
}

// Function to handle slider events
void sw_handle_slider_event(SDL_Event *event, Slider *slider) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (slider->is_hovered) {
            slider->is_dragging = true;
        }
    }
    
    if (event->type == SDL_MOUSEBUTTONUP) {
        slider->is_dragging = false;
    }
    
    if (slider->is_dragging && event->type == SDL_MOUSEMOTION) {
        int mouse_x = event->motion.x;
        
        // Move the thumb and adjust the value based on the mouse position
        slider->thumb_rect.x = mouse_x - slider->thumb_rect.w / 2;
        
        // Clamp the thumb to the slider bounds
        if (slider->thumb_rect.x < slider->panel_rect.x) {
            slider->thumb_rect.x = slider->panel_rect.x;
        }
        if (slider->thumb_rect.x > slider->panel_rect.x + slider->panel_rect.w - slider->thumb_rect.w) {
            slider->thumb_rect.x = slider->panel_rect.x + slider->panel_rect.w - slider->thumb_rect.w;
        }
        
        // Update the value based on the thumb position
        slider->value = ((float)(slider->thumb_rect.x - slider->panel_rect.x) / (float)(slider->panel_rect.w - slider->thumb_rect.w)) * slider->range;
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
    
    SDL_Window *window = SDL_CreateWindow("SDL Slider", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
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
    
    // Create slider
    SDL_Color panel_color = {200, 200, 200, 255};
    SDL_Color thumb_color = {100, 100, 250, 255};
    SDL_Color hover_color = {150, 150, 255, 255};
    SDL_Color text_color = {0, 0, 0, 255};
    
    Slider slider = sw_slider(100, 200, 400, 20, 100.0f, panel_color, thumb_color, hover_color, text_color, 24);
    
    // Main loop
    bool quit = false;
    SDL_Event event;
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            
            // Handle slider event
            sw_handle_slider_event(&event, &slider);
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 25, 99, 255);
        SDL_RenderClear(renderer);
        
        // Render slider
        sw_render_slider(renderer,  &slider);
        
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
