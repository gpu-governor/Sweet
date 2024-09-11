#include "backend.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include"widgets.h"
#include"state.h"
void do_func(){} // to be removed later after click is functional

//==================TEXT===================
//example 1
/*
int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize!\n");
        return -1;
    }

    // Initialize Text properties with style
    CREATE props = text("Hello, World!", -1, -1, 16, RED, NORMAL);

    // Modify properties if needed
    props.color = GREEN; // Change the color to GREEN
    props.style = ITALIC;  // Change the style to ITALIC

    // Render the text with the modified properties
    render_text(&props);
    
    // Add another text with auto alignment
    CREATE text2 = text("Auto align instead of x and y like ImGui", -1, -1, 16, RED, NORMAL);
    render_text(&text2);

    // Present the rendered output
    present();
    SDL_Delay(3000); // Delay to view the result
    
    quit();
    return 0;
}
*/


//======================================

//example 2
/*
int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize!\n");
        return -1;
    }

    // Initialize Text properties with style
    CREATE props = text("Hello, World!", 100, 150, 16, RED, NORMAL);

    // Modify properties if needed
    props.color = GREEN; // Change the color to GREEN
    props.style = ITALIC;  // Change the style to BOLD

    // Render the text with the modified properties
    render_text(&props);
    CREATE text2=text("auto align instead of x and y like imgui",set,set,16,RED,NORMAL);
    render_text(&text2);
    present();
    SDL_Delay(3000); // Delay to view the result
    
    quit();
    return 0;
}
*/
//==========================================
// example 3
/*
int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }
    background(WHITE);
    // using html tags for font size

    CREATE header1=text("this is H1 tag",set,set,h1,BLACK,NORMAL);
     CREATE header2=text("this is H2 tag",set,set,h2,BLUE,NORMAL);
          CREATE p_tag=text("this is a p tag",set,set,p,BLUE,NORMAL);

	render_text(&header1), render_text(&header2),render_text(&p_tag);
    // Present the rendered output
    handle_events();
    quit();
    return 0;
}
*/
//=========================================
// example 4: complex text and write
int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }
    background(WHITE);
	// complex text provide more control with more parameters
    CREATE header1=complex_text("this is H1 tag",set,set,h1,BLACK,NORMAL,1,(EFFECT){hover,BLUE});
	render_text(&header1);
	// write provides just font and text, but you can use .method to add other properties like color later
	CREATE simpe_text=write(p,"blah, blah, ound(WHITE);complex text provide more control with more parameters, while write provides just font and text, but you can use .method to add other properties like color later");
	render_text(&simpe_text);
    // Present the rendered output
    handle_events();
    quit();
    return 0;
}

//====================BUTTONS==================

// example 1
/*
int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }
    background(GRAY);

    CREATE my_button = button("click me", 30, 30, 50, RED, YELLOW, WHITE, BOLD);
    
    render_button(&my_button);

    // Present the rendered output
    handle_events();
    
    quit();
    return 0;
}
*/

// example 2
// early prototype
/*
int main() {
    if (!init("Gui", 500, 500)) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }
    background(GRAY);

    CREATE my_button = button("click me", set, set, 50, RED, YELLOW, WHITE, BOLD);
    
    
    CREATE button2 = button("click me", set, set, 50, WHITE, BLACK, WHITE, BOLD);
    
    // renderButton(&my_button);
    render_button(&button2);
    // event handling using ternary
    clicked(&my_button)? do_func() : proceed ;
    
    // event using if
    if(clicked(&button2)){
    	button2.bcolor=GREEN;
    	render_button(&button2);
    }
    

    // Present the rendered output
    handle_events();
    
    quit();
    return 0;
}
*/