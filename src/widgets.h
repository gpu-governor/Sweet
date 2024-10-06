#define MAX_OPTIONS 5 // Maximum number of options for dropdown
// GLOBAL VARIABLES
const char* font_path = "assets/FreeMono.ttf";
const int set = -1;
// EFFECT var
const int hover=1, when_clicked=2, outline=3;
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
    const char *text;
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
    // text box specific
    const char * place_holder;
    
        // Dropdown specific
    bool expanded;
    const char* options[MAX_OPTIONS];
    int selected_index;
} CREATE;

typedef struct{
	int type; // can be hover, outline
	Color color;
}EFFECT;




//===================RETAINED MODE SYSTEM==========================
//global
 int mouse_x;
 int mouse_y;
bool sw_mouse_over_widgets(CREATE *item) {
	 mouse_x = event.motion.x;
 	 mouse_y = event.motion.y;
    SDL_Point pt = {mouse_x, mouse_y};
    
    if (SDL_PointInRect(&pt, &item->rect1)) {
        item->is_hovered = true;
        return true;
    } else {
        item->is_hovered = false;
        return false;
    }

}
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
bool sw_button_pressed(CREATE *item){
	 mouse_x = event.motion.x;
 	 mouse_y = event.motion.y;
	 SDL_Point pt = {mouse_x, mouse_y};
    
    if (SDL_PointInRect(&pt, &item->rect1)) {
        item->is_pressed = true;
        return true;
    } else {
        item->is_pressed = false;
        return false;
    }
}

//----------------------------------------------


typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TEXT,
    WIDGET_DROP_DOWN
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
    // Update window size automatically
    updateWindowSize();

    CREATE new_text;
    new_text.text = text;
    new_text.font_size = font_size;
    new_text.color = color;
    new_text.style = style;

    // Handle auto positioning
    if (x == -1) {
        new_text.x = layout_context.cursor_x;
    } else {
        new_text.x = x;
    }

    if (y == -1) {
        new_text.y = layout_context.cursor_y;
    } else {
        new_text.y = y;
    }

    // Apply the effect if needed
    if (effect.type == hover) {
        printf("Hover effect is enabled.\n");
        // Handle hover effect, change color, etc.
    }
    new_text.color = effect.color;  // Apply the effect color

    layout_context.cursor_y += font_size + layout_context.padding;  // Adjust cursor for the next text

    return new_text;
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
             sw_render_all_button_states(&event);
             sw_render_all_drop_down_states(&event);
         }
          sw_background(GRAY);
         sw_render_widgets();  // Render all widgets (handled by library)
         sw_present();         // Present the rendered output
     }
 }
