#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
// local includes
#include "backend.h"
#include"widgets.h"
#include"state.h"
int main() {
   init("Gui", 500, 500);
   CREATE my_button = button("click me", 50, 50, h4, WHITE, GREEN, BOLD);
   CREATE my_text = text("haha hi, you've been on my mind", set, set, h4, WHITE,BOLD);
   
    while (active) {
                        // Poll for events
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_QUIT:
                            active = false;  // User closed the window
                            break;
                        case SDL_WINDOWEVENT:
                            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                                updateWindowSize();  // Resize event triggers window size update
                            }
                            break;
                        case SDL_MOUSEMOTION:
                        	 mouse_over_widgets(&my_button);
                        	break;
                        // Add other event handling cases here if needed
                        default:
                            break;
                    }
                       if (event.type == SDL_MOUSEMOTION){
                       	int x = event.motion.x;      // X position of the mouse
                       	        int y = event.motion.y;      // Y position of the mouse
                       	        int xrel = event.motion.xrel; // Relative motion in X direction
                       	        int yrel = event.motion.yrel; // Relative motion in Y direction
                       	        printf("Mouse moved to %d, %d\n", x, y);
                       }
                }
                 background(GRAY);

    
    
    render_button(&my_button);
    render_text(&my_text);
	present();
	}
    // Present the rendered output
    
    // Example usage of mouse_over_widgets function
    
   
    
    // Continue rendering the rest of the GUI
      // handle_events();// This should include your event loop logic

    quit();
    return 0;
}
