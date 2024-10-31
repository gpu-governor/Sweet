#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;
SDL_Event event;
SDL_Rect rect;
bool sw_active = true;  // Allows the program to run until the user quits

typedef struct {
    int cursor_x;
    int cursor_y;
    int padding;
    int window_width;
    int window_height;
} Layout_context;

Layout_context layout_context = {.cursor_x = 0, .cursor_y = 0, .padding = 5, .window_width = 500, .window_height = 500};

void updateWindowSize() {
    SDL_GetWindowSize(win, &layout_context.window_width, &layout_context.window_height);
}

// Color struct and color constants
typedef struct {
    Uint8 r, g, b, a;
} Color;

const Color BLACK = {0, 0, 0, 255};
const Color WHITE = {255, 255, 255, 255};
const Color GRAY = {128, 128, 128, 255};
const Color BLUE = {0, 0, 255, 255};

// Enums and structs for widgets
typedef enum {
    FILLED,
    OUTLINE
} ShapeType;

typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TEXT,
    WIDGET_DROP_DOWN,
    WIDGET_RADIO,
    WIDGET_RADIO_GROUP
} WidgetType;

typedef struct {
    SDL_Rect rect1;
    const char *text;
    int x, y, font_size;
    Color color, bcolor, hover_color;
    bool is_hovered, is_selected;
    int radius;
    bool is_end;
} CREATE;

typedef struct {
    WidgetType type;
    void *widget;
} Widget;

#define MAX_WIDGETS 100
Widget widgets[MAX_WIDGETS];
int widget_count = 0;

typedef struct {
    CREATE *radios;
    int radio_count;
} RadioGroup;

// SDL and external library initialization
bool sw_init(const char *title, int width, int height) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");  // Anisotropic filtering
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    win = SDL_CreateWindow(title, 100, 100, width, height, SDL_WINDOW_RESIZABLE);
    if (!win) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    return true;
}

void sw_quit() {
    if (ren) SDL_DestroyRenderer(ren);
    if (win) SDL_DestroyWindow(win);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

// Utility rendering functions
void sw_clear_screen(Color color) {
    SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
    SDL_RenderClear(ren);
}

void sw_present() {
    SDL_RenderPresent(ren);
}

void sw_background(Color color) {
    sw_clear_screen(color);
}

// Function to draw radio circle
void sw_draw_radio_circle(int x, int y, int radius, Color color) {
    SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(ren, x + dx, y + dy);
            }
        }
    }
}

// Function to create radio buttons
CREATE sw_create_radio(int x, int y, int radius, Color color, Color bcolor, Color hover_color, Color text_color, const char *text, int font_size) {
    CREATE radio;
    radio.x = x;
    radio.y = y;
    radio.radius = radius;
    radio.color = color;
    radio.bcolor = bcolor;
    radio.hover_color = hover_color;
    radio.text = strdup(text);
    radio.is_hovered = false;
    radio.font_size = font_size;
    radio.is_selected = false;
    radio.is_end = false;
    return radio;
}

void sw_register_widget(WidgetType type, void *widget) {
    if (widget_count < MAX_WIDGETS) {
        widgets[widget_count].type = type;
        widgets[widget_count].widget = widget;
        widget_count++;
    }
}

// Function to render radio buttons
void sw_render_radio(CREATE *radio) {
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    int dist = sqrt((mouse_x - radio->x) * (mouse_x - radio->x) + (mouse_y - radio->y) * (mouse_y - radio->y));
    radio->is_hovered = (dist <= radio->radius);

    sw_draw_radio_circle(radio->x, radio->y, radio->radius, radio->bcolor);

    if (radio->is_selected) {
        sw_draw_radio_circle(radio->x, radio->y, (int)(radio->radius * 0.7), radio->color);
    }

    if (radio->is_hovered) {
        sw_draw_radio_circle(radio->x, radio->y, radio->radius - 2, radio->hover_color);
    }

    TTF_Font *font = TTF_OpenFont("assets/FreeMono.ttf", radio->font_size);
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Surface *text_surface = TTF_RenderText_Blended(font, radio->text, (SDL_Color){radio->color.r, radio->color.g, radio->color.b, radio->color.a});
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
    SDL_Rect text_rect = {radio->x + radio->radius * 2, radio->y - text_surface->h / 2, text_surface->w, text_surface->h};
    SDL_RenderCopy(ren, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    TTF_CloseFont(font);
}

// Function to handle radio button events
void sw_handle_radio_event(SDL_Event *event, CREATE *radio, RadioGroup *group) {
    if (event->type == SDL_MOUSEBUTTONDOWN && radio->is_hovered) {
        // Deselect all other radios in the group
        for (int i = 0; i < group->radio_count; i++) {
            group->radios[i].is_selected = false;
        }
        // Select the clicked radio
        radio->is_selected = true;
    }
}

// Function to create a radio group
RadioGroup sw_create_radio_group(CREATE *radios, int radio_count) {
    RadioGroup group;
    group.radios = radios;
    group.radio_count = radio_count;
    return group;
}

// Function to handle multiple radio buttons
void sw_render_widgets() {
    for (int i = 0; i < widget_count; i++) {
        if (widgets[i].type == WIDGET_RADIO) {
            sw_render_radio((CREATE *)widgets[i].widget);
        }
    }
}

void sw_render_radio_group(RadioGroup *group) {
    for (int i = 0; i < group->radio_count; i++) {
        sw_render_radio(&group->radios[i]);
    }
}

void sw_render_all_radio_states(SDL_Event *event, RadioGroup *group) {
    for (int i = 0; i < group->radio_count; i++) {
        sw_handle_radio_event(event, &group->radios[i], group);
    }
}

void sw_loop(RadioGroup *group) {
    while (sw_active) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            sw_active = false;
        }
        sw_background(WHITE);
        sw_render_radio_group(group);
        sw_render_all_radio_states(&event, group);
        sw_present();
        SDL_Delay(10);
    }
}

int main(int argc, char *argv[]) {
    if (!sw_init("Radio Group Example", 500, 500)) return -1;

    CREATE radios[3];
    radios[0] = sw_create_radio(100, 100, 20, BLUE, BLACK, GRAY, BLACK, "Option 1", 18);
    radios[1] = sw_create_radio(100, 150, 20, BLUE, BLACK, GRAY, BLACK, "Option 2", 18);
    radios[2] = sw_create_radio(100, 200, 20, BLUE, BLACK, GRAY, BLACK, "Option 3", 18);

    RadioGroup group = sw_create_radio_group(radios, 3);

    sw_register_widget(WIDGET_RADIO_GROUP, &group);
    sw_loop(&group);

    sw_quit();
    return 0;
}
