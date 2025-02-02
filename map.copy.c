#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "monster.c"
extern char **global_tile;


void print_room(Room * room, Level *level);
Room ** room_set(Level *level);
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
void printGameHub(Level * level);
void playerMove(Position * newPosition, Player * user, char ** level);
void drawLevel(Level * level);
void drawLevel_completly(Level * level);
void printGameHub(Level * level);
void draw_circle(int y, int x);


Room * createRoom(int grid, int numberOfDoors)
{
	Room * newRoom;
	newRoom = malloc(sizeof(Room));
	newRoom->numberOfDoors = numberOfDoors;
    newRoom->visible = 0;

	switch (grid)
	{
	case 0:
		newRoom->position.x = 0;
		newRoom->position.y = 0;
		break;
	case 1:
		newRoom->position.x = 33;
		newRoom->position.y = 0;
		break;

	case 2:
		newRoom->position.x = 66;
		newRoom->position.y = 0;
		break;
    case 3:
		newRoom->position.x = 66;
		newRoom->position.y = 14;
		break;
	case 4:
		newRoom->position.x = 33;
		newRoom->position.y = 14;
		break;
    case 5:
		newRoom->position.x = 0;
		newRoom->position.y = 14;
		break;

	}
	newRoom->height = rand() % 6 + 6;
	newRoom->width = rand() % 14 + 6;

	newRoom->position.x += rand() % (30 - newRoom->width) + 1;
	newRoom->position.y += rand() % (14 - newRoom->height) + 1;

	newRoom->cols = malloc(sizeof(Position *) * 2);
	for (int i = 0; i < 2; i++){
		newRoom->cols[i] = malloc(sizeof(Position));
	}
	newRoom->cols[0]->x = rand() % (newRoom->width - 2) + newRoom->position.x + 1;
	newRoom->cols[0]->y = rand() % (newRoom->height - 2) + newRoom->position.y + 1;

	newRoom->cols[1]->x = rand() % (newRoom->width - 2) + newRoom->position.x + 1;
	newRoom->cols[1]->y = rand() % (newRoom->height - 2) + newRoom->position.y + 1;

	newRoom->doors = malloc(sizeof(Door *) * numberOfDoors);

	for (int i = 0; i < numberOfDoors; i++){
		newRoom->doors[i] = malloc(sizeof(Door));
		newRoom->doors[i]->connected = 0;
	}
	newRoom->doors[0]->position.x = rand() % (newRoom->width - 2) + newRoom->position.x + 1;
	newRoom->doors[0]->position.y = newRoom->position.y;

	newRoom->doors[1]->position.x = rand() % (newRoom->width - 2) + newRoom->position.x + 1;
	newRoom->doors[1]->position.y = newRoom->position.y + newRoom->height - 1;

	newRoom->doors[2]->position.y = rand() % (newRoom->height - 2) + newRoom->position.y + 1;
	newRoom->doors[2]->position.x = newRoom->position.x;

	newRoom->doors[3]->position.y = rand() % (newRoom->height - 2) + newRoom->position.y + 1;
	newRoom->doors[3]->position.x = newRoom->position.x + newRoom->width - 1;


	return newRoom;
}
void print_room(Room * room,Level *level)
{
	level = malloc(sizeof(Level));
	int x,y;
	for (x = room->position.x; x < room->position.x + room->width; x++) {
		mvprintw(room->position.y, x, "_");
        global_tile[room->position.y][x] = '_';
		mvprintw(room->position.y + room->height - 1, x, "_");
        global_tile[room->position.y + room->height - 1][x] = '_';

	}
	for (y = room->position.y + 1; y < room->position.y + room->height - 1; y++)
	{
		mvprintw(y, room->position.x, "|");
        global_tile[y][room->position.x] = '|';
		mvprintw(y, room->position.x + room->width - 1, "|");
        global_tile[y][room->position.x + room->width - 1] = '|';

		for (x = room->position.x + 1; x <room->position.x + room->width - 1; x++)
		{
			mvprintw(y, x, ".");
            global_tile[y][x] = '.';
		}
	}
	mvprintw(room->cols[0]->y,room->cols[0]->x,"O");
	global_tile[room->cols[0]->y][room->cols[0]->x] = 'O';
	mvprintw(room->cols[1]->y,room->cols[1]->x,"O");
	global_tile[room->cols[1]->y][room->cols[1]->x] = 'O';

	mvprintw(room->doors[0]->position.y, room->doors[0]->position.x, "+");
    global_tile[room->doors[0]->position.y][room->doors[0]->position.x] = '+';
	mvprintw(room->doors[1]->position.y, room->doors[1]->position.x, "+");
    global_tile[room->doors[1]->position.y][room->doors[1]->position.x] = '+';
	mvprintw(room->doors[2]->position.y, room->doors[2]->position.x, "+");
    global_tile[room->doors[2]->position.y][room->doors[2]->position.x] = '+';
	mvprintw(room->doors[3]->position.y, room->doors[3]->position.x, "+");
    global_tile[room->doors[3]->position.y][room->doors[3]->position.x] = '+';

	add_monster(level);

}
Room ** room_set(Level *level) {
	int x;
	Room ** rooms;
	rooms = malloc(sizeof(Room)*6);

	for (x = 0; x < 6; x++) {
		rooms[x] = createRoom(x, 4);
		print_room(rooms[x],level);
	}
	return rooms;
}
void addPositionYX(int ** frontier, int frontierCount, int y, int x) {
	frontier[frontierCount][0] = y;
	frontier[frontierCount][1] = x;
}
int checkPosition(int y, int x) {
	char temp = mvinch(y, x);

	if (temp == '.' || temp == '|' || temp == '_')
		return 0;
	else
		return 1;
}
int addNeighbors(int ** frontier, int frontierCount, int *** cameFrom, int y, int x) {
	if (y > 0 && cameFrom[y - 1][x][0] < 0 && checkPosition(y - 1, x))
	{
		addPositionYX(frontier, frontierCount, y - 1, x);
		frontierCount++;
		cameFrom[y - 1][x][0] = y;
		cameFrom[y - 1][x][1] = x;
	}

	if (y < (29) && cameFrom[y + 1][x][0] < 0 && checkPosition(y + 1, x))
	{
		addPositionYX(frontier, frontierCount, y + 1, x);
		frontierCount++;
		cameFrom[y + 1][x][0] = y;
		cameFrom[y + 1][x][1] = x;
	}

	if (x < (99) && cameFrom[y][x + 1][0] < 0 && checkPosition(y, x + 1))
	{
		addPositionYX(frontier, frontierCount, y, x + 1);
		frontierCount++;
		cameFrom[y][x + 1][0] = y;
		cameFrom[y][x + 1][1] = x;
	}

	if (x > 0 && cameFrom[y][x - 1][0] < 0 && checkPosition(y, x - 1))
	{
		addPositionYX(frontier, frontierCount, y, x - 1);
		frontierCount++;
		cameFrom[y][x - 1][0] = y;
		cameFrom[y][x - 1][1] = x;
	}

	return frontierCount;

}
void Connect_doors(Position * start, Position * end) {
	int i, j;
	int x, y;
	int tempY;
	int ** frontier = malloc(sizeof(int*) * 4000);
	int *** cameFrom = malloc(sizeof(int**) * 40 );

	int frontierIndex = 0;
	int frontierCount = 0;

	for (i = 0; i < 4000; i++)
	{
		frontier[i] = malloc(sizeof(int)*2);
	}

	for (i = 0; i < 40; i++){
		cameFrom[i] = malloc(sizeof(int*)* 100);
		for (j = 0; j < 100; j++){
			cameFrom[i][j] = malloc(sizeof(int)*2);
			cameFrom[i][j][0] = -1;
			cameFrom[i][j][1] = -1;
		}
	}
	cameFrom[start->y][start->x][0] = -2;
	cameFrom[start->y][start->x][1] = -2;

	addPositionYX(frontier, frontierCount, start->y, start->x);
	frontierCount++;
	while (frontierIndex < frontierCount)
	{
		y = frontier[frontierIndex][0];
		x = frontier[frontierIndex][1];

		frontierIndex++;

		if (y == end->y && x == end->x)
		{
			break;
		}

		frontierCount = addNeighbors(frontier, frontierCount, cameFrom, y, x);
	}
	y = end->y;
	x = end->x;

	while (y != start->y || x != start->x )
	{
		tempY = y;
		y = cameFrom[tempY][x][0];
		x = cameFrom[tempY][x][1];
		//mvprintw(y, x, "#");
        global_tile[y][x] = '#';
	}
}
Player * playerSetUp() {
	Player * newPlayer;
	newPlayer = malloc(sizeof(Player));
	newPlayer->position = malloc(sizeof(Position));
	newPlayer->health = 20;
	newPlayer->attack = 1;
	newPlayer->gold = 0;
	newPlayer->exp = 0;
	newPlayer->maxHealth = 20;

	return newPlayer;
}
void placePlayer(Room ** rooms, Player * user) {
	user->position->x = rooms[0]->position.x + 1;
	user->position->y = rooms[0]->position.y + 1;
}
Position * handleInput(int input, Player * user, Level *level){
	Position * newPosition;
	newPosition = malloc(sizeof(Position));

	if(input == 'f') {
		input = getch();
		switch(input) {
		case '8':
			newPosition->x = user->position->x;
			newPosition->y = user->position->y -1;
			while(mvinch(newPosition->y,newPosition->x) != '#' && mvinch(newPosition->y,newPosition->x) != '_' && mvinch(newPosition->y,newPosition->x)!='>' && mvinch(newPosition->y,newPosition->x)!='|') {
				newPosition->y--;
			}
			break;
		case '2':
			newPosition->x = user->position->x;
			newPosition->y = user->position->y +1;
			while(mvinch(newPosition->y,newPosition->x) != '#' && mvinch(newPosition->y,newPosition->x) != '_' && mvinch(newPosition->y,newPosition->x)!='>' && mvinch(newPosition->y,newPosition->x)!='|') {
				newPosition->y++;
			}
			break;
		case '6':
			newPosition->x = user->position->x + 1;
			newPosition->y = user->position->y;
			while(mvinch(newPosition->y,newPosition->x) != '#' && mvinch(newPosition->y,newPosition->x) != '_' && mvinch(newPosition->y,newPosition->x)!='>' && mvinch(newPosition->y,newPosition->x)!='|') {
				newPosition->x++;
			}
			break;
		case '4':
			newPosition->x = user->position->x - 1;
			newPosition->y = user->position->y;
			while(mvinch(newPosition->y,newPosition->x) != '#' && mvinch(newPosition->y,newPosition->x) != '_' && mvinch(newPosition->y,newPosition->x)!='>' && mvinch(newPosition->y,newPosition->x)!='|') {
				newPosition->x--;
			}
			break;
		case '9':
			newPosition->x = user->position->x + 1;
			newPosition->y = user->position->y -1;
			while(mvinch(newPosition->y,newPosition->x) != '#' && mvinch(newPosition->y,newPosition->x) != '_' && mvinch(newPosition->y,newPosition->x)!='>' && mvinch(newPosition->y,newPosition->x)!='|') {
				newPosition->y--;
				newPosition->x++;
			}
			break;
		case '7':
			newPosition->x = user->position->x - 1;
			newPosition->y = user->position->y -1;
			while(mvinch(newPosition->y,newPosition->x) != '#' && mvinch(newPosition->y,newPosition->x) != '_' && mvinch(newPosition->y,newPosition->x)!='>' && mvinch(newPosition->y,newPosition->x)!='|') {
				newPosition->y--;
				newPosition->x--;
			}
			break;
		case '1':
			newPosition->x = user->position->x - 1;
			newPosition->y = user->position->y + 1;
			while(mvinch(newPosition->y,newPosition->x) != '#' && mvinch(newPosition->y,newPosition->x) != '_' && mvinch(newPosition->y,newPosition->x)!='>' && mvinch(newPosition->y,newPosition->x)!='|') {
				newPosition->y++;
				newPosition->x--;
			}
			break;
		case '3':
			newPosition->x = user->position->x + 1;
			newPosition->y = user->position->y + 1;
			while(mvinch(newPosition->y,newPosition->x) != '#' && mvinch(newPosition->y,newPosition->x) != '_' && mvinch(newPosition->y,newPosition->x)!='>' && mvinch(newPosition->y,newPosition->x)!='|') {
				newPosition->y++;
				newPosition->x++;
			}
			break;
		default:
			break;

		}
	}
	switch (input)
	{
	case '8':
		newPosition->y = user->position->y - 1;
		newPosition->x = user->position->x;
		break;

	case '2':
		newPosition->y = user->position->y + 1;
		newPosition->x = user->position->x;
		break;

	case '4':
		newPosition->y = user->position->y;
		newPosition->x = user->position->x - 1;
		break;

	case '6':
		newPosition->y = user->position->y;
		newPosition->x = user->position->x + 1;
		break;
	case '1':
		newPosition->y = user->position->y + 1;
		newPosition->x = user->position->x - 1;
		break;

	case '3':
		newPosition->y = user->position->y + 1;
		newPosition->x = user->position->x + 1;
		break;

	case '7':
		newPosition->y = user->position->y - 1;
		newPosition->x = user->position->x - 1;
		break;

	case '9':
		newPosition->y = user->position->y - 1;
		newPosition->x = user->position->x + 1;
		break;
    case 'q':
        drawLevel_completly(level);
        break;
	default:
		break;

	}
	return newPosition;

}
void playerMove(Position * newPosition, Player * user, char ** level) {

	
	char buffer[8];
	sprintf(buffer,"%c",level[user->position->y][user->position->x]);
	mvprintw(user->position->y, user->position->x,"%s",buffer);

	user->position->y = newPosition->y;
	user->position->x = newPosition->x;
	mvprintw(user->position->y, user->position->x,"@");
	move(user->position->y, user->position->x);
}
void drawPlayer(Player * player) {
	mvprintw(player->position->y, player->position->x, "@");
	move(player->position->y, player->position->x);
}
Level * createLevel(int level) {
	Level * newLevel;
	newLevel = malloc(sizeof(Level));

	newLevel->level = level;
	newLevel->numberOfRooms = 6;
	newLevel->rooms = room_set(newLevel);
	newLevel->tile = global_tile;
	newLevel->user = playerSetUp();
	// add_monster(newLevel);
	placePlayer(newLevel->rooms, newLevel->user);

    newLevel->rooms[0]->visible = 1;

	drawLevel(newLevel);

    char input = getch();
    if(input == 'q'){
        drawLevel_completly(newLevel);
        return newLevel;
    }
    
	return newLevel;
}

void drawLevel(Level * level){
	clear();
	int x, y, i;
	 for (y = 0; y < 40; y++){
	 	for (x = 0; x < 100; x++){
	 		mvaddch(y,x,' ');
	 	}
	 }
    print_room(level->rooms[0],level);
    for(i = 1; i < 6;i++){
        if(level->rooms[i]->visible){
            print_room(level->rooms[i],level);
        }
    }
	Connect_doors(&level->rooms[1]->doors[2]->position,&level->rooms[0]->doors[3]->position);
    Connect_doors(&level->rooms[2]->doors[2]->position,&level->rooms[1]->doors[3]->position);
    Connect_doors(&level->rooms[3]->doors[0]->position,&level->rooms[2]->doors[1]->position);
    Connect_doors(&level->rooms[4]->doors[3]->position,&level->rooms[3]->doors[2]->position);
    Connect_doors(&level->rooms[5]->doors[3]->position,&level->rooms[4]->doors[2]->position);
	add_monster(level);
	drawPlayer(level->user);
	 refresh();
     
}
void drawLevel_completly(Level * level){
	clear();
	int x, y, i;
	 for (y = 0; y < 40; y++){
	 	for (x = 0; x < 100; x++){
	 		mvaddch(y,x,' ');
	 	}
	 }
	 for(int y = 0 ; y < 40; y++){
		for(int x = 0 ; x < 100;x++){
			if(global_tile[y][x] == '#'){
				mvprintw(y,x,"#");
			}
		}
	  }
    for(i = 0; i < 6;i++){
        print_room(level->rooms[i],level);
    }
	drawPlayer(level->user);
	add_monster(level);
    Connect_doors(&level->rooms[1]->doors[2]->position,&level->rooms[0]->doors[3]->position);
    Connect_doors(&level->rooms[2]->doors[2]->position,&level->rooms[1]->doors[3]->position);
    Connect_doors(&level->rooms[3]->doors[0]->position,&level->rooms[2]->doors[1]->position);
    Connect_doors(&level->rooms[4]->doors[3]->position,&level->rooms[3]->doors[2]->position);
    Connect_doors(&level->rooms[5]->doors[3]->position,&level->rooms[4]->doors[2]->position);

	  
	  refresh();
	  
}

void check_next_step(Position * newPosition, Level * level) {
	Player * user;
	user = level->user;
    switch (mvinch(newPosition->y, newPosition->x))
	{
	case '.':
	case '#':
	case '+':
		playerMove(newPosition, user, global_tile);
		break;
    case 'G':
        user->gold++;
        printGameHub(level);
        break;
	default:
		break;
	}
	if(global_tile[newPosition->y][newPosition->x] == '+' || global_tile[newPosition->y][newPosition->x] == '#'){
		draw_circle(newPosition->y,newPosition->x);
	}
    for(int i = 1 ; i < 6;i++){
        for(int j = 0; j < 4;j++){
            Door *door = level->rooms[i]->doors[j];
            if(newPosition->y == door->position.y && newPosition->x == door->position.x){
                int connect = i % 6;
                level->rooms[connect]->visible = 1;
                drawLevel(level);
                return;
            }
        }
    }
}
void printGameHub(Level * level){

	mvprintw(31,0,"______________________________________________________________");
	mvprintw(33,0,"______________________________________________________________");
	mvprintw(32, 0, " Level: %d   ", level->level);
	printw(" Gold: %d   ", level->user->gold);
	printw(" Hp: %d(%d)   ", level->user->health, level->user->maxHealth);
	printw(" Attack: %d   ", level->user->attack);
	printw(" Exp: %d   ", level->user->exp);
	printw(" ");
}
void draw_circle(int y, int x){
	if(global_tile[y][x] == '#'){
		mvprintw(y,x,"#");
	}
	if(global_tile[y-1][x] == '#'){
		mvprintw(y-1,x,"#");
	}
	if(global_tile[y+1][x] == '#'){
		mvprintw(y+1,x,"#");
	}
	if(global_tile[y-2][x] == '#'){
		mvprintw(y-2,x,"#");
	}
	if(global_tile[y+2][x] == '#'){
		mvprintw(y+2,x,"#");
	}
	if(global_tile[y][x -1] == '#'){
		mvprintw(y,x -1,"#");
	}
	if(global_tile[y][x+1] == '#'){
		mvprintw(y,x+1,"#");
	}
	if(global_tile[y][x-2] == '#'){
		mvprintw(y,x-2,"#");
	}
	if(global_tile[y][x+2] == '#'){
		mvprintw(y,x+2,"#");
	}
	if(global_tile[y-3][x] == '#'){
		mvprintw(y-3,x,"#");
	}
	if(global_tile[y+3][x] == '#'){
		mvprintw(y+3,x,"#");
	}
	if(global_tile[y][x-3] == '#'){
		mvprintw(y,x-3,"#");
	}
	if(global_tile[y][x+3] == '#'){
		mvprintw(y,x+3,"#");
	}
	
}




