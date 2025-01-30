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
    {"Dagger", 12, 5, 10, 1},
    {"Magic Wand", 15, 10, 8, 1}, 
    {"Normal Arrow", 5, 5, 20, 1}, 
    {"Sward", 10, 0, -1, 0}
};


void placeFood(Level *level);
void displayFoodMenu(Player * player);
Food * selectFood();
void updateHunger(Player *player);
void drawHungerBar(Player *player);
PlayerInventory *createInventory();
void displayWeaponMenu(PlayerInventory *inventory);
void useWeapon(Player *user, PlayerInventory *inventory);

void placeFood(Level *level) {
    int food_count = 4; 
    for (int i = 0; i < food_count; i++) {
        Room *room = level->rooms[rand() % level->numberOfRooms];
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

void displayWeaponMenu(PlayerInventory *inventory){
    clear(); 

    mvprintw(1, 1, "=== Weapon Menu ===");
    mvprintw(3, 1, "Select a weapon:");
    
    for (int i = 0; i < 5; i++) {
        Weapon *weapon = inventory->weapons[i];
        if (weapon->count >= 0) { 
            mvprintw(5 + i, 1, "%d: %s (Damage: %d, Range: %d)", 
                     i, weapon->name, weapon->damage, 
                     weapon->isThrowable ? weapon->maxDistance : 0);
        }
    }

    mvprintw(11, 1, "Press 'q' to return.");
    refresh(); 

    int choice;
    while ((choice = getch()) != 'q') {
        if (choice >= '0' && choice < '0' + 5) {
            int index = choice - '0';
            if (inventory->weapons[index]->count > 0 || index == inventory->equippedWeaponIndex) {
                inventory->equippedWeaponIndex = index;
                mvprintw(12, 1, "Equipped %s as default weapon.", inventory->weapons[index]->name);
            } else {
                mvprintw(12, 1, "You need to put your current weapon in the backpack first.");
            }
        } else {
            mvprintw(12, 1, "Invalid selection. Try again.");
        }
        refresh(); 
    }
    
    clear();
}

void useWeapon(Player *user, PlayerInventory *inventory){
    Weapon *currentWeapon = inventory->weapons[inventory->equippedWeaponIndex];
    
    if (currentWeapon->isThrowable) {
        if (currentWeapon->count > 0) {
            printf("Throwing %s...\n", currentWeapon->name);
            currentWeapon->count--;
            // Logic for hitting an enemy or wall goes here...
        } else {
            printf("No more %s left!\n", currentWeapon->name);
        }
    } else {
        printf("Using %s in close range...\n", currentWeapon->name);
        // Logic for close-range attack goes here...
    }
}


