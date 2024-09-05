//===========[[[[[CLICKED]]]]]======================
// clicked?
bool clicked(CREATE *item) {
    SDL_Event event;
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