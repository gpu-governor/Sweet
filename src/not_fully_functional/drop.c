#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <string.h>

#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40
#define OPTION_HEIGHT 30
#define MAX_OPTIONS 5

typedef struct {
    int x, y;
    int width, height;
    bool expanded;
    const char* options[MAX_OPTIONS];
    int selectedIndex;
} DropDown;

void DropDown_Init(DropDown* dropdown, int x, int y, const char* options[], int numOptions) {
    dropdown->x = x;
    dropdown->y = y;
    dropdown->width = BUTTON_WIDTH;
    dropdown->height = BUTTON_HEIGHT;
    dropdown->expanded = false;
    dropdown->selectedIndex = -1;

    for (int i = 0; i < numOptions && i < MAX_OPTIONS; ++i) {
        dropdown->options[i] = options[i];
    }
}

void RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void DropDown_Render(DropDown* dropdown, SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect buttonRect = {dropdown->x, dropdown->y, dropdown->width, dropdown->height};

    // Draw the button
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    // Draw the selected option or default text
    SDL_Color white = {255, 255, 255, 255};
    if (dropdown->selectedIndex >= 0) {
        RenderText(renderer, font, dropdown->options[dropdown->selectedIndex], dropdown->x + 10, dropdown->y + 10, white);
    } else {
        RenderText(renderer, font, "Select an option", dropdown->x + 10, dropdown->y + 10, white);
    }

    // If expanded, draw the list of options
    if (dropdown->expanded) {
        for (int i = 0; i < MAX_OPTIONS; ++i) {
            SDL_Rect optionRect = {dropdown->x, dropdown->y + dropdown->height + (i * OPTION_HEIGHT), dropdown->width, OPTION_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &optionRect);

            SDL_Color black = {0, 0, 0, 255};
            RenderText(renderer, font, dropdown->options[i], dropdown->x + 10, optionRect.y + 5, black);
        }
    }
}

void DropDown_HandleEvent(DropDown* dropdown, SDL_Event* event) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event->button.x;
        int mouseY = event->button.y;

        // Toggle expand/collapse when the main button is clicked
        if (mouseX >= dropdown->x && mouseX <= dropdown->x + dropdown->width &&
            mouseY >= dropdown->y && mouseY <= dropdown->y + dropdown->height) {
            dropdown->expanded = !dropdown->expanded;
        }
        // Check if clicking on an option when expanded
        else if (dropdown->expanded) {
            for (int i = 0; i < MAX_OPTIONS; ++i) {
                int optionY = dropdown->y + dropdown->height + (i * OPTION_HEIGHT);
                if (mouseX >= dropdown->x && mouseX <= dropdown->x + dropdown->width &&
                    mouseY >= optionY && mouseY <= optionY + OPTION_HEIGHT) {
                    dropdown->selectedIndex = i;  // Set the selected option index
                    dropdown->expanded = false;   // Collapse the dropdown
                    break;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();  // Initialize SDL_ttf

    SDL_Window* window = SDL_CreateWindow("SDL DropDown Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("assets/FreeMono.ttf", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return -1;
    }

    const char* options[] = {"Option 1", "Option 2", "Option 3", "Option 4", "Option 5"};
    DropDown dropdown;
    DropDown_Init(&dropdown, 220, 220, options, 5);

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            DropDown_HandleEvent(&dropdown, &event);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        DropDown_Render(&dropdown, renderer, font);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}