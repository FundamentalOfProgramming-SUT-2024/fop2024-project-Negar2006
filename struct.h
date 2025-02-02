
typedef struct {
	int x;
	int y;
} Position;

typedef struct {
	Position start;
	Position end;
	int is_visible;
}Stairs;

typedef struct Door
{
	Position position;
	int connected;
} Door;

typedef struct {
	Position position;
	int height;
	int width;
	struct Door ** doors;
	int numberOfDoors;
    int visible;
	Position **cols;
} Room;

typedef struct Level
{
    char **tile;
	int level;
	int numberOfRooms;
	struct Room ** rooms;
	struct Monster ** monsters;
	int numberOfMonsters;
	struct Player * user;
	Stairs stairs[5];
} Level;

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