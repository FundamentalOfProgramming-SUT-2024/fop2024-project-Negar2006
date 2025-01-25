#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include <time.h>

#include "menu.c"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
extern char **global_tile;

Mix_Music *music = NULL;
void play_music(const char *file){
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2 , 2048) < 0){
        printf("SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    music = Mix_LoadMUS(file);
    if(!music){
        printf("Failed to load music: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayMusic(music, -1);
}
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

    allocate_global(40,150);

    main_menu();
    if(music) {
        Mix_FreeMusic(music);
        Mix_CloseAudio();
    }

    free(global_tile);
    for(int i = 0 ; i < 40; i++){
        free(global_tile[i]);
    }
    endwin();
    return 0;
}