#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Global variables
SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;
SDL_Event event;
SDL_Rect rect; // so for hover and pressed can detect mouse over widgets rect (buttons, ...) 

bool sw_active = true; // so you can use while (active) { // SDL events }
// layout context
//  function to update the window size in the LayoutContext
// Define the LayoutContext struct globally
typedef struct {
    int cursor_x;       // Current X position
    int cursor_y;       // Current Y position
    int padding;       // Padding between widgets
    int window_width;   // Width of the window
    int window_height;  // Height of the window
} Layout_context;

// Initialize the LayoutContext instance globally
Layout_context layout_context = { .cursor_x = 0, .cursor_y = 0, .padding = 5, .window_width = 500, .window_height = 500 };

 void updateWindowSize() {
    SDL_GetWindowSize(win, &layout_context.window_width, &layout_context.window_height);
}

//end layout context
// =============INITIALIZATION, WINDOW, EVENTS===========================

bool sw_init(const char *title, int width, int height) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // "2" for anisotropic filtering
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    win = SDL_CreateWindow(title, 100, 100, width, height, SDL_WINDOW_RESIZABLE);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Init external libs
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
    if (ren) {
        SDL_DestroyRenderer(ren);
    }
    if (win) {
        SDL_DestroyWindow(win);
    }
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


// =============COLOR, DRAW (RECT, TRI, CIR), RENDER RELATED FUNCTIONS===========================

typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} Color;

// Define color constants
const Color BLACK = {0, 0, 0, 255};
const Color WHITE = {255, 255, 255, 255};
const Color RED = {255, 0, 0, 255};
const Color GREEN = {0, 255, 0, 255};
const Color BLUE = {0, 0, 255, 255};
const Color YELLOW = {255, 255, 0, 255};
const Color CYAN = {0, 255, 255, 255};
const Color MAGENTA = {255, 0, 255, 255};
const Color GRAY = {128, 128, 128, 255};
const Color DARK_GRAY = {64, 64, 64, 255};
const Color LIGHT_GRAY = {192, 192, 192, 255};
const Color ORANGE = {255, 165, 0, 255};
const Color PURPLE = {128, 0, 128, 255};
const Color PINK = {255, 192, 203, 255};
const Color BROWN = {139, 69, 19, 255};

typedef enum {
    FILLED,
    OUTLINE
} ShapeType;

void sw_draw_rectangle(Color color, int width, int height, int xpos, int ypos, ShapeType type) {
    rect.x = xpos; // X position
    rect.y = ypos; // Y position
    rect.w = width; // Width
    rect.h = height; // Height

    SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a); // Rectangle color
    // Draw based on the type
    if (type == FILLED) {
        SDL_RenderFillRect(ren, &rect);
    } else if (type == OUTLINE) {
        SDL_RenderDrawRect(ren, &rect);
    }
}


// =====================CLEAR SCREEN, REFRESH AND PRESENT FUNCTION==========================

void sw_clear_screen(Color color) {
    SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
    SDL_RenderClear(ren);
}

void sw_present() {
    SDL_RenderPresent(ren);
}

void sw_background(Color color){
	sw_clear_screen(color);
}

//========================================================
#define MAX_OPTIONS 5 // Maximum number of options for dropdown
// GLOBAL VARIABLES
const char* font_path = "assets/FreeMono.ttf";
const int set = -1;

// In HTML, the default font sizes for heading elements (`h1`, `h2`, etc.) are typically defined relative to the browser's default font size, which is usually `16px`. Here's the approximate mapping of default heading sizes to pixel values in most browsers:
const int h1 =32, h2=24, h3=19,h4=16,h5= 13,h6=11; int p=16;
// when using the text(), if users don't want to enter font size, the can use predefined html tags.

// GLOBAL MACROS
#define proceed (void)0 // Define a macro for no-op
// Define constants for text styles
const int NORMAL = TTF_STYLE_NORMAL;
const int BOLD = TTF_STYLE_BOLD;
const int ITALIC = TTF_STYLE_ITALIC;
const int UNDERLINE = TTF_STYLE_UNDERLINE;
const int STRIKETHROUGH = TTF_STYLE_STRIKETHROUGH;

// Define a structure to hold widgets (button, label) properties
typedef struct {
    // general (buttons, text, grid, ...)
    SDL_Rect rect1;     // Unique rectangle for each button
    const char *text; // i removed const incase i get an error
    int x;
    int y;
    int font_size;
    Color color; 
    bool is_clicked;
    bool is_hovered;
    bool is_pressed;
    
    Color shadow_color;
    //  ... complex general
    int alignment;
    
     
    // button specific
	Color bcolor; // Background color for the button
    Color hover_color; // Color when hovered
    int style;   // Text style (NORMAL, BOLD, etc.)
    int padding; // Padding around the text
    int outline_thickness; // Thickness of the button border
    int width;  // Width of the button
    int height; // Height of the button
    // slider specific
    int type;

        // Dropdown specific
    bool expanded;
    const char* options[MAX_OPTIONS];
    int selected_index;

    // slider specific
    SDL_Rect panel_rect;    
    SDL_Rect thumb_rect;  
    Color panel_color;  
    Color thumb_color;  
    int range;           
    float value;          
    bool is_dragging;            // Flag to indicate the end of a group

    //text entry
    char *entry_text;
    Color placeholder_color;  // Color for the placeholder text
    Color outline_color;  // Outline color for the text box
    Color outline_color_selected; // Outline color when selected
    char *place_holder; // Placeholder text for the text box
    bool active;        // Track if the text box is active or clicked
    int cursor_position; // Cursor position for text input
    bool show_cursor;    // Flag to control blinking cursor
    Uint32 last_cursor_blink_time; // Time tracking for cursor blinking
    int text_scroll_offset;  // Horizontal offset to enable text scrolling
} CREATE;

typedef struct{
	int type; // can be hover, outline
	Color color;
}EFFECT;




//===================RETAINED MODE SYSTEM==========================
int mouse_x, mouse_y;
// clicked? may be deprecated for the pressed function
bool sw_clicked(CREATE *item) {

    SDL_Point pt = {mouse_x, mouse_y};
    
    if (SDL_PointInRect(&pt, &item->rect1)) {
        item->is_clicked = true;
        return true;
    } else {
        item->is_clicked = false;
        return false;
    }
}
//----------------------------------------------


typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TEXT,
    WIDGET_DROP_DOWN,
    WIDGET_SLIDER,
    WIDGET_RADIO,
    WIDGET_RADIO_GROUP,
    WIDGET_ENTRY
} WidgetType;

typedef struct {
    WidgetType type;
    void* widget;  // Pointer to the actual widget (Button, Label, or Text)
} Widget;

#define MAX_WIDGETS 100
Widget widgets[MAX_WIDGETS];
int widget_count = 0;


void sw_register_widget(WidgetType type, void* widget) {
// this function is called internally when a new widgets is created
/*
When users call button(), label(), or text(), these will register their widgets internally:

Button button(parameters) {
    Button new_button = create_button(text, x, y, font_size, fg, bg, style);
    register_widget(WIDGET_BUTTON, &new_button); <<<<<< registering
    return new_button;
}

*/
    if (widget_count < MAX_WIDGETS) {
        widgets[widget_count].type = type;
        widgets[widget_count].widget = widget;
        widget_count++;
    }
}
//=======================================================






// ====================TEXT======================
// Function to initialize Text with default values
CREATE sw_text(const char *text, int x, int y, int font_size, Color color, int style) {
    // Update window size automatically
    updateWindowSize();

    CREATE new_text;
    new_text.text = text;
    new_text.font_size = font_size;
    new_text.color = color;
    new_text.style = style;

    // Handle auto positioning
    if (x == -1) {  // -1 represents 'auto' for x
        new_text.x = layout_context.cursor_x;
    } else {
        new_text.x = x;
    }

    if (y == -1) {  // -1 represents 'auto' for y
        new_text.y = layout_context.cursor_y;
    } else {
        new_text.y = y;
    }

    // Update the layout context cursor for the next text
    layout_context.cursor_y += font_size + layout_context.padding; // Move down by the height of the font and padding

    
    // registers widgets so sw_loop() can keep track of it, see render_widgets() for more details
    	sw_register_widget(WIDGET_TEXT, &new_text);
    
    	//return new_widgets so it can be used in render 
        return new_text;
}

// Function to render text based on the Text structure
int sw_render_text(const CREATE *text_properties) {
    // Load font
    TTF_Font *font = TTF_OpenFont(font_path, text_properties->font_size);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    // Set font style
    TTF_SetFontStyle(font, text_properties->style);

    SDL_Color sdl_color = {text_properties->color.r, text_properties->color.g, text_properties->color.b, text_properties->color.a};

    // Render text to surface
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text_properties->text, sdl_color);
    if (!text_surface) {
        printf("Unable to render text surface! TTF_Error: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return -1;
    }

    // Create a Texture from the Surface
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
    if (!text_texture) {
        printf("Unable to create texture from rendered text! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        TTF_CloseFont(font);
        return -1;
    }

    // Set the rectangle based on the surface size
    SDL_Rect dst = { text_properties->x, text_properties->y, text_surface->w, text_surface->h };
    SDL_RenderCopy(ren, text_texture, NULL, &dst);

    // Clean up the surface, texture, and font
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    TTF_CloseFont(font);

    return 0;
}
// ------------------------------------complex text----------------------------
CREATE sw_complex_text(const char *text, int x, int y, int font_size, Color color, int style, int alignment, EFFECT effect) {
    
    
}
//------------------------------------write-----------------------------
CREATE sw_write(int font_size,const char *text) {
    // Update window size automatically
    updateWindowSize();

	//default values
	Color color= BLACK;
	int style= NORMAL;
	int x=-1;
	int y= -1; // set
	
    CREATE new_text;
    new_text.text = text;
    new_text.font_size = font_size;
    new_text.color = color;
    new_text.style = style;

    // Handle auto positioning
    if (x == -1) {  // -1 represents 'auto' for x
        new_text.x = layout_context.cursor_x;
    } else {
        new_text.x = x;
    }

    if (y == -1) {  // -1 represents 'auto' for y
        new_text.y = layout_context.cursor_y;
    } else {
        new_text.y = y;
    }

    // Update the layout context cursor for the next text
    layout_context.cursor_y += font_size + layout_context.padding; // Move down by the height of the font and padding

    return new_text;
}


//===============================BUTTONS============================================
// Function to calculate a darker shadow color
Color calculateShadowColor(Color color) {
    Color shadow_color;
    int darken_factor = 100;
    shadow_color.r = (color.r > darken_factor) ? color.r - darken_factor : 0;
    shadow_color.g = (color.g > darken_factor) ? color.g - darken_factor : 0;
    shadow_color.b = (color.b > darken_factor) ? color.b - darken_factor : 0;
    shadow_color.a = color.a;
    return shadow_color;
}

// Function to initialize a button
CREATE sw_button(const char *text, int x, int y, int font_size, Color color, Color bcolor, int style) {
    
    // Update window size automatically
    updateWindowSize();
    
    CREATE new_button;
    new_button.text = text;
    new_button.font_size = font_size;
    new_button.color = color;
    new_button.bcolor = bcolor;
    new_button.style = style;
    new_button.padding = 10;  // Padding for text inside the button
    new_button.is_hovered = 0;
    new_button.is_clicked = 0;
    new_button.is_pressed = 0;

    // Calculate hover color (lighter)
    const int lighten_factor = 50;
    new_button.hover_color.r = (bcolor.r + lighten_factor > 255) ? 255 : bcolor.r + lighten_factor;
    new_button.hover_color.g = (bcolor.g + lighten_factor > 255) ? 255 : bcolor.g + lighten_factor;
    new_button.hover_color.b = (bcolor.b + lighten_factor > 255) ? 255 : bcolor.b + lighten_factor;
    new_button.hover_color.a = bcolor.a;

    // Calculate shadow color (darker)
    new_button.shadow_color = calculateShadowColor(bcolor);
    
    // Handle auto positioning
    if (x == -1) {  // -1 represents 'auto' for x
        new_button.x = layout_context.cursor_x;
    } else {
        new_button.x = x;
    }

    if (y == -1) {  // -1 represents 'auto' for y
        new_button.y = layout_context.cursor_y;
    } else {
        new_button.y = y;
    }
    new_button.width = 200;
    new_button.height = 50;

    new_button.rect1.x = new_button.x;
    new_button.rect1.y = new_button.y;
    new_button.rect1.w = new_button.width;
    new_button.rect1.h = new_button.height;

    // Update the layout context cursor for the next text
    layout_context.cursor_y += font_size + layout_context.padding; // Move down by the height of the font and padding
    
    
    // registers widgets so sw_loop() can keep track of it, see render_widgets() for more details
    	sw_register_widget(WIDGET_BUTTON, &new_button);

    return new_button;
}

// Function to render a button with a shadow and text
void sw_render_button( CREATE *button) {
    // Load the font (inside renderButton)
    TTF_Font *font = TTF_OpenFont(font_path, button->font_size);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return;
    }

    // Shadow position (same x, y but bigger width/height)
    SDL_Rect shadow_rect = button->rect1;
    shadow_rect.w += 4;  // Make shadow wider
    shadow_rect.h += 4;  // Make shadow taller

    // Adjust button and shadow size when pressed
    SDL_Rect button_rect = button->rect1;
    if (button->is_pressed) {
        button_rect.x += 2;
        button_rect.y += 2;
        button_rect.w -= 4;
        button_rect.h -= 4;
        shadow_rect.w -= 4;  // Shrink shadow to match button shrink
        shadow_rect.h -= 4;
    }

    // Render the shadow first (always below the button)
    SDL_SetRenderDrawColor(ren, button->shadow_color.r, button->shadow_color.g, button->shadow_color.b, button->shadow_color.a);
    SDL_RenderFillRect(ren, &shadow_rect);

    // Set color based on hover state (no color change for press)
    if (button->is_hovered) {
        SDL_SetRenderDrawColor(ren, button->hover_color.r, button->hover_color.g, button->hover_color.b, button->hover_color.a);
    } else {
        SDL_SetRenderDrawColor(ren, button->bcolor.r, button->bcolor.g, button->bcolor.b, button->bcolor.a);
    }

    // Draw the button
    SDL_RenderFillRect(ren, &button_rect);

    // Render the text centered in the button
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, button->text, (SDL_Color){button->color.r, button->color.g, button->color.b, button->color.a});
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ren, text_surface);

    int text_width, text_height;
    SDL_QueryTexture(text_texture, NULL, NULL, &text_width, &text_height);

    // Calculate position for centering the text with padding
    int text_x = button_rect.x + (button_rect.w - text_width) / 2;
    int text_y = button_rect.y + (button_rect.h - text_height) / 2;

    SDL_Rect text_rect = {text_x, text_y, text_width, text_height};
    SDL_RenderCopy(ren, text_texture, NULL, &text_rect);

    // Cleanup
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    TTF_CloseFont(font);  // Close the font
}

// Updated button_state function with event and button as arguments
void sw_button_state(SDL_Event *event, CREATE *button) {
    if (event->type == SDL_MOUSEMOTION) {
        int mouse_x = event->motion.x;
        int mouse_y = event->motion.y;
        // Check if mouse is over button
        if (mouse_x >= button->rect1.x && mouse_x <= button->rect1.x + button->rect1.w &&
            mouse_y >= button->rect1.y && mouse_y <= button->rect1.y + button->rect1.h) {
            button->is_hovered = 1;
        } else {
            button->is_hovered = 0;
        }
    } else if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (button->is_hovered) {
            button->is_pressed = 1;
        }
    } else if (event->type == SDL_MOUSEBUTTONUP) {
        button->is_pressed = 0;
        if (button->is_hovered) {
            button->is_clicked = 1;
            printf("Button clicked!\n"); //to  be removed
        }
    }
}

void sw_render_all_button_states(SDL_Event *event) {
    for (int i = 0; i < widget_count; ++i) {
        if (widgets[i].type == WIDGET_BUTTON) {
            sw_button_state(event, (CREATE*)widgets[i].widget);
        }
    }
}

//===============================LABELS============================================
// Function to initialize Button with default values
CREATE sw_label(const char *text, int x, int y, int font_size, Color color, Color bcolor, int style) {
	// label is almost like button as in has text, background, ...
	// but label does not hover on default and does not have shadow and dept
    CREATE new_label;
    new_label.text = text;
    new_label.font_size = font_size;
    new_label.color = color;
    new_label.bcolor = bcolor;
    new_label.style = style;
    new_label.padding = 10;  // Set a default padding value
    new_label.outline_thickness = 2;  // Set default border thickness; FIX ME (outline removed)
    // Handle auto positioning
    if (x == -1) {
        new_label.x = layout_context.cursor_x;
    } else {
        new_label.x = x;
    }

    if (y == -1) {
        new_label.y = layout_context.cursor_y;
    } else {
        new_label.y = y;
    }

    // Update the layout context cursor for the next button
    layout_context.cursor_y += font_size + layout_context.padding;

    // registers widgets so sw_loop() can keep track of it, see render_widgets() for more details
    	sw_register_widget(WIDGET_LABEL, &new_label);
    
    	//return new_widgets so it can be used in render 
        return new_label;
}


// Function to render button
int sw_render_label(const CREATE *label_properties) {
    // Load font
    TTF_Font *font = TTF_OpenFont(font_path, label_properties->font_size);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    // Set font style
    TTF_SetFontStyle(font, label_properties->style);

    SDL_Color sdl_color = {label_properties->color.r, label_properties->color.g, label_properties->color.b, label_properties->color.a};

    // Render text to surface
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, label_properties->text, sdl_color);
    if (!text_surface) {
        printf("Unable to render text surface! TTF_Error: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return -1;
    }

    // Create a Texture from the Surface
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
    if (!text_texture) {
        printf("Unable to create texture from rendered text! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        TTF_CloseFont(font);
        return -1;
    }

    // Calculate button dimensions with padding
    int padding = label_properties->padding;
    int label_width = text_surface->w + 2 * padding;
    int label_height = text_surface->h + 2 * padding;

   
    // Draw the button background rectangle
   sw_draw_rectangle(label_properties->bcolor,label_width, label_height,label_properties->x, label_properties->y, FILLED);

    // Set the text position with padding
    SDL_Rect dst = {
        label_properties->x + padding,
        label_properties->y + padding,
        text_surface->w,
        text_surface->h
    };

    // Copy text texture to renderer
    SDL_RenderCopy(ren, text_texture, NULL, &dst);

    // Clean up
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    TTF_CloseFont(font);

    return 0;
}

//=====================DROP====================

#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40
#define OPTION_HEIGHT 30

CREATE sw_drop_down(int x, int y, const char* options[], int num_options) {
    updateWindowSize();

    CREATE new_drop_down;
    new_drop_down.width = BUTTON_WIDTH;
    new_drop_down.height = BUTTON_HEIGHT;
    new_drop_down.expanded = false;
    new_drop_down.selected_index = -1;

    for (int i = 0; i < num_options && i < MAX_OPTIONS; ++i) {
        new_drop_down.options[i] = options[i];
    }

    // Handle auto positioning
    new_drop_down.x = (x == -1) ? layout_context.cursor_x : x;
    new_drop_down.y = (y == -1) ? layout_context.cursor_y : y;

    layout_context.cursor_y += BUTTON_HEIGHT + layout_context.padding;
    
    // registers widgets so sw_loop() can keep track of it, see render_widgets() for more details
    	sw_register_widget(WIDGET_DROP_DOWN, &new_drop_down);

    return new_drop_down;
}

void sw_draw_filled_triangle_up(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int x3, int y3, SDL_Color color) {
    // Set triangle color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw the filled triangle (upward)
    for (int y = y3; y <= y1; y++) {
        int startX = x3 + (x1 - x3) * (y - y3) / (y1 - y3);
        int endX = x3 + (x2 - x3) * (y - y3) / (y2 - y3);
        SDL_RenderDrawLine(renderer, startX, y, endX, y);  // Draw horizontal line
    }
}

void sw_draw_filled_triangle_down(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int x3, int y3, SDL_Color color) {
    // Set triangle color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw the filled triangle (downward)
    for (int y = y1; y <= y3; y++) {
        int startX = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
        int endX = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
        SDL_RenderDrawLine(renderer, startX, y, endX, y);  // Draw horizontal line
    }
}


void sw_render_drop_down(CREATE *drop_down_properties) {
    SDL_Rect buttonRect = {drop_down_properties->x, drop_down_properties->y, drop_down_properties->width, drop_down_properties->height};

    // Draw the button
    SDL_SetRenderDrawColor(ren, 100, 100, 255, 255);
    SDL_RenderFillRect(ren, &buttonRect);

    // Load font for dropdown text
    TTF_Font *font = TTF_OpenFont(font_path, 16);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    // Selected option or default text
    const char* display_text = (drop_down_properties->selected_index >= 0)
        ? drop_down_properties->options[drop_down_properties->selected_index]
        : "Select an option";

    // Render the dropdown text
    sw_render_text(&(CREATE){
        .text = display_text,
        .x = drop_down_properties->x + 10,
        .y = drop_down_properties->y + 10,
        .font_size = 16,
        .color = WHITE,
        .style = NORMAL
    });

    // Calculate center of the dropdown for triangle placement
    int centerX = drop_down_properties->x + drop_down_properties->width - 20;
    int centerY = drop_down_properties->y + drop_down_properties->height / 2;

    // Triangle color
    SDL_Color triangleColor = {255, 255, 255, 255};  // White color

    if (drop_down_properties->expanded) {
        // Smaller triangle pointing upwards (expanded)
        sw_draw_filled_triangle_up(ren, centerX - 4, centerY + 4, centerX + 4, centerY + 4, centerX, centerY - 4, triangleColor);
    } else {
        // Smaller triangle pointing downwards (collapsed)
        sw_draw_filled_triangle_down(ren, centerX - 4, centerY - 4, centerX + 4, centerY - 4, centerX, centerY + 4, triangleColor);
    }

    // Draw dropdown options if expanded
    if (drop_down_properties->expanded) {
        for (int i = 0; i < MAX_OPTIONS; ++i) {
            SDL_Rect optionRect = {
                drop_down_properties->x,
                drop_down_properties->y + drop_down_properties->height + (i * OPTION_HEIGHT),
                drop_down_properties->width, OPTION_HEIGHT
            };
            SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
            SDL_RenderFillRect(ren, &optionRect);

            sw_render_text(&(CREATE){
                .text = drop_down_properties->options[i],
                .x = optionRect.x + 10,
                .y = optionRect.y + 5,
                .font_size = 16,
                .color = BLACK,
                .style = NORMAL
            });
        }
    }

    TTF_CloseFont(font);
}

void drop_down_handle_event(CREATE* dropdown, SDL_Event* event) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event->button.x;
        int mouseY = event->button.y;

        // Toggle dropdown expansion
        if (mouseX >= dropdown->x && mouseX <= dropdown->x + dropdown->width &&
            mouseY >= dropdown->y && mouseY <= dropdown->y + dropdown->height) {
            dropdown->expanded = !dropdown->expanded;
        }
        // Check if an option was clicked
        else if (dropdown->expanded) {
            for (int i = 0; i < MAX_OPTIONS; ++i) {
                int optionY = dropdown->y + dropdown->height + (i * OPTION_HEIGHT);
                if (mouseX >= dropdown->x && mouseX <= dropdown->x + dropdown->width &&
                    mouseY >= optionY && mouseY <= optionY + OPTION_HEIGHT) {
                    dropdown->selected_index = i;
                    dropdown->expanded = false;
                    break;
                }
            }
        }
    }
}


void sw_render_all_drop_down_states(SDL_Event *event) {
    for (int i = 0; i < widget_count; ++i) {
        if (widgets[i].type == WIDGET_DROP_DOWN) {
            drop_down_handle_event((CREATE*)widgets[i].widget, event);
        }
    }
}
//============SLIDER===============
// Function to create a slider
CREATE sw_slider(int x, int y, int width, int height, float range, Color panel_color, Color thumb_color, Color hover_color, Color text_color, int font_size) {
    CREATE slider;
    slider.x = x;
    slider.y = y;
    slider.width = width;
    slider.height = height;
    slider.range = range;
    slider.value = 0.0f; // Start from 0 (beginning)
    slider.font_size = font_size;
    
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

    slider.color = text_color; // Store the text color
    slider.is_hovered = false;
    slider.is_dragging = false;

	sw_register_widget(WIDGET_SLIDER, &slider);
    return slider;
}

// Function to render the slider
void sw_render_slider(CREATE *slider) {
    // Load the font each time you render
    TTF_Font *font = TTF_OpenFont(font_path, slider->font_size);
    if (font == NULL) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return; // Exit the function if the font fails to load
    }

    // Draw the panel (background)
    SDL_SetRenderDrawColor(ren, slider->panel_color.r, slider->panel_color.g, slider->panel_color.b, slider->panel_color.a);
    SDL_RenderFillRect(ren, &slider->panel_rect);

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
        SDL_SetRenderDrawColor(ren, slider->hover_color.r, slider->hover_color.g, slider->hover_color.b, slider->hover_color.a);
    } else {
        SDL_SetRenderDrawColor(ren, slider->thumb_color.r, slider->thumb_color.g, slider->thumb_color.b, slider->thumb_color.a);
    }
    SDL_RenderFillRect(ren, &slider->thumb_rect);

    // Render text showing the value
    char text[32];
    sprintf(text, "%d", (int)slider->value);

    // Set font to bold
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);

    // Use Blended text for sharper rendering
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, *(SDL_Color *)&slider->color); // Cast Color to SDL_Color for rendering
    if (text_surface == NULL) {
        printf("TTF_RenderText Error: %s\n", TTF_GetError());
        TTF_CloseFont(font);  // Make sure to close the font before returning
        return;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
    if (text_texture == NULL) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    } else {
        SDL_Rect text_rect = {slider->x + slider->width / 2 - text_surface->w / 2, slider->y + slider->height / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
        SDL_RenderCopy(ren, text_texture, NULL, &text_rect);
        SDL_DestroyTexture(text_texture);
    }

    SDL_FreeSurface(text_surface);
    
    // Close the font after rendering
    TTF_CloseFont(font);
}

// Function to handle slider events
void sw_handle_slider_event(SDL_Event *event, CREATE*slider) {
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

void sw_render_all_slider_states(SDL_Event *event) {
    for (int i = 0; i < widget_count; ++i) {
        if (widgets[i].type == WIDGET_SLIDER) {
            sw_handle_slider_event(event, (CREATE*)widgets[i].widget);
        }
    }
}

//=======================================ENTRY====================================================
// Function to create a text box widget
CREATE sw_text_entry(int x, int y, int w, int h, const char *placeholder, int font_size, Color text_color, Color placeholder_color, Color bg_color, Color outline_color, Color outline_color_selected, int style) {
    CREATE text_entry;
    text_entry.rect1.x = x;
    text_entry.rect1.y = y;
    text_entry.rect1.w = w;
    text_entry.rect1.h = h;
    text_entry.entry_text = malloc(256);  // Allocate memory for the text
    text_entry.entry_text[0] = '\0';      // Start with an empty string
    text_entry.font_size = font_size;
    text_entry.color = text_color;
    text_entry.placeholder_color = placeholder_color;
    text_entry.bcolor = bg_color;
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

    sw_register_widget(WIDGET_ENTRY, &text_entry);
    return text_entry;
}

// Function to render the text box widget
void sw_render_text_entry(CREATE *text_entry) {
    // Draw the background (the box)
    SDL_SetRenderDrawColor(ren, text_entry->bcolor.r, text_entry->bcolor.g, text_entry->bcolor.b, text_entry->bcolor.a);
    SDL_RenderFillRect(ren, &text_entry->rect1);

    // Draw the outline (light for inactive, dark for active)
    Color currentOutlineColor = text_entry->active ? text_entry->outline_color_selected : text_entry->outline_color;
    SDL_SetRenderDrawColor(ren, currentOutlineColor.r, currentOutlineColor.g, currentOutlineColor.b, currentOutlineColor.a);
    SDL_RenderDrawRect(ren, &text_entry->rect1);

    const char *displayText;
    Color currentColor;

    // If no text has been entered, display the placeholder text in pale color
    if (strlen(text_entry->text) == 0) {
        displayText = text_entry->place_holder;
        currentColor = text_entry->placeholder_color;
    } else {
        displayText = text_entry->text;
        currentColor = text_entry->color;
    }

	TTF_Font *font = TTF_OpenFont(font_path, text_entry->font_size);
	if (!font) {
	    printf("Failed to load font: %s\n", TTF_GetError());
	}
	
	
    // Render the text (or placeholder)
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, displayText, (SDL_Color){currentColor.r, currentColor.g, currentColor.b, currentColor.a});
    if (!textSurface) {
        printf("Failed to create text surface: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
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
    SDL_RenderSetClipRect(ren, &clipRect);  // Set the clipping region

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
    SDL_RenderCopy(ren, textTexture, NULL, &textRect);

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
        SDL_SetRenderDrawColor(ren, text_entry->color.r, text_entry->color.g, text_entry->color.b, 255);
        SDL_RenderDrawLine(ren, cursor_x, cursor_y, cursor_x, cursor_y + text_height);  // Adjust cursor height to match text height
    }

    SDL_RenderSetClipRect(ren, NULL);  // Disable clipping

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

// Function to handle text box input
void sw_handle_input(CREATE *text_entry, SDL_Event *e) {
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
        strcat(text_entry->entry_text, e->text.text);
        text_entry->cursor_position++;

		TTF_Font *font = TTF_OpenFont(font_path, text_entry->font_size);
		if (!font) {
		    printf("Failed to load font: %s\n", TTF_GetError());
		}


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
            text_entry->entry_text[text_entry->cursor_position - 1] = '\0';
            text_entry->cursor_position--;
        }
    }

    // Handle cursor blinking
    if (SDL_GetTicks() - text_entry->last_cursor_blink_time >= 500) {
        text_entry->show_cursor = !text_entry->show_cursor;
        text_entry->last_cursor_blink_time = SDL_GetTicks();
    }
}

void sw_render_all_entry_states(SDL_Event *event) {
    for (int i = 0; i < widget_count; ++i) {
        if (widgets[i].type == WIDGET_ENTRY) {
            sw_handle_input((CREATE*)widgets[i].widget, event);
        }
    }
}
//=====================RENDER ALL WIDGETS=============================
void sw_render_widgets() {
/*
 Key Steps for render_widgets():
 
    *  Widget Registration: Whenever the user calls button(), label(), or text(), these widgets need to be registered in an internal data structure like a list or array.
    * Widget Rendering: In render_widgets(), the system will loop through all registered widgets and call the appropriate rendering function for each one, like render_button(), render_label(), or render_text().
    * Widget Type Checking: Each widget will have a type (e.g., button, label, text), and based on that type, the corresponding render function will be called.
*/
    for (int i = 0; i < widget_count; ++i) {
        switch (widgets[i].type) {
            case WIDGET_BUTTON:
                sw_render_button((CREATE*)widgets[i].widget);
                break;
            case WIDGET_LABEL:
                sw_render_label((CREATE*)widgets[i].widget);
                break;
            case WIDGET_TEXT:
                sw_render_text((CREATE*)widgets[i].widget);
                break;
          case WIDGET_DROP_DOWN:
                sw_render_drop_down((CREATE*)widgets[i].widget);
                break;
         case WIDGET_RADIO:
        
                break;
         case WIDGET_RADIO_GROUP:
             
                break;
         case WIDGET_SLIDER:
             
                sw_render_slider((CREATE*)widgets[i].widget);
                break; 
         case WIDGET_ENTRY:
             	sw_render_text_entry((CREATE*)widgets[i].widget);
                break;
            // Add cases for other widget types here as you implement them
            default:
                break;
        }
    }
}

//=====================================gui loop=================================================
 void sw_loop() {
     while (sw_active) {
         SDL_Event event;
         while (SDL_PollEvent(&event)) {
             switch (event.type) {
                 case SDL_QUIT:
                     sw_active = false;  // User closed the window
                     break;
                 
                     break;
                 case SDL_WINDOWEVENT:
                     if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                         updateWindowSize();  // Resize event triggers window size update
                     }
                     break;
                 default:
                     break;
             }
             //buttons
             sw_render_all_button_states(&event);
             //drop down
             sw_render_all_drop_down_states(&event);
			//slider
			sw_render_all_slider_states(&event);
            //entry
            sw_render_all_entry_states(&event);
        
         }
          sw_background(GRAY);
         sw_render_widgets();  // Render all widgets (handled by library)
         sw_present();         // Present the rendered output
     }
 }
