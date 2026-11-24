#ifndef MAP_H
#define MAP_H

#include "game.h"
#include "player.h"
#include "entity.h" // Подключаем новый модуль

typedef struct {
    char **grid;       // Сетка символов (нужна для быстрой отрисовки)
    int width, height;
    int floor;
    int hero_x, hero_y;
    int camp_x, camp_y;
    int portal_active, portal_x, portal_y;
    int edge_transitions;

    // НОВОЕ: Массив объектов на этаже
    Entity *entities;
    int entity_count;
} Map;

typedef struct {
    int x, y, crossed_edge, valid;
} MoveResult;

Map* map_generate(int floor);
void map_destroy(Map *m);
void map_display(const Map *m);
MoveResult map_calc_next_position(int hero_x, int hero_y, int width, int height, char move);
void action_adventure(Player *p, GameTime *t, const char item_names[MAX_ITEMS][MAX_NAME_LEN]);

#endif // MAP_H