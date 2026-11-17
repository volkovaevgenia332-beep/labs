#ifndef MAP_H
#define MAP_H

#include "game.h"
#include "player.h"

// Структура карты
typedef struct {
    char **grid;       // Двумерный динамический массив
    int width;
    int height;
    int floor;         // Номер этажа/локации (для награды)
    int hero_x, hero_y;
    int camp_x, camp_y;
    int portal_active;
    int portal_x, portal_y;
    int edge_transitions;
} Map;

// Результат шага
typedef struct {
    int x;
    int y;
    int crossed_edge;
    int valid;
} MoveResult;

// Функции
Map* map_generate(int floor);
void map_destroy(Map *m);
void map_display(const Map *m);
MoveResult map_calc_next_position(int hero_x, int hero_y, int width, int height, char move);
void action_adventure(Player *p, GameTime *t, const char item_names[MAX_ITEMS][MAX_NAME_LEN]);
#endif // MAP_H