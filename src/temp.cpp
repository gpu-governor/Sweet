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
	background(WHITE);
    // Initialize Text properties with style
    CREATE props = text("Hello, World!", set, set, 50, BLACK, ITALIC);

    // Render the text with the modified properties
    render_text(&props);
    
    // Add another text with auto alignment
    CREATE text2 = complex_text("Auto align instead of x and y like ImGui", -1, -1, 16, RED, NORMAL,0,(EFFECT){hover,BLUE});
    render_text(&text2);
    
    CREATE my_button = button("click me", 250, 100, 20, WHITE, BLUE, GRAY, BOLD);
    
    render_button(&my_button);

    // Present the rendered output
    handle_events();
    quit();
    return 0;
}
