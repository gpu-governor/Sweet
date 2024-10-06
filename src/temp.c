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

int main() {
   sw_init("Gui", 680, 680);
   
   //buttons
   CREATE my_button = sw_button("click me", 200, 100, h4, WHITE, BLACK, BOLD);  
   CREATE my_button1 = sw_button("click me", 200, 200, h4, WHITE, BLUE, BOLD);
   CREATE my_button2 = sw_button("click me", 200, 300, h4, WHITE, GREEN, BOLD);   
   CREATE my_button3 = sw_button("light", 200, 400, h4, BLACK, YELLOW, BOLD);
   
   // text based
   CREATE my_label = sw_label("This is a Label", 0, 30, h1, BLACK, BLUE, BOLD);
   CREATE my_text = sw_text("haha hi, you've been on my mind", 0, 2, h3, WHITE,BOLD);
   
   
    const char* options[] = {"Option 1", "Option 2", "Option 3", "Option 4", "Option 5"};
    CREATE dropdown = sw_drop_down(450, 100, options, 5);

   // writing this because i may forget, if a widgets is not showing up ensure it is registering
   
    // Main GUI loop (handled by library)
   sw_loop();

    sw_quit();
    return 0;
}
