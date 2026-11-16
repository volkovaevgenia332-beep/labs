#ifndef UTILS_H
#define UTILS_H

#include "game.h"   // нужен для MAX_ITEMS, MAX_NAME_LEN, GameTime

// ===== Безопасный ввод =====
int  get_safe_int(void);
void get_safe_string(char *buffer, int max_len);

// ===== Каталог предметов =====
void load_item_catalogue(char names[MAX_ITEMS][MAX_NAME_LEN]);

// ===== Время =====
void action_show_time(GameTime t);
void action_work(GameTime *t);

// ===== Файлы =====
void action_save_to_diary(const char *farmer_name, GameTime t,
                          const int inv[],
                          const char names[MAX_ITEMS][MAX_NAME_LEN],
                          int size);

void action_check_recipes(const int inv[],
                          const char names[MAX_ITEMS][MAX_NAME_LEN],
                          int size);

#endif // UTILS_H