#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include <time.h>
#include "map.c"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define FILENAME "users.txt"
#define SCOREFILE "scores.txt"

typedef struct {
    char username[50];
    char password[50];
    char email[100];
    int score;
    int rank;
} User;

int user_exists(const char *username);
int is_valid_password(const char *password);
int is_valid_email(const char *email);
void save_user(const User *user);
void generate_random_password(char *password, int length);
void setting_menu();
void hardness();
void show_song_playlist();
void play_song(const char *filename);
void stop_music();

void setting_menu(){
     int choice;

    clear();
    mvprintw(4,10,"===============================");
    mvprintw(5,10,"Setting board");
    mvprintw(6,10,"===============================");
    mvprintw(9,10,"1. Game difficulty level");
    mvprintw(10,10,"2. Color of Player");
    mvprintw(11,10,"3. Back to main menu");

    echo();
    scanw("%d", &choice);
    noecho();
    switch (choice)
    {
    case 1:
    hardness();
        break;
    case 2:
        break;
    case 3:
        return;
    default:
        mvprintw(17, 10, "Invalid option! Try again.");
        getch();
        break;
    }



}
void hardness(){
    int choice;

    clear();
    mvprintw(4,10,"===============================");
    mvprintw(5,10,"Game difficulty level");
    mvprintw(6,10,"===============================");
    mvprintw(8,10,"1. HARD");
    mvprintw(9,10,"2. MEDIUM");
    mvprintw(10,10,"3. EASY");

    echo();
    scanw("%d", &choice);
    noecho();
    switch (choice)
    {
    case 1:
        break;
    case 2:
        return ;
    default:
        mvprintw(17, 10, "Invalid option! Try again.");
        getch();
        break;
    }

}

void show_users() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        mvprintw(5, 10, "User file not found!");
        getch();
        return;
    }

    clear();
    attron(COLOR_PAIR(2));
    mvprintw(1, 10, "==================================");
    mvprintw(2, 10, " Registered Users List ");
    mvprintw(3, 10, "==================================");
    attroff(COLOR_PAIR(2));

    User user;
    int y = 5;

    mvprintw(y++, 10, "Username           Password           Email           score           rank");
    mvprintw(y++, 10, "--------------------------------------------------------------------------");

    while (fread(&user, sizeof(User), 1, file)) {
        mvprintw(y++, 10, "%-16s %-16s %-16s %-16d %-16d", user.username, user.password, user.email, user.score, user.rank);
    }

    fclose(file);

    mvprintw(y + 1, 10, "Press any key to return to the main menu...");
    getch();
}
void Profile(const char * username){
   FILE *file = fopen(FILENAME,"r");
   User user;
   int found = 0;
   while(fread(&user,sizeof(User),1,file)){
    if(strcmp(user.username,username) == 0){
        found = 1;
        break;
    }
   }
   fclose(file);
   clear();
   mvprintw(5,10,"=====================================================");
   mvprintw(6,10,"            User Profile");
   mvprintw(7,10,"======================================================");
   mvprintw(10,10,"Username: %s",user.username);
   mvprintw(11,10,"Password: %s",user.password);
   mvprintw(12,10,"Email: %s",user.email);
   mvprintw(13,10,"Score: %d", user.score);
   mvprintw(14,10,"Rank: %d",user.rank);

   mvprintw(17,10,"Press 'q' to return to pre_game_menu: ");
   char ch;
   do{
    ch = getch();
   }while(ch != 'q');
}
void pre_game_menu(const char *username) {
    int choice;

    while (1) {
        clear();
        attron(COLOR_PAIR(2));
        mvprintw(5, 10, "==================================");
        mvprintw(6, 10, " Pre-Game Menu ");
        mvprintw(7, 10, "==================================");
        attroff(COLOR_PAIR(2));

        mvprintw(9, 10, "Welcome, %s!", username);
        mvprintw(11, 10,"1. Start New Game");
        mvprintw(12, 10,"2. Continue Previous Game");
        mvprintw(13, 10,"3. Profile: (Do not selected this button if you are logged in as a guest)");
        mvprintw(14, 10,"4. Setting");
        mvprintw(15, 10,"5. Score Board: ");
        mvprintw(16, 10,"6. Back to Main Menu");
        mvprintw(19, 10,"Select an option: ");

        echo();
        scanw("%d", &choice);
        noecho();

        switch (choice) {
            case 1:
                clear();
                gameLoop();
                refresh();
                break;
            case 2:
                clear();
                mvprintw(0, 0, "Continuing the previous game...");
                //gameLoop();
                getch();
                break;
            case 3:
                Profile(username);
                break;
            case 4:
                setting_menu();
                break;
            case 5:
                show_users();
                break;
            case 6:
                return ;
            default:
                mvprintw(17, 10, "Invalid option! Try again.");
                getch();
                break;
        }
    }
}
void guest_login() {
    clear();
    attron(COLOR_PAIR(2));
    mvprintw(5, 10, "==================================");
    mvprintw(6, 10, "         Welcome, Guest!          ");
    mvprintw(7, 10, "==================================");
    attroff(COLOR_PAIR(2));
    pre_game_menu("Guest");
    getch();
}
int login_user() {
    char username[50], password[50];
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        mvprintw(5, 10, "User file missing!");
        getch();
        return 0;
    }

    while (1) {
        clear();
        attron(COLOR_PAIR(2));
        mvprintw(1, 10, "==================================");
        mvprintw(2, 10, " Login User ");
        mvprintw(3, 10, "==================================");
        attroff(COLOR_PAIR(2));

        mvprintw(5, 10, "Enter username: ");
        echo();
        getstr(username);
        noecho();

        mvprintw(7, 10, "Enter password: ");
        echo();
        getstr(password);
        noecho();

        User user;
        while (fread(&user, sizeof(User), 1, file)) {
            if (strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0) {
                fclose(file);
                pre_game_menu(username);
                return 1;
            }
        }

        attron(COLOR_PAIR(3));
        mvprintw(9, 10, "Invalid username or password. Try again.");
        attroff(COLOR_PAIR(3));
        getch();
        return 0;
    }
}
void reset_password() {
    char username[50], new_password[50];
    int found = 0;

    FILE *file = fopen(FILENAME, "r+");
    if (!file) {
        mvprintw(5, 10, "User file missing!");
        getch();
        return;
    }

    clear();
    attron(COLOR_PAIR(2));
    mvprintw(1, 10, "==================================");
    mvprintw(2, 10, " Reset User Password ");
    mvprintw(3, 10, "==================================");
    attroff(COLOR_PAIR(2));

    mvprintw(5, 10, "Enter username: ");
    echo();
    getstr(username);
    noecho();

    User user;
    long pos;

    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(user.username, username) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        do {
            mvprintw(7, 10, "Enter new password (at least 7 characters): ");
            echo();
            getstr(new_password);
            noecho();

            if (strlen(new_password) < 7) {
                attron(COLOR_PAIR(3));
                mvprintw(9, 10, "Password too short! Try again.");
                attroff(COLOR_PAIR(3));
                getch();
                clear();
            }
        } while (strlen(new_password) < 7);

        strcpy(user.password, new_password);

        pos = ftell(file) - sizeof(User);
        fseek(file, pos, SEEK_SET);
        fwrite(&user, sizeof(User), 1, file);

        attron(COLOR_PAIR(4));
        mvprintw(11, 10, "Password reset successfully!");
        attroff(COLOR_PAIR(4));
    } else {
        attron(COLOR_PAIR(3));
        mvprintw(7, 10, "Username not found!");
        attroff(COLOR_PAIR(3));
    }

    fclose(file);
    getch();
}

void save_user(const User *user) {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        perror("Error opening file");
        return;
    }
    fwrite(user, sizeof(User), 1, file);
    fclose(file);
}

void create_user_menu() {
    char username[50], password[50], email[100];
    int valid = 0;

    clear();
    attron(COLOR_PAIR(2));
    mvprintw(1, 10, "==================================");
    mvprintw(2, 10, " Create New User ");
    mvprintw(3, 10, "==================================");
    attroff(COLOR_PAIR(2));

    while (!valid) {
        mvprintw(5, 10, "Enter username: ");
        echo();
        getstr(username);
        noecho();

        if (user_exists(username)) {
            attron(COLOR_PAIR(3));
            mvprintw(7, 10, "Error: Username already exists!");
            attroff(COLOR_PAIR(3));
            getch();
            continue;
        }

        char choice;
        mvprintw(8, 10, "Do you want to generate a random password? (y/n): ");
        choice = getch();

        if (choice == 'y' || choice == 'Y') {
            generate_random_password(password, 12);
            mvprintw(10, 10, "Generated Password: %s", password);
        } else {
            mvprintw(10, 10, "Enter password: ");
            echo();
            getstr(password);
            noecho();

            if (!is_valid_password(password)) {
                attron(COLOR_PAIR(3));
                mvprintw(12, 10, "Error: Password must be at least 7 characters long.");
                attroff(COLOR_PAIR(3));
                getch();
                return ;
                continue;
            }
        }

        mvprintw(14, 10, "Enter email: ");
        echo();
        getstr(email);
        noecho();

        if (!is_valid_email(email)) {
            attron(COLOR_PAIR(3));
            mvprintw(16, 10, "Error: Invalid email format!");
            attroff(COLOR_PAIR(3));
            getch();
            return ;
            continue;
        }

        User new_user;
        strcpy(new_user.username, username);
        strcpy(new_user.password, password);
        strcpy(new_user.email, email);

        save_user(&new_user);
        attron(COLOR_PAIR(4));
        mvprintw(18, 10, "User created successfully!");
        attroff(COLOR_PAIR(4));
        getch();
        valid = 1;
    }
}
void welcome_screen() {
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(5, 10, "==================================");
    mvprintw(6, 10, " Welcome to the Game ");
    mvprintw(7, 10, "==================================");
    attroff(COLOR_PAIR(1));

    mvprintw(9, 10, "1. Create New User");
    mvprintw(10, 10, "2. Login User");
    mvprintw(11, 10, "3. Login as Guest");
    mvprintw(12, 10, "4. Show Registered Users");
    mvprintw(13, 10, "5. Choose your song:)");
    mvprintw(14, 10, "6. Exit");
    mvprintw(17, 10, "Select an option: ");
}
void main_menu() {
    int choice;

    while (1) {
        welcome_screen();
        echo();
        scanw("%d", &choice);
        noecho();

        switch (choice) {
            case 1:
                create_user_menu();
                break;
            case 2:
                if (login_user()) {
                }
                break;
            case 3:
                guest_login();
                break;
            case 4:
                show_users();
                break;
            case 5:
                show_song_playlist();
                break;
            case 6:
                stop_music();
                endwin();
                exit(0);
                break;
            default:
                attron(COLOR_PAIR(3));
                mvprintw(15, 10, "Invalid option. Press any key to try again...");
                attroff(COLOR_PAIR(3));
                getch();
        }
    }
}
int user_exists(const char *username) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return 0;

    User user;
    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(user.username, username) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int is_valid_password(const char *password) {
    if (strlen(password) < 7) return 0;

    int has_digit = 0, has_upper = 0, has_lower = 0;
    for (int i = 0; password[i]; i++) {
        if (isdigit(password[i])) has_digit = 1;
        else if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
    }

    return has_digit && has_upper && has_lower;
}

int is_valid_email(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    return at && dot && at < dot;
}

void generate_random_password(char *password, int length) {
    const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";
    int chars_len = strlen(chars);

    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % chars_len];
    }
    password[length] = '\0';
}
#include <SDL2/SDL_mixer.h>

void show_song_playlist(){
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(5, 10, "==================================");
    mvprintw(6, 10, "         Song Playlist          ");
    mvprintw(7, 10, "==================================");
    attroff(COLOR_PAIR(1));

    mvprintw(9, 10, "1. music1.mp3");
    mvprintw(10, 10, "2. music2.mp3");
    mvprintw(11, 10, "3. Back to Menu");
    mvprintw(12, 10, "Select a song: ");

    int choice;
    echo();
    scanw("%d", &choice);
    noecho();

    switch (choice) {
        case 1:
            play_song("music1.mp3");
            //return;
            break;
        case 2:
            play_song("music2.mp3");
            break;
        case 3:
            break;
        default:
            attron(COLOR_PAIR(3));
            mvprintw(13, 10, "Invalid option. Press any key to try again...");
            attroff(COLOR_PAIR(3));
            getch();
    }
}
void play_song(const char *filename) {
    // Initialize SDL_mixer if not already done
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) {
        printf("Failed to load music: %s\n", Mix_GetError());
        Mix_CloseAudio();
        return;
    }

    Mix_PlayMusic(music, -1); // Play in loop
}
void stop_music(){
    Mix_HaltMusic();
    Mix_CloseAudio();
}

