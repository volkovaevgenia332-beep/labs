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
    
    
    int attack;     // Сила удара
    int defense;    // Защита
    int crit_chance;  // Шанс крита (для варианта)
    
    int x, y;
    int inventory[INVENTORY_SIZE];
} Player;


extern Food food_db[MAX_ITEMS];

Player* player_create(void);
void player_destroy(Player *p);
void player_print_card(const Player *p, GameTime t, const char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void player_update_hunger(Player *p, int hours_passed, int steps_passed);
void player_eat(Player *p, const char item_names[MAX_ITEMS][MAX_NAME_LEN]);

// Функции инвентаря
void print_inventory(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size);
void action_put_item(int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size);
void action_drop_item(int inv[], int size);
void action_favorite_resource(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size);
void action_search_by_name(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size);

#endif // PLAYER_H