#include <stdlib.h>
#include <ncurses.h>
#include <time.h>


typedef struct {
    int x, y, height, width;
} Room;
typedef struct {
    int x, y , parent_x, parent_y;
}QueueNode;

Room rooms[7];
int room_count = 0;
char map[30][50];

void init_map() {
    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < 50; x++) {
            map[y][x] = ' ';
        }
    }
}
int create_room() {
    int width = 5 + rand() % 5;
    int height = 5 + rand() % 5;
    int x = rand() % (50 - width - 1) + 1;
    int y = rand() % (30 - height - 1) + 1;

for (int i = 0; i < room_count; i++) {
if (x + width >= rooms[i].x && x <= rooms[i].x + rooms[i].width &&
y + height >= rooms[i].y && y <= rooms[i].y + rooms[i].height) {
return 0;
}
}

for (int i = x + 1; i < x + width - 1; i++) {
map[y][i] = '_';
map[y + height - 1][i] = '_';
}
for (int i = y + 1; i < y + height; i++) {
map[i][x] = '|';
map[i][x + width - 1] = '|';
}
for (int i = y; i < y + height; i++) {
for (int j = x; j < x + width; j++) {
if (i != y && i != y + height - 1 && j != x && j != x + width - 1) {
map[i][j] = '.';
}
}
}

rooms[room_count++] = (Room){x, y, height, width};
return 1;
}
void add_door(Room *room, int is_first_or_last) {
    int door_count = is_first_or_last ? 1 : 2;
    int added_doors = 0;

    while (added_doors < door_count) {
        int side = rand() % 4;
        int x, y;
        switch (side) {
            case 0:
                x = room->x + rand() % room->width;
                y = room->y;
                break;
            case 1:
                x = room->x + rand() % room->width;
                y = room->y + room->height - 1;
                break;
            case 2:
                x = room->x;
                y = room->y + rand() % room->height;
                break;
            case 3:
                x = room->x + room->width - 1;
                y = room->y + rand() % room->height;
                break;
        }
        if (map[y][x] == '|' || map[y][x] == '_') {
            map[y][x] = '+';
            added_doors++;
        }
    }
}
void connect_rooms(Room a, Room b) {
    int x_a, y_a, x_b, y_b;

    for (int i = a.y; i < a.y + a.height; i++) {
        for (int j = a.x; j < a.x + a.width; j++) {
            if (map[i][j] == '+') {
                x_a = j;
                y_a = i;
                break;
            }
        }
    }

    for (int i = b.y; i < b.y + b.height; i++) {
        for (int j = b.x; j < b.x + b.width; j++) {
            if (map[i][j] == '+') {
                x_b = j;
                y_b = i;
                break;
            }
        }
    }

    while (x_a != x_b || y_a != y_b) {
        if (x_a != x_b) {
            int next_x = x_a + (x_b > x_a ? 1 : -1);
            if (map[y_a][next_x] == ' ' || map[y_a][next_x] == '+') {
                x_a = next_x;
            } else {
                if (y_a != y_b) {
                    y_a += (y_b > y_a ? 1 : -1);
                } else {
                    break;
                }
            }
        } else if (y_a != y_b) {
            int next_y = y_a + (y_b > y_a ? 1 : -1);
            if (map[next_y][x_a] == ' ' || map[next_y][x_a] == '+') {
                y_a = next_y;
            } else {
                if (x_a != x_b) {
                    x_a += (x_b > x_a ? 1 : -1);
                } else {
                    break;
                }
            }
        }

        if (map[y_a][x_a] == ' ') {
            map[y_a][x_a] = '#';
        }
    }
}
void generate_map() {
    init_map();
    for (int i = 0; i < 7; i++) {
        if (!create_room()) i--;
    }
    for (int i = 0; i < room_count; i++) {
        add_door(&rooms[i], (i == 0 || i == room_count - 1));
        if (i > 0) connect_rooms(rooms[i - 1], rooms[i]);
    }
    map[rooms[0].y + 1][rooms[0].x + 1] = 'S';
    map[rooms[room_count - 1].y + 1][rooms[room_count - 1].x + 1] = 'E';
}

void display_map() {
    clear();
    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < 50; x++) {
            mvaddch(y, x, map[y][x]);
        }
    }
    refresh();
}