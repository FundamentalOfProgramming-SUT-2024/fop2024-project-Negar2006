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

typedef struct Player
{
	Position * position;
	int health;
	int attack;
	int gold;
	int maxHealth;
	int exp;
} Player;

int addMonsters(Level* level);
Monster * selectMonster(int level);
Monster * createMonster(char symbol, int health, int attack, int speed, int defence, int pathfinding);
int killMonster(Monster * monster);
void setStartingPosition(Monster * monster, Room * room,Level *level);
int moveMonsters(Level * level);
int pathfindingRandom(Position * position);
int pathfindingSeek(Position * start, Position * destination);
Monster * getMonsterAt(Position * position, Monster ** monsters);
void drawMonster(Monster * monster);

void setStartingPosition(Monster * monster, Room * room, Level *level){
    monster->position->x = (rand() % (room->width - 2)) + room->position.x + 1;
    monster->position->y = (rand() % (room->height - 2)) + room->position.y + 1;
	level->tile[monster->position->y][monster->position->x] = monster->symbol;
	//mvprintw(monster->position->y, monster->position->x,monster->symbol);
}
int addMonsters(Level* level){
    int x;
    level->monsters = malloc(sizeof(Monster *)* 6);
    level->numberOfMonsters = 0;

    for (x = 0; x < level->numberOfRooms; x++)
    {
        if ((rand() % 2) == 0)
        {
            level->monsters[level->numberOfMonsters] = selectMonster(level->level);
            setStartingPosition(level->monsters[level->numberOfMonsters], level->rooms[x], level);
            level->numberOfMonsters++;
        }
    }
}
Monster * selectMonster(int level){
    int monster;
    switch (level)
    {
        case 1:
        case 2:
        case 3:
            monster = (rand() % 2) + 1;
            break;
        case 4:
        case 5:
            monster = (rand() % 2) + 2;
            break;
        case 6:
            monster = 3;
            break;
    }


    switch (monster)
    {
        case 1: /* spider */
            return createMonster('X', 2, 1, 1, 1, 1);
        case 2: /* goblin */
            return createMonster('G', 5, 3, 1, 1, 2);
        case 3: /* troll */
            return createMonster('T', 15, 5, 1, 1, 1);
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
int killMonster(Monster * monster){
    monster->alive = 0;
    return 1;
}
int moveMonsters(Level * level){
    int x;
    for (x = 0; x < level->numberOfMonsters; x++)
    {
        if (level->monsters[x]->alive == 0)
            continue;

        if (level->monsters[x]->pathfinding == 1)
        {
            pathfindingRandom(level->monsters[x]->position);
        } else
        {
            pathfindingSeek(level->monsters[x]->position, level->user->position);
        }
    }

}
int pathfindingRandom(Position * position){
    int random;

    random = rand() % 5;

    switch (random)
    {
        /* step up */
        case 0:
            if (mvinch(position->y - 1, position->x) == '.')
            {
                position->y = position->y - 1;
            }
            break;

        /* step down */
        case 1:
            if (mvinch(position->y + 1, position->x) == '.')
            {
                position->y = position->y + 1;
            }
            break;

        /* step left */
        case 2:
            if (mvinch(position->y, position->x - 1) == '.')
            {
                position->x = position->x - 1;
            }
            break;

        /* step right */
        case 3:
            if (mvinch(position->y, position->x + 1) == '.')
            {
                position->x = position->x + 1;
            }
            break;

        /* do nothing */
        case 4:
            break;

    }

    return 1;
}
int pathfindingSeek(Position * start, Position * destination){

    if ((abs((start->x - 1) - destination->x) < abs(start->x - destination->x)) && (mvinch(start->y, start->x - 1) == '.'))
    {
        start->x = start->x - 1;

    } else if ((abs((start->x + 1) - destination->x) < abs(start->x - destination->x)) && (mvinch(start->y, start->x + 1) == '.'))
    {
        start->x = start->x + 1;
    
    } else if ((abs((start->y + 1) - destination->y) < abs(start->y - destination->y)) && (mvinch(start->y + 1, start->x) == '.'))
    {
        start->y = start->y + 1;

    } else if ((abs((start->y - 1) - destination->y) < abs(start->y - destination->y)) && (mvinch(start->y - 1, start->x) == '.'))
    {
        start->y = start->y - 1;
    } else
    {
    }

    return 1;
}
Monster * getMonsterAt(Position * position, Monster ** monsters){
    int x;
    for (x = 0; x < 6; x++)
    {
        if ((position->y == monsters[x]->position->y) && (position->x == monsters[x]->position->x))
            return monsters[x];
    }

    return NULL;
}
void drawMonster(Monster * monster){
    if (monster->alive)
    {
        mvprintw(monster->position->y, monster->position->x, monster->string);
    }
}