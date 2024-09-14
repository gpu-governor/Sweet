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
   CREATE my_button = button("click me", 200, 200, h4, YELLOW, BLACK, BOLD);
   CREATE my_button1 = button("click me", 200, 250, h4, WHITE, BLUE, BOLD);
   CREATE my_button2 = button("click me", 200, 300, h4, WHITE, GREEN, BOLD);
   CREATE my_label = label("This is a Label", set, set, h1, BLACK, BLUE, BOLD);
   CREATE my_text = text("haha hi, you've been on my mind", 2, 0, h3, WHITE,BOLD);
   
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
                      
                }
                 background(GRAY);

    
    
    render_label(&my_label);
    render_button(&my_button);
    render_button(&my_button1);
    render_button(&my_button2);
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
