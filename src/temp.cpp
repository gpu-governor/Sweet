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
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }
   
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
                        // Add other event handling cases here if needed
                        default:
                            break;
                    }
                }
                 background(GRAY);

    CREATE my_button = button("click me", 50, 50, h4, WHITE, BLUE, BOLD);
    CREATE my_text = text("haha hi, you've been on my mind", set, set, h4, WHITE,BOLD);
    
    render_button(&my_button);
    render_text(&my_text);
    mouse_over_widgets(&my_button);
	present();
	}
    // Present the rendered output
    
    // Example usage of mouse_over_widgets function
    
   
    
    // Continue rendering the rest of the GUI
      // handle_events();// This should include your event loop logic

    quit();
    return 0;
}
