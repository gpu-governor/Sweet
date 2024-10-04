#include<stdbool.h>

//================CLICKED======================
// Calculate hover color (less bright lighter shade)

// clicked?
bool clicked(CREATE *item) {

    SDL_Point pt = {mouse_x, mouse_y};
    
    if (SDL_PointInRect(&pt, &item->rect1)) {
        item->is_clicked = true;
        return true;
    } else {
        item->is_clicked = false;
        return false;
    }
}

// ===============================MOUSE IS OVER BUTTON==================================



// ===============================BUTTON IS PRESSED==================================
// Press Effect: Change the button's state when clicked or held down, for example, by slightly shrinking the button or adding a subtle color change.
bool button_pressed(CREATE *item){
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
