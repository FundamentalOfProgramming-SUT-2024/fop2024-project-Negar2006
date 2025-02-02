#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define FILENAME "users.txt"


typedef struct Position{
	int x;
	int y;
} Position;

typedef struct {
	Position start;
	Position end;
	int is_visible;
}Stairs;

typedef struct Level
{
    char **tile;
	int **visited;
	int level;
	int numberOfRooms;
	struct Room ** rooms;
	struct Monster ** monsters;
	int numberOfMonsters;
	struct Player * user;
	char *comment;
    Position traps;
    int trap_revealed;
    Position secret;
    int secret_revealed;
} Level;

typedef struct Monster{
	char string[2];
	char symbol;
	int health;
	int attack;
	int speed;
	int defence;
	int pathfinding;
	int alive;
	Position * position;
} Monster;

typedef struct Door
{
	Position position;
	int connected;
} Door;

typedef struct Room
{
	Position position;
	int height;
	int width;
	struct Door ** doors;
	int numberOfDoors;
    int visible;
	Position **cols;
	Position ** window;
    Position * stairs;
    Position * stairsss;
} Room;

typedef struct {
    char name[20];
    char symbol;
    int hunger_reduction;
} Food;

typedef struct Weapon{
    char *name;
    int damage;
    int maxDistance;
    int count;
    int isThrowable; 
} Weapon;

typedef struct {
    Weapon *weapons[5];
    int equippedWeaponIndex;
} PlayerInventory;

typedef struct Player
{
    int level;
	Position * position;
	int health;
    int hunger;
	int attack;
	int gold;
	int maxHealth;
	int exp;
    int normal_food;
    int excellent_food;
    int spoiled_food;
    int magical_food;
    PlayerInventory * inventory;
    int Mase;
    int Sward;
    int Normal_arrow;
    int Magic_wand;
    int Dagger;
    char *main_weapon;
    int HEALTH;
    int SPEED;
    int DAMAGE;
    int speed;
    int speed_timer;

    int healthBoostApplied; 
    time_t healthBoostEnd;  
} Player;

typedef struct {
    char username[50];
    char password[50];
    char email[100];
    int score;
    int rank;
} User;


int addMonsters(Level* level);
Monster * selectMonster(int level);
Monster * createMonster(char symbol, int health, int attack, int speed, int defence, int pathfinding);
void setStartingPosition(Monster * monster, Room * room,Level *level);
void moveMonsters(Level * level);
int pathfindingRandom(Position * position);
int pathfindingSeek(Position * start, Position * destination);
void drawMonster(Monster * monster, Level *level);
int pathfindingSeekkkk(Position *start, Position *destination);
void save_user_info(const char *username, int score);
void save_game_state(const char *username, Level *level);
int load_game_state(const char *username, Level *level);
int load_user_info(const char *username, User *user);
void save_level_to_txt(const char *username, Level *level);

void setStartingPosition(Monster * monster, Room * room, Level *level){

    monster->position->x = (rand() % (room->width - 2)) + room->position.x + 1;
    monster->position->y = (rand() % (room->height - 2)) + room->position.y + 1;
	
}
int addMonsters(Level* level){
    int x;
    level->monsters = malloc(sizeof(Monster *)* 6);
    level->numberOfMonsters = 0;

    for (x = 0; x < 6; x++){
        level->monsters[level->numberOfMonsters] = selectMonster(level->level);
        setStartingPosition(level->monsters[level->numberOfMonsters], level->rooms[x], level);
        level->numberOfMonsters++;
    }
}
Monster * selectMonster(int level){
    int monster;
    switch (level)
    {
        case 1:
        case 2:
        case 3:
        case 4:
            monster = (rand() % 5) + 1;
            break;
     }
    switch (monster){
        case 1: 
            return createMonster('D', 5, 1, 1, 1, 0);
        case 2:
            return createMonster('F', 10, 3, 1, 1, 0);
        case 3: 
            return createMonster('G', 15, 5, 1, 1, 2);
        case 4:
            return createMonster('S', 20, 1, 1, 1, 3);
        case 5:
            return createMonster('U', 30, 1, 1, 1, 2);
    }

}
Monster * createMonster(char symbol, int health, int attack, int speed, int defence, int pathfinding){
    Monster * newMonster;
    newMonster = malloc(sizeof(Monster));

    newMonster->symbol = symbol;
    newMonster->health = health;
    newMonster->attack = attack;
    newMonster->speed = speed;
    newMonster->defence = defence;
    newMonster->pathfinding = pathfinding;
    newMonster->alive = 1;

    sprintf(newMonster->string, "%c", symbol);

    newMonster->position = malloc(sizeof(Position));
    

    return newMonster;
}
void moveMonsters(Level * level) {
    int x;
    for (x = 0; x < level->numberOfMonsters; x++) {
        Monster *monster = level->monsters[x];
        if (monster->alive) {
            mvaddch(monster->position->y, monster->position->x, '.');
            if (monster->pathfinding == 0) {
                continue;
            } else if (monster->pathfinding == 1) {
                pathfindingRandom(monster->position);
            } else if(monster->pathfinding == 2){
                pathfindingSeek(monster->position, level->user->position);
            }else{
                pathfindingSeekkkk(monster->position, level->user->position);
            } 
        }
    }
}
int pathfindingRandom(Position * position){
    int random = rand() % 8;
    switch (random){
        case 0:
            if(mvinch(position->y - 1, position->x) == '.'){
                position->y = position->y - 1;
            }
            break;
        case 1:
            if (mvinch(position->y + 1, position->x) == '.'){
                position->y = position->y + 1;
            }
            break;
        case 2:
            if (mvinch(position->y, position->x - 1) == '.'){
                position->x = position->x - 1;
            }
            break;
        case 3:
            if (mvinch(position->y, position->x + 1) == '.'){
                position->x = position->x + 1;
            }
            break;
        case 4:
            if (mvinch(position->y+ 1, position->x + 1) == '.'){
                position->y = position->y + 1;
                position->x = position->x + 1;
            }
            break;
        case 5:
            if (mvinch(position->y- 1, position->x + 1) == '.'){
                position->y = position->y - 1;
                position->x = position->x + 1;
            }
            break;
        case 6:
            if (mvinch(position->y+ 1, position->x - 1) == '.'){
                position->y = position->y + 1;
                position->x = position->x - 1;
            }
            break;
        case 7:
            if (mvinch(position->y- 1, position->x - 1) == '.'){
                position->y = position->y - 1;
                position->x = position->x - 1;
            }
            break;

    }
    return 1;
}
int pathfindingSeek(Position * start, Position * destination){

    if ((abs((start->x - 1) - destination->x) < abs(start->x - destination->x)) && (mvinch(start->y, start->x - 1) == '.')){
        start->x = start->x - 1;

    } else if ((abs((start->x + 1) - destination->x) < abs(start->x - destination->x)) && (mvinch(start->y, start->x + 1) == '.')){
        start->x = start->x + 1;
    
    } else if ((abs((start->y + 1) - destination->y) < abs(start->y - destination->y)) && (mvinch(start->y + 1, start->x) == '.')){
        start->y = start->y + 1;

    } else if ((abs((start->y - 1) - destination->y) < abs(start->y - destination->y)) && (mvinch(start->y - 1, start->x) == '.')){
        start->y = start->y - 1;
    } else{
    }

    return 1;
}
void drawMonster(Monster * monster, Level *level) {
    if (monster->health > 0) {
        if (level->visited[monster->position->y][monster->position->x] == 1) {
           mvaddch(monster->position->y, monster->position->x, monster->symbol);
        }
    }else{
        level->user->gold += 40;
        level->comment = "CONGRATULATION you kill the monster and got 40 gold !!!!!";
        monster->alive = 0;
        level->tile[monster->position->y][monster->position->x] = '.';
        level->numberOfMonsters--;
    }
}
int pathfindingSeekkkk(Position *start, Position *destination){
    
    if ((abs((start->x - 1) - destination->x) < abs(start->x - destination->x)) && ((mvinch(start->y, start->x - 1) == '.') || (mvinch(start->y, start->x - 1) == '#')|| (mvinch(start->y, start->x - 1) == '+'))){
        start->x = start->x - 1;

    } else if ((abs((start->x + 1) - destination->x) < abs(start->x - destination->x)) && ((mvinch(start->y, start->x + 1) == '.') || (mvinch(start->y, start->x - 1) == '#')|| (mvinch(start->y, start->x - 1) == '+'))){
        start->x = start->x + 1;
    
    } else if ((abs((start->y + 1) - destination->y) < abs(start->y - destination->y)) && ((mvinch(start->y + 1, start->x) == '.') || (mvinch(start->y, start->x - 1) == '#')|| (mvinch(start->y, start->x - 1) == '+'))){
        start->y = start->y + 1;

    } else if ((abs((start->y - 1) - destination->y) < abs(start->y - destination->y)) && ((mvinch(start->y - 1, start->x) == '.') || (mvinch(start->y, start->x - 1) == '#') || (mvinch(start->y, start->x - 1) == '+'))){
        start->y = start->y - 1;
    } else{
    }
    return 1;

}
void save_user_info(const char *username, int score){
    FILE *file = fopen(FILENAME, "r+");
    
    User user;
    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(user.username, username) == 0) {
            user.score += score;
            fseek(file, -sizeof(User), SEEK_CUR);
            fwrite(&user, sizeof(User), 1, file);
            break;
        }
    }
    fclose(file);
}
void save_game_state(const char *username, Level *level){
    char game_filename[100];
    snprintf(game_filename, sizeof(game_filename), "%s_game.dat", username);
    FILE *game_file = fopen(game_filename, "wb");
    if (!game_file) {
        perror("Error saving game state");
        return;
    }

    fwrite(level, sizeof(Level), 1, game_file);

    for (int i = 0; i < level->numberOfRooms; i++) {
        fwrite(level->rooms[i], sizeof(Room), 1, game_file);
    }

    for (int i = 0; i < level->numberOfMonsters; i++) {
        fwrite(level->monsters[i], sizeof(Monster), 1, game_file);
    }

    fwrite(level->user, sizeof(Player), 1, game_file);

    fclose(game_file);
}
int load_game_state(const char *username, Level *level){
    char game_filename[100];
    snprintf(game_filename, sizeof(game_filename), "%s_game.dat", username);
    FILE *game_file = fopen(game_filename, "rb");
    if (!game_file) {
        return 0; 
    }
    fread(level, sizeof(Level), 1, game_file);

    for (int i = 0; i < level->numberOfRooms; i++) {
        level->rooms[i] = malloc(sizeof(Room)); 
        fread(level->rooms[i], sizeof(Room), 1, game_file);
    }

    for (int i = 0; i < level->numberOfMonsters; i++) {
        level->monsters[i] = malloc(sizeof(Monster)); 
        fread(level->monsters[i], sizeof(Monster), 1, game_file);
    }

    level->user = malloc(sizeof(Player)); 
    fread(level->user, sizeof(Player), 1, game_file);

    fclose(game_file);
    return 1;
}
int load_user_info(const char *username, User *user){
    FILE *file = fopen(FILENAME, "r");
    while (fread(user, sizeof(User), 1, file)) {
        if (strcmp(user->username, username) == 0) {
            fclose(file);
            return 1; 
        }
    }
    fclose(file);
    return 0;
}
void save_level_to_txt(const char *username, Level *level) {
    char level_filename[100];
    snprintf(level_filename, sizeof(level_filename), "%s_level.txt", username);
    FILE *level_file = fopen(level_filename, "w");

    fprintf(level_file, "Level: %d\n", level->level);
    fprintf(level_file, "Number of Rooms: %d\n", level->numberOfRooms);
    fprintf(level_file, "Number of Monsters: %d\n", level->numberOfMonsters);

    fprintf(level_file, "Player Position: (%d, %d)\n", level->user->position->x, level->user->position->y);
    fprintf(level_file, "Player Health: %d\n", level->user->health);
    fprintf(level_file, "Player Hunger: %d\n", level->user->hunger);
    fprintf(level_file, "Player Attack: %d\n", level->user->attack);
    fprintf(level_file, "Player Gold: %d\n", level->user->gold);
    for (int i = 0; i < level->numberOfRooms; i++) {
        Room *room = level->rooms[i];
        fprintf(level_file, "Room %d Position: (%d, %d)\n", i, room->position.x, room->position.y);
        fprintf(level_file, "Room %d Dimensions: %d x %d\n", i, room->height, room->width);
    }

    for (int i = 0; i < level->numberOfMonsters; i++) {
        Monster *monster = level->monsters[i];
        fprintf(level_file, "Monster %d: Symbol: %c, Position: (%d, %d), Health: %d, Attack: %d\n",
                i, monster->symbol, monster->position->x, monster->position->y, monster->health, monster->attack);
    }


    fprintf(level_file, "Tile Grid:\n");
    for (int i = 0; i < 40; i++) { 
        for (int j = 0; j < 150; j++) {
            fprintf(level_file, "%c", level->tile[i][j]); 
        }
        fprintf(level_file, "\n"); 
    }
    fclose(level_file);
}
int load_level_from_txt(const char *username, Level *level) {
    char level_filename[100];
    snprintf(level_filename, sizeof(level_filename), "%s_level.txt", username);
    FILE *level_file = fopen(level_filename, "r");
    if (!level_file) {
        return 0; 
    }

    char line[256];
    while (fgets(line, sizeof(line), level_file)) {
        if (strncmp(line, "Level: ", 7) == 0) {
            sscanf(line, "Level: %d", &level->level);
        } else if (strncmp(line, "Number of Rooms: ", 17) == 0) {
            sscanf(line, "Number of Rooms: %d", &level->numberOfRooms);
            level->rooms = malloc(level->numberOfRooms * sizeof(Room*)); 
        } else if (strncmp(line, "Number of Monsters: ", 20) == 0) {
            sscanf(line, "Number of Monsters: %d", &level->numberOfMonsters);
            level->monsters = malloc(level->numberOfMonsters * sizeof(Monster*)); 
        } else if (strncmp(line, "Player Position: ", 17) == 0) {
            sscanf(line, "Player Position: (%d, %d)", &level->user->position->x, &level->user->position->y);
        } else if (strncmp(line, "Player Health: ", 15) == 0) {
            sscanf(line, "Player Health: %d", &level->user->health);
        } else if (strncmp(line, "Player Hunger: ", 16) == 0) {
            sscanf(line, "Player Hunger: %d", &level->user->hunger);
        } else if (strncmp(line, "Player Attack: ", 15) == 0) {
            sscanf(line, "Player Attack: %d", &level->user->attack);
        } else if (strncmp(line, "Player Gold: ", 13) == 0) {
            sscanf(line, "Player Gold: %d", &level->user->gold);
        } else if (strncmp(line, "Room ", 5) == 0) {
            int roomIndex = 0;
            sscanf(line, "Room %d Position: (%d, %d)", &roomIndex, &level->rooms[roomIndex]->position.x, &level->rooms[roomIndex]->position.y);
        } else if (strncmp(line, "Room Dimensions: ", 17) == 0) {
            int roomIndex = 0;
            sscanf(line, "Room %d Dimensions: %d x %d", &roomIndex, &level->rooms[roomIndex]->height, &level->rooms[roomIndex]->width);
        } else if (strncmp(line, "Monster ", 8) == 0) {
            int monsterIndex = 0;
            char symbol;
            sscanf(line, "Monster %d: Symbol: %c, Position: (%d, %d), Health: %d, Attack: %d",
                   &monsterIndex, &symbol, &level->monsters[monsterIndex]->position->x,
                   &level->monsters[monsterIndex]->position->y, &level->monsters[monsterIndex]->health,
                   &level->monsters[monsterIndex]->attack);
            level->monsters[monsterIndex]->symbol = symbol;
        }
    }

    fclose(level_file);
    return 1;
}