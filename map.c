#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include <wchar.h>

#include "weapon.c"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


void printGameHub(Level * level);
Room * createRoom(int grid, int numberOfDoors);
void print_room(Room * room);
void placePlayer(Room ** rooms, Player * user);
void addPositionYX(int ** frontier, int frontierCount, int y, int x);
int checkPosition(int y, int x);
int addNeighbors(int ** frontier, int frontierCount, int *** cameFrom, int y, int x);
void pathFind(Position * start, Position * end);
void connectDoors(Level * level);
Player * playerSetUp();
Position * handleInput(int input, Player * user,Level *level);
void visited_tiles(Level * level);
void playerMove(Position * newPosition, Player * user, Level * level);
void drawPlayer(Player * player);
int **initializeVisitedArray();
Level * createLevel(int level);
void drawLevel(Level * level);
Room ** room_set(Level *level);
char ** saveLevelPositions();
void revealRoom(Level *level, Room *room);
void check_next_step(Position * newPosition, Level * level);
void placeTraps(Level * level);
int gameLoop();

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
		newRoom->position.y = 4;
		break;
	case 1:
		newRoom->position.x = 33;
		newRoom->position.y = 4;
		break;

	case 2:
		newRoom->position.x = 66;
		newRoom->position.y = 4;
		break;
    case 3:
		newRoom->position.x = 66;
		newRoom->position.y = 18;
		break;
	case 4:
		newRoom->position.x = 33;
		newRoom->position.y = 18;
		break;
    case 5:
		newRoom->position.x = 0;
		newRoom->position.y = 18;
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

	newRoom->window = malloc(sizeof(Position *) * 2);
	for (int i = 0; i < 2; i++){
		newRoom->window[i] = malloc(sizeof(Position));
	}

	newRoom->window[0]->x = rand() % (newRoom->width - 2) + newRoom->position.x + 1;
	newRoom->window[0]->y = newRoom->position.y;

	newRoom->window[1]->x = rand() % (newRoom->width - 2) + newRoom->position.x + 1;
	newRoom->window[1]->y = newRoom->position.y + newRoom->height - 1;

	newRoom->doors = malloc(sizeof(Door *) * numberOfDoors);

	for (int i = 0; i < numberOfDoors; i++){
		newRoom->doors[i] = malloc(sizeof(Door));
		newRoom->doors[i]->connected = 0;
	}
	// newRoom->doors[0]->position.x = rand() % (newRoom->width - 2) + newRoom->position.x + 1;
	// newRoom->doors[0]->position.y = newRoom->position.y;

	// newRoom->doors[1]->position.x = rand() % (newRoom->width - 2) + newRoom->position.x + 1;
	// newRoom->doors[1]->position.y = newRoom->position.y + newRoom->height - 1;

	newRoom->doors[0]->position.y = rand() % (newRoom->height - 2) + newRoom->position.y + 1;
	newRoom->doors[0]->position.x = newRoom->position.x;

	newRoom->doors[1]->position.y = rand() % (newRoom->height - 2) + newRoom->position.y + 1;
	newRoom->doors[1]->position.x = newRoom->position.x + newRoom->width - 1;

	return newRoom;
}
void print_room(Room * room)
{
	int x,y;
	for (x = room->position.x; x < room->position.x + room->width; x++) {
		mvprintw(room->position.y, x, "_");
		mvprintw(room->position.y + room->height - 1, x, "_");

	}
	for (y = room->position.y + 1; y < room->position.y + room->height - 1; y++)
	{
		mvprintw(y, room->position.x, "|");
		mvprintw(y, room->position.x + room->width - 1, "|");

		for (x = room->position.x + 1; x <room->position.x + room->width - 1; x++)
		{
			mvprintw(y, x, ".");
		}
	}
	mvprintw(room->cols[0]->y,room->cols[0]->x,"O");
	mvprintw(room->cols[1]->y,room->cols[1]->x,"O");

	mvprintw(room->window[0]->y, room->window[0]->x, "=");
    mvprintw(room->window[1]->y, room->window[1]->x, "=");

	mvprintw(room->doors[0]->position.y, room->doors[0]->position.x, "+");
	mvprintw(room->doors[1]->position.y, room->doors[1]->position.x, "+");

}
void placePlayer(Room ** rooms, Player * user) {
	user->position->x = rooms[0]->position.x + 1;
	user->position->y = rooms[0]->position.y + 1;
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
void pathFind(Position * start, Position * end) {
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
	while (frontierIndex < frontierCount){
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
		mvprintw(y, x, "#");
	}
}
void connectDoors(Level * level){
    int i, j;
    int randomRoom, randomDoor;
    int count;

    for (i = 0; i < 6; i++){
        for (j = 0; j < 2; j++){
            if (level->rooms[i]->doors[j]->connected == 1) {
                continue;
            }
            count = 0;
            while (count < 2){
                randomRoom = rand() % 6;
                randomDoor = rand() % 2;

                if (level->rooms[randomRoom]->doors[randomDoor]->connected == 1 || randomRoom == i){
                    count++;
                    continue;
                }
                pathFind(&level->rooms[randomRoom]->doors[randomDoor]->position, &level->rooms[i]->doors[j]->position);

                level->rooms[randomRoom]->doors[randomDoor]->connected = 1;
                level->rooms[i]->doors[j]->connected = 1;
                break;
            }
        }
    }
}
Player * playerSetUp() {
	Player * newPlayer;
	newPlayer = malloc(sizeof(Player));
	newPlayer->position = malloc(sizeof(Position));
	newPlayer->level = 1;
	newPlayer->health = 100;
	newPlayer->attack = 1;
	newPlayer->gold = 0;
	newPlayer->exp = 0;
	newPlayer->maxHealth = 100;
	newPlayer->hunger = 0;
	newPlayer->excellent_food = 0;
	newPlayer->magical_food = 0;
	newPlayer->normal_food = 0;
	newPlayer->spoiled_food = 0;
    newPlayer->inventory = createInventory();

	return newPlayer;
}
Position * handleInput(int input, Player * user, Level *level){
	Position * newPosition;
	newPosition = malloc(sizeof(Position));
	newPosition->x = user->position->x;
	newPosition->y = user->position->y;

	if(input == 'f'){
		int inputt = getch();
		switch(inputt){
			case '8':
			    newPosition->y = user->position->y - 1;
                newPosition->x = user->position->x;
                while(newPosition->y >= 0  && level->tile[newPosition->y][newPosition->x] != '|' && level->tile[newPosition->y][newPosition->x] != '_' && level->tile[newPosition->y][newPosition->x] != '+' && level->tile[newPosition->y][newPosition->x] != 'O'){
                    newPosition->y--;}
                newPosition->y++; 
                break;
			 case '2':
                newPosition->y = user->position->y + 1;
                newPosition->x = user->position->x;
                while(level->tile[newPosition->y][newPosition->x] != '|' && level->tile[newPosition->y][newPosition->x] != '_' && level->tile[newPosition->y][newPosition->x] != '+' && level->tile[newPosition->y][newPosition->x] != 'O'){
                    newPosition->y++;}
                newPosition->y--; 
                break;
            case '4':
                newPosition->y = user->position->y;
                newPosition->x = user->position->x - 1;
                while(level->tile[newPosition->y][newPosition->x] != '|' && level->tile[newPosition->y][newPosition->x] != '_' && level->tile[newPosition->y][newPosition->x] != '+' && level->tile[newPosition->y][newPosition->x] != 'O'){
                    newPosition->x--;}
                newPosition->x++; 
                break;
            case '6':
                newPosition->y = user->position->y;
                newPosition->x = user->position->x + 1;
                while(level->tile[newPosition->y][newPosition->x] != '|' && level->tile[newPosition->y][newPosition->x] != '_' && level->tile[newPosition->y][newPosition->x] != '+' && level->tile[newPosition->y][newPosition->x] != 'O'){
                    newPosition->x++;
                }
                newPosition->x--;
                break;
            case '1':
                newPosition->y = user->position->y + 1;
                newPosition->x = user->position->x - 1;
                while(level->tile[newPosition->y][newPosition->x] != '|' && level->tile[newPosition->y][newPosition->x] != '_' && level->tile[newPosition->y][newPosition->x] != '+' && level->tile[newPosition->y][newPosition->x] != 'O'){
                    newPosition->y++;
                    newPosition->x--;}
                newPosition->y--; 
                newPosition->x++; 
                break;
            case '3':
                newPosition->y = user->position->y + 1;
                newPosition->x = user->position->x + 1;
                while(level->tile[newPosition->y][newPosition->x] != '|' && level->tile[newPosition->y][newPosition->x] != '_' && level->tile[newPosition->y][newPosition->x] != '+' && level->tile[newPosition->y][newPosition->x] != 'O'){
                    newPosition->y++;
                    newPosition->x++;}
                newPosition->y--; 
                newPosition->x--;
                break;
            case '7':
                newPosition->y = user->position->y - 1;
                newPosition->x = user->position->x - 1;
                while(level->tile[newPosition->y][newPosition->x] != '|' && level->tile[newPosition->y][newPosition->x] != '_' && level->tile[newPosition->y][newPosition->x] != '+' && level->tile[newPosition->y][newPosition->x] != 'O'){
                    newPosition->y--;
                    newPosition->x--;}
                newPosition->y++; 
                newPosition->x++; 
                break;
            case '9':
                newPosition->y = user->position->y - 1;
                newPosition->x = user->position->x + 1;
                while(level->tile[newPosition->y][newPosition->x] != '|' && level->tile[newPosition->y][newPosition->x] != '_' && level->tile[newPosition->y][newPosition->x] != '+' && level->tile[newPosition->y][newPosition->x] != 'O'){
                    newPosition->y--;
                    newPosition->x++;}
                newPosition->y++; 
                newPosition->x--; 
                break;
		    }
	}
	switch (input){
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
	case 'n':
	    visited_tiles(level);
	 	break;
	case 'E':
    displayFoodMenu(user);
    int choice = getch() - '0'; 
    switch (choice) {
        case 1:
            if (user->normal_food > 0) {
                user->normal_food--;
                user->hunger -= normal_food.hunger_reduction;
            }
            break;
        case 2:
            if (user->excellent_food > 0) {
                user->excellent_food--;
                user->hunger -= excellent_food.hunger_reduction;
            }
            break;
        case 3:
            if (user->spoiled_food > 0) {
                user->spoiled_food--;
                user->hunger -= spoiled_food.hunger_reduction;
            }
            break;
        case 4:
            if (user->magical_food > 0) {
                user->magical_food--;
                user->hunger -= magical_food.hunger_reduction;
            }
            break;
        default:
            break;
    }
    if (user->hunger < 0) user->hunger = 0;
    if (user->hunger >= 100) {
        user->health -= 10;
        user->hunger = 0;
    }
    clear();
    drawLevel(level);
    drawPlayer(user);
    mvprintw(0, 0, "Hunger: %d, Health: %d", user->hunger, user->health);
    refresh();
    break;
    case 'i':
        displayWeaponMenu(user->inventory);
        break;
    case 'f':
        useWeapon(user, user->inventory);
        break;
	case 'N':
	    Mix_HaltMusic();
        Mix_CloseAudio();
		break;
	default:
		break;	
	}
	return newPosition;
}
void visited_tiles(Level * level){
	for(int y = 0 ; y < 40; y++){
		for(int x = 0; x < 100; x++){
			level->visited[y][x] = 1;
		}
	}
	level->trap_revealed = 1;
	printGameHub(level);
	drawLevel(level);
}
void playerMove(Position * newPosition, Player * user, Level * level) {
	int Y = user->position->y;
	int X = user->position->x;
	mvaddch(Y,X,level->tile[Y][X]);
	user->position->y = newPosition->y;
	user->position->x = newPosition->x;

	level->visited[newPosition->y][newPosition->x] = 1;
	drawPlayer(user);
}
void drawPlayer(Player * player) {
	attron(COLOR_PAIR(2)); 
    mvprintw(player->position->y, player->position->x, "@");
    attroff(COLOR_PAIR(2));
	move(player->position->y, player->position->x);
}
int **initializeVisitedArray(){
	int **visited = malloc(sizeof(int *) * 40);
	for(int y = 0 ; y < 40; y++){
		visited[y] = malloc(sizeof(int) * 100);
		for(int x = 0 ; x < 100 ; x++){
			visited[y][x] = 0;
		}
	}
	return visited;
}
Level * createLevel(int level) {
	clear();
	Level * newLevel;
	newLevel = malloc(sizeof(Level));
	newLevel->level = level;
	newLevel->numberOfRooms = 6;
	newLevel->rooms = room_set(newLevel);
	connectDoors(newLevel);
    newLevel->tile = saveLevelPositions();
	newLevel->comment = malloc(sizeof(char));
	newLevel->visited = initializeVisitedArray();
	newLevel->user = playerSetUp();
	placePlayer(newLevel->rooms, newLevel->user);
	drawPlayer(newLevel->user);
	addMonsters(newLevel);
	newLevel->comment = "attention: You can only keep moving until you reach the wall AND be careful of the monsters!!!";
	placeTraps(newLevel);
	placeFood(newLevel);
	add_gold(newLevel);

	Room *firstroom = newLevel->rooms[0];
	for(int y = firstroom->position.y; y < firstroom->position.y + firstroom->height;y++){
		for(int x = firstroom->position.x; x < firstroom->position.x + firstroom->width; x++){
			newLevel->visited[y][x] = 1;
		}
	}

	newLevel->rooms[5]->stairs = malloc(sizeof(Position));
	int Y = newLevel->rooms[5]->stairs->y = rand() % (newLevel->rooms[5]->height - 2) + newLevel->rooms[5]->position.y + 1;
	int X = newLevel->rooms[5]->stairs->x = rand() % (newLevel->rooms[5]->width - 2) + newLevel->rooms[5]->position.x + 1;
	newLevel->tile[Y][X] = '>';
	mvprintw(Y,X,">");


	int itemCount1 = 5;
    for (int i = 0; i < itemCount1; i++) {
        Room *room = newLevel->rooms[rand() % newLevel->numberOfRooms];
        int x = rand() % (room->width - 2) + room->position.x + 1;
        int y = rand() % (room->height - 2) + room->position.y + 1;

        int itemType = rand() % 5; 
        if (itemType == 0) {
            newLevel->tile[y][x] = 'm'; 
        } else if(itemType == 1){
            newLevel->tile[y][x] = 'd'; 
        }else if(itemType == 2){
			newLevel->tile[y][x] = 'v';
		}else if(itemType == 3){
			newLevel->tile[y][x] = 'a';
		}else {
			newLevel->tile[y][x] = 's';
		}
    }
	int itemCount2 = 3;
    for (int i = 0; i < itemCount2; i++) {
        Room *room = newLevel->rooms[rand() % newLevel->numberOfRooms];
        int x = rand() % (room->width - 2) + room->position.x + 1;
        int y = rand() % (room->height - 2) + room->position.y + 1;

        int itemType = rand() % 3; 
        if (itemType == 0) {
            newLevel->tile[y][x] = 'h'; 
        } else if(itemType == 1){
            newLevel->tile[y][x] = 'p'; 
        }else {
			newLevel->tile[y][x] = 'g';
		}
    }
	drawLevel(newLevel);
	refresh();
    
	return newLevel;
}
void drawLevel(Level * level){
	setlocale(LC_ALL, "");
	int x, y, i;
	for (y = 0; y < 40; y++){
	 	for (x = 0; x < 100; x++){
			if(level->visited[y][x]){
	 		   switch(level->tile[y][x]) {
                    case 'm':
					    attron(COLOR_PAIR(1));
					    const wchar_t symbol1[] = L"\U00002692";
                        mvaddnwstr(y, x, symbol1,-1); 
						attroff(COLOR_PAIR(1));
                        break;
					case 'd':
					    const wchar_t symbol2[] = L"\U0001F5E1";
                        mvaddnwstr(y, x, symbol2,-1);
						break;
					case 'v':
					    const wchar_t symbol3[] = L"\U00002734";//\U0001FA84
						//assaye jadoyii
                        mvaddnwstr(y, x, symbol3,-1);
						break;
					case 'a':
					    const wchar_t symbol4[] = L"\U000027B3";
                        mvaddnwstr(y, x, symbol4,-1);
						break;
					case 's':
					    const wchar_t symbol5[] = L"\U00002694";
                        mvaddnwstr(y, x, symbol5,-1);
						break;
					case 'h':
					    attron(COLOR_PAIR(3));
					    const wchar_t symbol6[] = L"\U00002665";
                        mvaddnwstr(y, x, symbol6,-1);
						attroff(COLOR_PAIR(3));
						break;
					case 'p':
					    const wchar_t symbol7[] = L"\U0000269C";//\U0001F680
						//moshak
                        mvaddnwstr(y, x, symbol7,-1);
						break;
					case 'g':
					    const wchar_t symbol8[] = L"\U000026B0";//\U0001F4A5
						//damage
                        mvaddnwstr(y, x, symbol8,-1);
						break;
					
                    default:
                        mvaddch(y, x, level->tile[y][x]);
                }
			}else{
				mvaddch(y,x,' ');
			}
	 	}
	}
	for(i = 0 ; i < level->numberOfMonsters;i++){
		drawMonster(level->monsters[i] , level);
	}
	printGameHub(level);
	drawPlayer(level->user);
}
Room ** room_set(Level *level) {
	int x;
	Room ** rooms;
	rooms = malloc(sizeof(Room)*6);

	for (x = 0; x < 6; x++) {
		rooms[x] = createRoom(x, 4);
		print_room(rooms[x]);
	}
	return rooms;
}
char **saveLevelPositions(){
	int x, y;
	char **positions;
	positions = malloc(sizeof(char *) * 40);
	for(y = 0 ; y < 40;y++){
		positions[y] = malloc(sizeof(char) * 100);
		for(x = 0 ; x < 100; x++){
			positions[y][x] = mvinch(y,x);
		}
	}
	return positions;
}
void revealRoom(Level *level, Room *room){
	for(int y = room->position.y ;y < room->position.y + room->height; y++){
		for(int x = room->position.x ; x < room->position.x + room->width; x++){
			level->visited[y][x] = 1;
		}
	}
}
void check_next_step(Position * newPosition, Level * level) {
	Player * user;
	user = level->user;
    switch (level->tile[newPosition->y][newPosition->x])
	{
	case '.':
	case '#':
	case '+':
		playerMove(newPosition, user,level);
		moveMonsters(level);
		break;
	case 'm':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
		level->tile[newPosition->y][newPosition->x] = '.';
	case 'v':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
		level->tile[newPosition->y][newPosition->x] = '.';
	case 'd':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
		level->tile[newPosition->y][newPosition->x] = '.';
	case 'a':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
		level->tile[newPosition->y][newPosition->x] = '.';
	case 's':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
		level->tile[newPosition->y][newPosition->x] = '.';
	case 'h':
	case 'p':
	case 'g':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
		level->tile[newPosition->y][newPosition->x] = '.';
		break;
	case 'f':
	    level->comment = "attention: You can reduce hunger and increase your health by eating food!!!";
		playerMove(newPosition, user,level);
		moveMonsters(level);
		level->tile[newPosition->y][newPosition->x] = '.';
		
		printGameHub(level);
		break;
	 case '>':
	    level->comment = "attention: You can go to next level!!!";
		printGameHub(level);
	    playerMove(newPosition, user,level);
		moveMonsters(level);
		
		clear();
		mvprintw(18,18,"Do you want to go to next level?");
		mvprintw(19,18,"1. YES");
		mvprintw(20,18,"2. NO");
		int choice;

		echo();
        scanw("%d", &choice);
        noecho();

		switch (choice)
		{
		case 1:
		    int current_level =  level->level;
		    current_level++;
		    Player *new_user = level->user; 
		    clear();
		    free(level);
		    level = createLevel(current_level);
		
	        level->user->level = current_level;
	        level->user->health = new_user->health;
	        level->user->attack = new_user->attack;
	        level->user->gold = new_user->gold;
	        level->user->exp = new_user->exp;
	        level->user->maxHealth = 100;
			level->user->hunger = new_user->hunger;
			level->user->excellent_food = new_user->excellent_food;
			level->user->normal_food = new_user->normal_food;
			level->user->magical_food = new_user->magical_food;
			level->user->spoiled_food = new_user->spoiled_food;
	        
			printGameHub(level);
			break;
		case 2:
		    break;
		default:
			break;
		}
	 	
	 	break;

	case '=':
	    const wchar_t symbol[] = L"\U0001F911";
        mvaddnwstr(1, 0, symbol,-1);
	    mvprintw(1,2,"attention: you can see another room if ...");
	    playerMove(newPosition, user,level);
		moveMonsters(level);
	    for(int i = 0 ; i < 6; i++){
	       if((newPosition->x >= level->rooms[i]->position.x )&& (newPosition->x < level->rooms[i]->position.x + level->rooms[i]->width)){
			   revealRoom(level,level->rooms[i]);
		   }
		}
		drawLevel(level);
	    break;
	case 'I':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
	    //level->user->hunger += 5;
		
		if(level->user->spoiled_food + level->user->magical_food + level->user->normal_food + level->user->excellent_food < 5){ 
		    level->tile[newPosition->y][newPosition->x] = '.';
			level->user->spoiled_food += 1;}
		break;
	case 'N':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
	    //level->user->hunger -= 10;
		
		if(level->user->spoiled_food + level->user->magical_food + level->user->normal_food + level->user->excellent_food < 5){ 
		    level->tile[newPosition->y][newPosition->x] = '.';
			level->user->normal_food += 1;}
		break;
	case 'E':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
	    //level->user->hunger -= 20;
		
		if(level->user->spoiled_food + level->user->magical_food + level->user->normal_food + level->user->excellent_food < 5){ 
		    level->tile[newPosition->y][newPosition->x] = '.';
			level->user->excellent_food += 1;}
		break;
	case 'M':
	    playerMove(newPosition, user,level);
		moveMonsters(level);
	    //level->user->hunger -= 30;
		
		if(level->user->spoiled_food + level->user->magical_food + level->user->normal_food + level->user->excellent_food < 5){ 
		    level->tile[newPosition->y][newPosition->x] = '.';
			level->user->magical_food += 1;}
		break;
	case 'X':
		playerMove(newPosition, user,level);
		moveMonsters(level);
		level->tile[newPosition->y][newPosition->x] = '.';
		int random_gold = rand() % 30;
		level->user->gold += random_gold;
		level->comment = "attention: You collect GOLD!!!";
		printGameHub(level);
		break;
	default:
		break;
	}
	if(level->tile[newPosition->y][newPosition->x] == '#' || level->tile[newPosition->y][newPosition->x] == '+'){
		if(level->tile[newPosition->y][newPosition->x + 1] == '#'){
			level->visited[newPosition->y][newPosition->x + 1] = 1;
		}
		if(level->tile[newPosition->y][newPosition->x - 1] == '#'){
			level->visited[newPosition->y][newPosition->x - 1] = 1;
		}
		if(level->tile[newPosition->y][newPosition->x + 2] == '#'){
			level->visited[newPosition->y][newPosition->x + 2] = 1;
		}
		if(level->tile[newPosition->y][newPosition->x - 2] == '#'){
			level->visited[newPosition->y][newPosition->x - 2] = 1;
		}
		if(level->tile[newPosition->y + 1][newPosition->x] == '#'){
			level->visited[newPosition->y + 1][newPosition->x] = 1;
		}
		if(level->tile[newPosition->y - 1][newPosition->x] == '#'){
			level->visited[newPosition->y -1][newPosition->x] = 1;
		}
		if(level->tile[newPosition->y + 2][newPosition->x] == '#'){
			level->visited[newPosition->y + 2][newPosition->x] = 1;
		}
		if(level->tile[newPosition->y - 2][newPosition->x] == '#'){
			level->visited[newPosition->y -2][newPosition->x] = 1;
		}
		drawLevel(level);
	}
	for(int i = 0 ; i < 6; i++){
		Room *room = level->rooms[i];
		if(newPosition->x >= room->position.x && newPosition->x < room->position.x + room->width && newPosition->y >= room->position.y && newPosition->y < room->position.y + room->height){
			revealRoom(level,room);
			drawLevel(level);
			break;
		}
	}
	if((newPosition->x == level->traps.x) && (newPosition->y == level->traps.y)){
		if(level->trap_revealed == 0){
			level->trap_revealed = 1;
		}
		level->comment = "attention: You stepped on a trap!!! -5 health";
		printGameHub(level);
		level->user->health -= 5;
		level->tile[newPosition->y][newPosition->x] = '.';
		level->comment = " ";
		printGameHub(level);
	}
}
void printGameHub(Level * level){
	setlocale(LC_ALL, "");

	const wchar_t symbol[] = L"\U0001F61C";
    mvaddnwstr(1, 0, symbol,-1);
	mvprintw(1,4,"%s",level->comment);
	mvprintw(40,0,"____________________________________________________________________");
	mvprintw(33,0,"____________________________________________________________________");
	const wchar_t symbol0[] = L"\U0001F380";
    mvaddnwstr(34, 1, symbol0,-1);
	mvprintw(34,4, "Level: %d | Health: %d/%d | Gold: %d | Experience: %d | Hunger: %d ", level->level, level->user->health,level->user->maxHealth,level->user->gold, level->user->exp, level->user->hunger);
	
	const wchar_t symbol1[] = L"\U000026D4";
    mvaddnwstr(35, 1, symbol1,-1);

	const wchar_t symbol2[] = L"\U0001FAA7";
    mvaddnwstr(36, 1, symbol2,-1);

	const wchar_t symbol3[] = L"\U0001FA93";
    mvaddnwstr(37, 1, symbol3,-1);

	const wchar_t symbol4[] = L"\U0001F354";
    mvaddnwstr(38, 1, symbol4,-1);

	mvprintw(35,4,"%s", "*press Enter to quit the game*");
	mvprintw(36,4,"%s", "*press 'n' to see all the map*");
	mvprintw(37,4,"%s", "*press 'i' to view inventory*");
	mvprintw(38,4,"%s", "*press 'E' to view menu of food*");
	mvprintw(39,4,"%s", "*press 'N' to STOP music:/*");
	if(level->trap_revealed){
		mvprintw(level->traps.y,level->traps.x,"V");
	}
}
void placeTraps(Level * level){
	int room_index = rand() % 6;
	Room * room = level->rooms[room_index];

	level->traps.x = (rand() % (room->width - 2)) + room->position.x + 1;
	level->traps.y = (rand() % (room->height - 2)) + room->position.y + 1;
	level->trap_revealed = 0;
}
int gameLoop(){
    time_t last_hunger_update = time(NULL);
    time_t last_health_decrease = time(NULL);

    int ch;
    Position * newPosition;
    Level * level1;
    level1 = createLevel(1);
    printGameHub(level1);
    while ((ch = getch()) != '\n')
    {
        printGameHub(level1);
        newPosition = handleInput(ch, level1->user,level1);
        check_next_step(newPosition, level1);
        move(level1->user->position->y,level1->user->position->x);
        
        if (level1->user->health <= 0)
        {
            clear();
            mvprintw(14,14,"END THE GAME...");
            return -1;
        }

        time_t current_time = time(NULL);
        if (difftime(current_time, last_hunger_update) >= 10) {
        level1->user->hunger += 10;
        if (level1->user->hunger >= 100) {
            level1->user->health -= 10;
            level1->user->hunger = 0; 
        }
        last_hunger_update = current_time;

        if (difftime(current_time, last_health_decrease) >= 5) {
        for (int i = 0; i < level1->numberOfMonsters; i++) {
            Monster *monster = level1->monsters[i];
            int distance = abs(level1->user->position->x - monster->position->x) + abs(level1->user->position->y - monster->position->y);
            if (distance <= 2 && monster->alive) { 
                level1->user->health -= 5; 
                break;
            }
        }
        last_health_decrease = current_time;
    }
    }
    }
}


