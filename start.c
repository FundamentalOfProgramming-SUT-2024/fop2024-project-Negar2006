#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include <time.h>

#include "menu.c"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr,TRUE);

    beep();
    flash();

    srand(time(NULL));

    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);



    main_menu();

    endwin();
    return 0;
}
