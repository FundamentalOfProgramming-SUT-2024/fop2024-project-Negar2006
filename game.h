#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include <time.h>

char ** global_tile;

typedef struct {
	int x;
	int y;
} Position;

typedef struct {
    char **tile;
	int level;
	int numberOfRooms;
	struct Room ** rooms;
	struct Monster ** monsters;
	int numberOfMonsters;
	struct Player * user;
} Level;

typedef struct{
	Position position;
	int height;
	int width;
	struct Door ** doors;
	int numberOfDoors;
    int visible;
	char ** cols;
} Room;

typedef struct {
	Position position;
	int connected;
} Door;

typedef struct {
	Position * position;
	int health;
	int attack;
	int gold;
	int maxHealth;
	int exp;
} Player;


typedef struct{
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


Room * createRoom(int grid, int numberOfDoors);
void print_room(Room * room);
Room ** room_set();
void Connect_doors(Position * start, Position * end);
void addPositionYX(int ** frontier, int frontierCount, int y, int x);
int checkPosition(int y, int x);
int addNeighbors(int ** frontier, int frontierCount, int *** cameFrom, int y, int x);
Level * createLevel(int level);
char ** saveLevelPositions();
Player * playerSetUp();
Position * handleInput(int input, Player * user,Level *level);
void check_next_step(Position * newPosition, Level * level);
void placePlayer(Room ** rooms, Player * user);
void drawPlayer(Player * player);
Room * createRoom(int grid, int numberOfDoors);
int drawRoom(Room * room);
void setStartingPosition(Monster * monster, Room * room);
void printGameHub(Level * level);
void playerMove(Position * newPosition, Player * user, char ** level);
void drawLevel_completly(Level * level);
void printGameHub(Level * level);
void draw_circle(int y, int x);
