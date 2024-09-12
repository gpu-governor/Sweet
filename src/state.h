#include<stdbool.h>

//================CLICKED======================
// clicked?
bool clicked(CREATE *item) {
    while (SDL_PollEvent(&event)) {
        // Check for mouse button down event
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            
            // Check if the mouse click is within the button's boundaries
            if (x >= item->x && x <= item->x + item->width &&
                y >= item->y && y <= item->y + item->height) {
                item->is_clicked = true;
            }
        } 
        // Check for touch events
        else if (event.type == SDL_FINGERDOWN) {
            int x = event.tfinger.x * 500;  // Convert normalized coordinates to screen coordinates
            int y = event.tfinger.y * 500;  // Assuming a 500x500 window size
            
            // Check if the touch is within the button's boundaries
            if (x >= item->x && x <= item->x + item->width &&
                y >= item->y && y <= item->y + item->height) {
                item->is_clicked = true;
            }
        }
    }
    return item->is_clicked; // Return true if the button was clicked
}

// ===============================MOUSE IS OVER BUTTON==================================
bool mouse_over_widgets(CREATE *item) {
    // Get the current mouse position
    int x, y;
    SDL_GetMouseState(&x, &y);
	SDL_Point pt={x,y};

    // Define the widget rectangle (using the widget's dimensions)
    SDL_Rect widget_rect = { item->x, item->y, item->width, item->height };
    // Check if the mouse pointer is within the widget rectangle
    if (SDL_PointInRect(&pt, &widget_rect)) {
        // Logic when the mouse is over the widget
        item->is_hovered = true; // Update widget state (hovered)
        printf("Mouse is over the widget at (%d, %d)\n", x, y);

        // Apply hover effects (e.g., changing color or style)
        // Example: change background color on hover
        if (item->style == BOLD) {
            item->bcolor = BLACK;  // You can change this to any hover color
        }
        return true;
    } else {
        // Mouse is not over the widget
        item->is_hovered = false;
        return false;
    }
}

// ===============================BUTTON IS PRESSED==================================
// Press Effect: Change the button's state when clicked or held down, for example, by slightly shrinking the button or adding a subtle color change.
