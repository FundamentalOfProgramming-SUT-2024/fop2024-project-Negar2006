#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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
    //Room * spellroom;
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
} Player;


int addMonsters(Level* level);
Monster * selectMonster(int level);
Monster * createMonster(char symbol, int health, int attack, int speed, int defence, int pathfinding);
void setStartingPosition(Monster * monster, Room * room,Level *level);
void moveMonsters(Level * level);
int pathfindingRandom(Position * position);
int pathfindingSeek(Position * start, Position * destination);
void drawMonster(Monster * monster, Level *level);

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
            return createMonster('S', 20, 1, 1, 1, 2);
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
            //level->tile[monster->position->y][monster->position->x]
            if (monster->pathfinding == 0) {
                continue;
            } else if (monster->pathfinding == 1) {
                pathfindingRandom(monster->position);
            } else {
                pathfindingSeek(monster->position, level->user->position);
            }
            
            // Draw monster at new position
            //mvaddch(monster->position->y, monster->position->x, monster->symbol);
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

