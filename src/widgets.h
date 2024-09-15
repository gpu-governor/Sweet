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
    const char *text;
    int x;
    int y;
    int font_size;
    Color color;
    bool is_clicked;
    bool is_hovered;
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
} CREATE;

typedef struct{
	int type; // can be hover, outline
	Color color;
}EFFECT;
// ====================TEXT======================
// Function to initialize Text with default values
CREATE text(const char *text, int x, int y, int font_size, Color color, int style) {
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

    return new_text;
}

// Function to render text based on the Text structure
int render_text(const CREATE *text_properties) {
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
CREATE complex_text(const char *text, int x, int y, int font_size, Color color, int style, int alignment, EFFECT effect) {
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
CREATE write(int font_size,const char *text) {
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
// Function to initialize Button with default values
CREATE button(const char *text, int x, int y, int font_size, Color color, Color bcolor, int style) {
    CREATE new_button;
    new_button.text = text;
    new_button.font_size = font_size;
    new_button.color = color;
    new_button.bcolor = bcolor;
    new_button.style = style;
    new_button.padding = 10;  // Set a default padding value
    new_button.outline_thickness = 2;  // Set default border thickness
	new_button.is_hovered = false; // set it to false on default (before programs start)

    // Calculate hover color (lighter shade)
    new_button.hover_color.r = (bcolor.r + 255) / 2;
    new_button.hover_color.g = (bcolor.g + 255) / 2;
    new_button.hover_color.b = (bcolor.b + 255) / 2;
    new_button.hover_color.a = bcolor.a; // Keep alpha the same
    

    // Handle auto positioning
    if (x == -1) {
        new_button.x = layout_context.cursor_x;
    } else {
        new_button.x = x;
    }

    if (y == -1) {
        new_button.y = layout_context.cursor_y;
    } else {
        new_button.y = y;
    }

    // Update the layout context cursor for the next button
    layout_context.cursor_y += font_size + layout_context.padding;

    return new_button;
}


// Function to render button
int render_button(const CREATE *button_properties) {
    // Load font
    TTF_Font *font = TTF_OpenFont(font_path, button_properties->font_size);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    // Set font style
    TTF_SetFontStyle(font, button_properties->style);

    SDL_Color sdl_color = {button_properties->color.r, button_properties->color.g, button_properties->color.b, button_properties->color.a};

    // Render text to surface
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, button_properties->text, sdl_color);
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
    int padding = button_properties->padding;
    int button_width = text_surface->w + 2 * padding;
    int button_height = text_surface->h + 2 * padding;

    // Shadow properties
    int shadow_offset_w = 3;
    int shadow_offset_h = 3;

    // Shadow color
    Color shadow_color = {
        button_properties->bcolor.r / 2,
        button_properties->bcolor.g / 2,
        button_properties->bcolor.b / 2,
        button_properties->bcolor.a
    };

    // Draw the shadow
    draw_rectangle(shadow_color, button_width + shadow_offset_w, button_height + shadow_offset_h,
                   button_properties->x, button_properties->y, FILLED);
    // Draw the button background rectangle
    draw_rectangle(button_properties->is_hovered ?button_properties->hover_color : button_properties->bcolor,
                   button_width, button_height,
                   button_properties->x, button_properties->y, FILLED);

    // Set the text position with padding
    SDL_Rect dst = {
        button_properties->x + padding,
        button_properties->y + padding,
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

//===============================LABELS============================================
// Function to initialize Button with default values
CREATE label(const char *text, int x, int y, int font_size, Color color, Color bcolor, int style) {
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

    return new_label;
}


// Function to render button
int render_label(const CREATE *label_properties) {
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
    draw_rectangle(label_properties->bcolor,label_width, label_height,label_properties->x, label_properties->y, FILLED);

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
//===============================SLIDERS============================================
CREATE slider(int type, float range,int x, int y, int font_size, int width, Color color, Color bcolor) {
	CREATE new_slider;
    new_slider.type = type; 	// where type can be percentage, radian (360), 
    new_slider.font_size = font_size;
    new_slider.color = color;
    new_slider.bcolor = bcolor;
    new_slider.width = width; // so users can make it wide to their taste, maybe height will be same, but i dunno 


    // Handle auto positioning
    if (x == -1) {
        new_slider.x = layout_context.cursor_x;
    } else {
        new_slider.x = x;
    }

    if (y == -1) {
        new_slider.y = layout_context.cursor_y;
    } else {
        new_slider.y = y;
    }

    // Update the layout context cursor for the next button
    layout_context.cursor_y += font_size + layout_context.padding;

    return new_slider;
}

int render_slider(const CREATE *slider_properties) {
    // Load font
    TTF_Font *font = TTF_OpenFont(font_path, slider_properties->font_size);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    // Set font style
    TTF_SetFontStyle(font, slider_properties->style);

    SDL_Color sdl_color = {slider_properties->color.r,slider_properties->color.g, slider_properties->color.b, slider_properties->color.a};

    // Render text to surface
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, slider_properties->text, sdl_color);
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

    // TODO Calculate slider dimensions with padding
    int padding = slider_properties->padding;
    int slider_width = text_surface->w + 2 * padding;
    int slider_height = text_surface->h + 2 * padding;

   
    // Draw the  background rectangle (Gauge)
    draw_rectangle(slider_properties->bcolor,slider_width, slider_height,slider_properties->x, slider_properties->y, FILLED);

    // Set the text position with padding
    SDL_Rect dst = {
        slider_properties->x + padding,
        slider_properties->y + padding,
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
//=================Forms============
// forms and text area
/*CREATE form(const char *place_holder ){
	return;	
}
*/
