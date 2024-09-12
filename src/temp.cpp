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
    background(GRAY);

    CREATE my_button = button("click me", 30, 30, h4, RED, YELLOW, BOLD);
    
    render_button(&my_button);

    // Present the rendered output
    handle_events();
    
    quit();
    return 0;
}
