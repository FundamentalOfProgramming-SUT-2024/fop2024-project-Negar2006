#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include <wchar.h>

#include "monster.c"


Food normal_food = {"Normal Food", 'N', 10};
Food excellent_food = {"Excellent Food", 'E', 20};
Food spoiled_food = {"Spoiled Food", 'I', -5};  
Food magical_food = {"Magical Food", 'M', 30};

Weapon weapons[] = {
    {"Mace", 5, 0, -1, 0},
    {"Sward", 10, 0, -1, 0},
    {"Magic Wand", 15, 10, 8, 1}, 
    {"Normal Arrow", 5, 5, 20, 1},
    {"Dagger", 12, 5, 10, 1}
};


void placeFood(Level *level);
void displayFoodMenu(Player * player);
Food * selectFood();
void updateHunger(Player *player);
void drawHungerBar(Player *player);
PlayerInventory *createInventory();
void displayWeaponMenu(PlayerInventory *inventory, Player * player);
void printSpellRoom(Level * level) ;
void displaySpellMenu(Player * player);

void placeFood(Level *level) {
    int food_count = 4; 
    for (int i = 0; i < food_count; i++) {
        Room *room = level->rooms[rand() % 6];
        int x = rand() % (room->width - 2) + room->position.x + 1;
        int y = rand() % (room->height - 2) + room->position.y + 1;
        
        int food_type = rand() % 4;
        char food_symbol;
        switch (food_type) {
            case 0:
                food_symbol = normal_food.symbol;
                break;
            case 1:
                food_symbol = excellent_food.symbol;
                break;
            case 2:
                food_symbol = spoiled_food.symbol;
                break;
            case 3:
                food_symbol = magical_food.symbol;
                break;
        }
        level->tile[y][x] = food_symbol;
    }
}
void displayFoodMenu(Player * player) {
    clear();
    drawHungerBar(player);
    attron(COLOR_PAIR(1));
    mvprintw(4, 4, "Food Menu:");
    attroff(COLOR_PAIR(1));
    mvprintw(6, 4, "1. Normal Food =  %d", player->normal_food);
    mvprintw(7, 4, "2. Excellent Food = %d",player->excellent_food);
    mvprintw(8, 4, "3. Spoiled Food = %d",player->spoiled_food);
    mvprintw(9, 4, "4. Magical Food = %d",player->magical_food);
    refresh();
}
Food * selectFood() {
    int choice;
    printf("Select a food (1-4): ");
    scanf("%d", &choice);
    switch (choice) {
        case 1:
            return &normal_food;
        case 2:
            return &excellent_food;
        case 3:
            return &spoiled_food;
        case 4:
            return &magical_food;
        default:
            return NULL;
    }
}
void updateHunger(Player *player){
    player->hunger += 1;
    if (player->hunger >= 100) {
        player->health -= 10;
        player->hunger = 0;
    }
}
void drawHungerBar(Player *player){
    mvprintw(0,1,"Hunger bar: ");
    int bar_length = player->hunger;
    int filled_length = (player->hunger / 100.0) * bar_length;
    mvprintw(0, 11, "[");
    for (int i = 11; i < filled_length + 11; i++) {
        mvprintw(0, i + 1, "=");
    }
    mvprintw(0, bar_length + 12, "]");
}
void add_gold(Level * level){
    int random_room1 = rand() % 6;
    int x1 = rand() % (level->rooms[random_room1]->width - 2) + level->rooms[random_room1]->position.x + 1;
    int y1 = rand() % (level->rooms[random_room1]->height - 2) + level->rooms[random_room1]->position.y + 1;

    level->tile[y1][x1] = 'X';

    int random_room2 = rand() % 6;
    int x2 = rand() % (level->rooms[random_room2]->width - 2) + level->rooms[random_room2]->position.x + 1;
    int y2 = rand() % (level->rooms[random_room2]->height - 2) + level->rooms[random_room2]->position.y + 1;

    level->tile[y2][x2] = 'X';

    int random_room3 = rand() % 6;
    int x3 = rand() % (level->rooms[random_room3]->width - 2) + level->rooms[random_room3]->position.x + 1;
    int y3 = rand() % (level->rooms[random_room3]->height - 2) + level->rooms[random_room3]->position.y + 1;

    level->tile[y3][x3] = 'X';
}
//     Mace
//         Damage: 5 units
//         Special Feature: Close-range attacks; cannot be thrown. Damages adjacent houses in all 8 directions simultaneously.
//         Collection: Default weapon available from the start; not found on the ground.
//     Dagger
//         Damage: 12 units
//         Special Feature: Can be thrown up to 5 units; falls to the ground after impact. If it hits an enemy, it reduces their life by 12 units.
//         Available Count: 10 per collection.
//     Magic Wand
//         Damage: 15 units
//         Special Feature: Can be thrown like a dagger but up to a maximum distance of 10 units. If it hits an enemy, it loses its movement early.
//         Available Shots: 8 shots.
//     Normal Arrow
//         Damage: 5 units
//         Special Feature: Simple arrow thrown up to 5 units; damages houses in all 8 directions if used as a close-range weapon.
//         Available Count: 20 units
//     Sword
//         Damage: 10 units
//         Special Feature: every units in 8 direction damage in the same time 
//         Available Count: Unlimited after first collection.

PlayerInventory *createInventory(){
    PlayerInventory *inventory = malloc(sizeof(PlayerInventory));
    for (int i = 0; i < 5; i++) {
        inventory->weapons[i] = &weapons[i];
    }
    inventory->equippedWeaponIndex = 0;
    return inventory;
}

void displayWeaponMenu(PlayerInventory *inventory, Player * player){
    clear(); 

    mvprintw(1, 1, "=== Weapon Menu ===");
    mvprintw(3, 1, "Select a weapon:");
    
    for (int i = 0; i < 5; i++) {
        Weapon *weapon = inventory->weapons[i];
        if(i == 0){
        mvprintw(6 + i, 1, "%d: %s (Damage: %d, Range: %d): %d",i + 1, weapon->name, weapon->damage, weapon->isThrowable ? weapon->maxDistance : 0, player->Mase);
        }
        if(i == 1){
        mvprintw(6 + i, 1, "%d: %s (Damage: %d, Range: %d): %d",i + 1, weapon->name, weapon->damage, weapon->isThrowable ? weapon->maxDistance : 0, player->Sward);
        }
        if(i == 2){
        mvprintw(9 + i, 1, "%d: %s (Damage: %d, Range: %d): %d",i + 1, weapon->name, weapon->damage, weapon->isThrowable ? weapon->maxDistance : 0, player->Magic_wand);
        }
        if(i == 3){
        mvprintw(9 + i, 1, "%d: %s (Damage: %d, Range: %d): %d",i + 1, weapon->name, weapon->damage, weapon->isThrowable ? weapon->maxDistance : 0, player->Normal_arrow);
        }
        if(i == 4){
        mvprintw(9 + i, 1, "%d: %s (Damage: %d, Range: %d): %d",i + 1, weapon->name, weapon->damage, weapon->isThrowable ? weapon->maxDistance : 0, player->Dagger);
        }
    }
    mvprintw(4,1,"Short range weapons:");
    mvprintw(9,1,"long range weapons:");
    mvprintw(14,1,"%s", player->main_weapon);

    mvprintw(17, 1, "Press ENTER to return.");
}
void displaySpellMenu( Player * player){
    clear(); 
    
    clear();
    attron(COLOR_PAIR(1));

    mvprintw(4, 4, "=== Spell Menu ===");
    mvprintw(5, 4, "Select a Spell:");

    attroff(COLOR_PAIR(1));
    mvprintw(7, 4, "1. HEALTH spell =  %d", player->HEALTH);
    mvprintw(8, 4, "2. SPEED spell = %d",player->SPEED);
    mvprintw(9, 4, "3. DAMAGE spell = %d",player->DAMAGE);
    refresh();
}

void Kill_monster(Level * level){
    if(strcmp(level->user->main_weapon, "Mase") == 0){   
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            int main_x = abs(x2 - x1);
            int main_y = abs(y2 - y1);
            if((main_x + main_y == 1) || (main_x == 1 && main_y == 1)){
                level->monsters[i]->health -= 5;
                refresh();
            }
        }
    }else if(strcmp(level->user->main_weapon,"Sward")){
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            int main_x = abs(x2 - x1);
            int main_y = abs(y2 - y1);
            if((main_x + main_y == 1) || (main_x == 1 && main_y == 1)){
                level->monsters[i]->health -= 10;
                refresh();
            }
        }
    }else if(strcmp(level->user->main_weapon, "Dagger") == 0){

        int direction;
        direction = getch() - '0';
        switch (direction){
        case 8:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((x1 == x1) && ((y2 - y1) <= 5)){
                level->monsters[i]->health -= 12;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x] = 'd';
            }
        }
		break;
	    case 2:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((x1 == x1) && ((y1 - y2) <= 5)){
                level->monsters[i]->health -= 12;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x] = 'd';
            }
        }
		break;
	    case 4:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 == y1) && ((x2 - x1) <= 5)){
                level->monsters[i]->health -= 12;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y][level->user->position->x - 5] = 'd';
            }
        }
		break;
	    case 6:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 == y1) && ((x1 - x2) <= 5)){
                level->monsters[i]->health -= 12;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y][level->user->position->x + 5] = 'd';
            }
        }
		break;
	    case 1:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 - y2) <= 5 && ((x2 - x1) <= 5)){
                level->monsters[i]->health -= 12;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x - 5] = 'd';
            }
        }
		break;
	    case 3:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 - y2) <= 5 && ((x1 - x2) <= 5)){
                level->monsters[i]->health -= 12;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x + 5] = 'd';
            }
        }
		break;
	    case 7:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y2 - y1) <= 5 && ((x2 - x1) <= 5)){
                level->monsters[i]->health -= 12;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x - 5] = 'd';
            }
        }
		break;
	    case 9:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y2 - y1) <= 5 && ((x1 - x2) <= 5)){
                level->monsters[i]->health -= 12;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x + 5] = 'd';
            }
        }
		break;
        default:
        break;
        }
    }else if(strcmp(level->user->main_weapon, "Normal_arrow") == 0){

        int direction;
        direction = getch() - '0';
        switch (direction){
        case 8:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((x1 == x1) && ((y2 - y1) <= 5)){
                level->monsters[i]->health -= 5;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x] = 'a';
            }
        }
		break;
	    case 2:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((x1 == x1) && ((y1 - y2) <= 5)){
                level->monsters[i]->health -= 5;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x] = 'a';
            }
        }
		break;
	    case 4:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 == y1) && ((x2 - x1) <= 5)){
                level->monsters[i]->health -= 5;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y][level->user->position->x - 5] = 'a';
            }
        }
		break;
	    case 6:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 == y1) && ((x1 - x2) <= 5)){
                level->monsters[i]->health -= 5;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y][level->user->position->x + 5] = 'a';
            }
        }
		break;
	    case 1:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 - y2) <= 5 && ((x2 - x1) <= 5)){
                level->monsters[i]->health -= 5;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x - 5] = 'a';
            }
        }
		break;
	    case 3:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 - y2) <= 5 && ((x1 - x2) <= 5)){
                level->monsters[i]->health -= 5;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x + 5] = 'a';
            }
        }
		break;
	    case 7:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y2 - y1) <= 5 && ((x2 - x1) <= 5)){
                level->monsters[i]->health -= 5;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x - 5] = 'a';
            }
        }
		break;
	    case 9:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y2 - y1) <= 5 && ((x1 - x2) <= 5)){
                level->monsters[i]->health -= 5;
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x + 5] = 'a';
            }
        }
		break;
        default:
        break;
        }
    }else if(strcmp(level->user->main_weapon, "Magic_wand") == 0){

        int direction;
        direction = getch() - '0';
        switch (direction){
        case 8:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((x1 == x1) && ((y2 - y1) <= 10)){
                level->monsters[i]->health -= 15;
                level->monsters[i]->pathfinding = 0;
                moveMonsters(level);
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x] = 'v';
            }
        }
		break;
	    case 2:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((x1 == x1) && ((y1 - y2) <= 10)){
                level->monsters[i]->health -= 15;
                level->monsters[i]->pathfinding = 0;
                moveMonsters(level);
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x] = 'v';
            }
        }
		break;
	    case 4:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 == y1) && ((x2 - x1) <= 10)){
                level->monsters[i]->health -= 15;
                level->monsters[i]->pathfinding = 0;
                moveMonsters(level);
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y][level->user->position->x - 5] = 'v';
            }
        }
		break;
	    case 6:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 == y1) && ((x1 - x2) <= 10)){
                level->monsters[i]->health -= 15;
                level->monsters[i]->pathfinding = 0;
                moveMonsters(level);
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y][level->user->position->x + 5] = 'v';
            }
        }
		break;
	    case 1:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 - y2) <= 10 && ((x2 - x1) <= 10)){
                level->monsters[i]->health -= 15;
                level->monsters[i]->pathfinding = 0;
                moveMonsters(level);
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x - 5] = 'v';
            }
        }
		break;
	    case 3:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y1 - y2) <= 10 && ((x1 - x2) <= 10)){
                level->monsters[i]->health -= 15;
                level->monsters[i]->pathfinding = 0;
                moveMonsters(level);
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y + 5][level->user->position->x + 5] = 'v';
            }
        }
		break;
	    case 7:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y2 - y1) <= 10 && ((x2 - x1) <= 10)){
                level->monsters[i]->health -= 15;
                level->monsters[i]->pathfinding = 0;
                moveMonsters(level);
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x - 5] = 'v';
            }
        }
		break;
	    case 9:
        for(int i = 0 ; i < level->numberOfMonsters;i++){
            int x1 = level->monsters[i]->position->x;
            int y1 = level->monsters[i]->position->y;

            int x2 = level->user->position->x;
            int y2 = level->user->position->y;

            if((y2 - y1) <= 10 && ((x1 - x2) <= 10)){
                level->monsters[i]->health -= 15;
                level->monsters[i]->pathfinding = 0;
                moveMonsters(level);
                level->user->main_weapon =" ";
            }else{
                level->user->main_weapon = " ";
                level->tile[level->user->position->y - 5][level->user->position->x + 5] = 'v';
            }
        }
		break;
        default:
        break;
        }
    }

}

void printSpellRoom(Level * level) {
    attron(COLOR_PAIR(2)); 

    for (int x = level->rooms[6]->position.x; x < level->rooms[6]->position.x + level->rooms[6]->width; x++) {
        mvprintw(level->rooms[6]->position.y, x, "_"); 
        mvprintw(level->rooms[6]->position.y + level->rooms[6]->height - 1, x, "_"); 
    }
    
    for (int y = level->rooms[6]->position.y + 1; y < level->rooms[6]->position.y + level->rooms[6]->height - 1; y++) {
        mvprintw(y, level->rooms[6]->position.x, "|"); 
        mvprintw(y, level->rooms[6]->position.x + level->rooms[6]->width - 1, "|"); 

        for (int x = level->rooms[6]->position.x + 1; x < level->rooms[6]->position.x + level->rooms[6]->width - 1; x++) {
            mvprintw(y, x, "."); 
        }
    }
    mvprintw(level->rooms[6]->cols[0]->y, level->rooms[6]->cols[0]->x, "O"); 
    mvprintw(level->rooms[6]->cols[1]->y, level->rooms[6]->cols[1]->x, "O"); 

    attroff(COLOR_PAIR(2));
}



