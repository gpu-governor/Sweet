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
   sw_init("Gui", 500, 500);
   CREATE my_button1 = sw_button("click me", 200, 250, h4, WHITE, BLUE, BOLD);
   CREATE my_button2 = sw_button("click me", 200, 300, h4, WHITE, GREEN, BOLD);
   CREATE my_label = sw_label("This is a Label", set, set, h1, BLACK, BLUE, BOLD);
   CREATE my_text = sw_text("haha hi, you've been on my mind", 2, 0, h3, WHITE,BOLD);

   // writing this because i may forget, if a widgets is not showing up ensure it is registering
   
    // Main GUI loop (handled by library)
   sw_loop();

    sw_quit();
    return 0;
}
