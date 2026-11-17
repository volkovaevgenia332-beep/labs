#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

typedef struct {
    int id;
    int hunger_restore;
    int stamina_restore;
} Food;

typedef struct {
    char name[MAX_NAME_LEN];
    int hp;
    int max_hp;
    int stamina;
    int max_stamina;
    int gold;
    int hunger;
    int x, y;
    int inventory[INVENTORY_SIZE];
} Player;

extern Food food_db[MAX_ITEMS];

Player* player_create(void);
void player_destroy(Player *p);
void player_print_card(const Player *p, GameTime t, const char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void player_update_hunger(Player *p, int hours_passed, int steps_passed);
void player_eat(Player *p, const char item_names[MAX_ITEMS][MAX_NAME_LEN]);

#endif // PLAYER_H

void print_inventory(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size);