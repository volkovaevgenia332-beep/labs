#include "player.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Заполняем базу еды (по ID предметов)
Food food_db[MAX_ITEMS] = {
    [ITEM_BREAD] = {ITEM_BREAD, 30, 0},
    [ITEM_APPLE] = {ITEM_APPLE, 15, 5},
    [ITEM_SOUP]  = {ITEM_SOUP,  50, 10}
};

// Создание персонажа (выделение памяти)
Player* player_create(void) {
    Player *p = (Player*)malloc(sizeof(Player));
    if (p == NULL) {
        printf("[Ошибка]: Не удалось выделить память под персонажа.\n");
        return NULL;
    }

    printf("Введите имя вашего персонажа: ");
    get_safe_string(p->name, MAX_NAME_LEN);

    p->hp = START_HP;
    p->max_hp = MAX_HP;
    p->stamina = START_STAMINA;
    p->max_stamina = MAX_STAMINA;
    p->gold = START_GOLD;
    p->hunger = START_HUNGER;
    p->x = 0;
    p->y = 0;

    // Выдаём стартовый инвентарь
    int start_inv[INVENTORY_SIZE] = {
        ITEM_WOOD, ITEM_STONE, ITEM_BREAD, ITEM_APPLE, ITEM_SEEDS,
        ITEM_BREAD, ITEM_SOUP, ITEM_IRON, ITEM_POTION, ITEM_EMPTY
    };
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        p->inventory[i] = start_inv[i];
    }

    printf("[Успех]: Персонаж '%s' создан по адресу %p\n", p->name, (void*)p);
    return p;
}

// Удаление персонажа (очистка памяти с выводом адреса)
void player_destroy(Player *p) {
    if (p != NULL) {
        printf("Память структуры по адресу %p освобождена\n", (void*)p);
        free(p);
    }
}

// Красивая карточка персонажа
void player_print_card(const Player *p, GameTime t, const char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("\n===================================\n");
    printf("        КАРТОЧКА ПЕРСОНАЖА\n");
    printf("===================================\n");
    printf(" Имя: %s\n", p->name);
    printf(" HP: %d/%d\n", p->hp, p->max_hp);
    printf(" Выносливость: %d/%d\n", p->stamina, p->max_stamina);
    printf(" Сытость: %d/%d\n", p->hunger, MAX_HUNGER);
    printf(" Золото: %d\n", p->gold);
    printf(" Время: День %d, %02d:00\n", t.day, t.hour);
    printf("--- ИНВЕНТАРЬ ---\n");
    
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int id = p->inventory[i];
        printf("  [%d] %s\n", i, item_names[id]);
    }
    printf("===================================\n\n");
}

// Обновление голода
void player_update_hunger(Player *p, int hours_passed, int steps_passed) {
    if (hours_passed > 0) {
        p->hunger -= hours_passed * HUNGER_PER_HOUR;
    }
    if (steps_passed >= 5) {
        p->hunger -= (steps_passed / 5) * HUNGER_PER_5_STEPS;
    }

    if (p->hunger < 0) p->hunger = 0;

    if (p->hunger < HUNGER_LOW && steps_passed > 0) {
        p->hp -= steps_passed * STARVE_HP_LOSS;
        if (p->hp < 0) p->hp = 0;
        printf("[ВНИМАНИЕ]: Вы голодаете! Потеряно %d HP.\n", steps_passed * STARVE_HP_LOSS);
    }
}

// Логика поедания еды
void player_eat(Player *p, const char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("\n--- ВАШ РАЦИОН ---\n");
    int has_food = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int id = p->inventory[i];
        if (id == ITEM_BREAD || id == ITEM_APPLE || id == ITEM_SOUP) {
            printf("  Слот %d: %s\n", i, item_names[id]);
            has_food = 1;
        }
    }
    
    if (!has_food) {
        printf("У вас нет еды в рюкзаке!\n");
        return;
    }

    printf("Введите индекс слота с едой для употребления: ");
    int slot = get_safe_int();
    
    if (slot < 0 || slot >= INVENTORY_SIZE) {
        printf("Ошибка: неверный слот.\n");
        return;
    }

    int id = p->inventory[slot];
    if (id == ITEM_BREAD || id == ITEM_APPLE || id == ITEM_SOUP) {
        p->hunger += food_db[id].hunger_restore;
        p->stamina += food_db[id].stamina_restore;
        
        if (p->hunger > MAX_HUNGER) p->hunger = MAX_HUNGER;
        if (p->stamina > p->max_stamina) p->stamina = p->max_stamina;

        printf("Вы съели %s! Восстановлено %d сытости и %d выносливости.\n", 
               item_names[id], food_db[id].hunger_restore, food_db[id].stamina_restore);
        
        p->inventory[slot] = ITEM_EMPTY; 
    } else {
        printf("В этом слоте не еда!\n");
    }
}

// --- ФУНКЦИИ ИНВЕНТАРЯ ---

void print_inventory(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf("======= СОДЕРЖИМОЕ ИНВЕНТАРЯ =======\n");
    for (int i = 0; i < size; i++) {
        int id = inv[i];
        if (id >= 0 && id < MAX_ITEMS) {
            printf("Слот %d: [%d] - %s\n" , i, id, names[id]);
        } else {
            printf("Слот %d: [%d] - (Неизвестный предмет)\n", i, id);
        }
    }
}

void action_put_item(int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf("Введите индекс слота (от 0 до %d): ", size - 1);
    int slot = get_safe_int();
    if (slot < 0 || slot >= size) { printf("Ошибка! Неверный индекс.\n"); return; }
    printf("Доступные предметы для добавления:\n");
    for (int i = 0; i < MAX_ITEMS; i++) printf("  [%d] - %s\n", i, names[i]);

    printf("Введите ID предмета: ");
    int id = get_safe_int();
    if (id >= 0 && id < MAX_ITEMS) {
        inv[slot] = id;
        printf("Предмет '%s' помещен в слот %d.\n", names[id], slot);
    } else {
        printf("Ошибка! Неверный ID.\n");
    }
}

void action_drop_item(int inv[], int size) {
    printf("Введите индекс слота для очистки: ");
    int slot = get_safe_int();
    if (slot >= 0 && slot < size) {
        inv[slot] = ITEM_EMPTY;
        printf("Слот %d очищен.\n", slot);
    } else {
        printf("Ошибка индекса.\n");
    }
}

void action_favorite_resource(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf("=== Поиск любимого ресурса ===\n");
    int counts[MAX_ITEMS] = {0};
    for (int i = 0; i < size; i++) {
        if (inv[i] != ITEM_EMPTY) counts[inv[i]]++;
    }
    int max_slots = 0, fav_id = -1; 
    for (int id = 1; id < MAX_ITEMS; id++) {
        if (counts[id] > max_slots) { max_slots = counts[id]; fav_id = id; }
    }
    if (fav_id == -1) printf("Рюкзак пуст.\n");
    else printf("Любимый ресурс: [%d] (%s) - %d шт.\n", fav_id, names[fav_id], max_slots);
}

void action_search_by_name(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    char search_name[MAX_NAME_LEN];
    printf("Введите название предмета: ");
    get_safe_string(search_name, MAX_NAME_LEN);

    int target_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(names[i], search_name) == 0) { target_id = i; break; }
    }

    if (target_id == -1 || target_id == ITEM_EMPTY) {
        printf("Предмет не найден в каталоге.\n");
        return;
    }

    int found = 0;
    printf("Предмет '%s' найден в слотах: ", search_name);
    for (int i = 0; i < size; i++) {
        if (inv[i] == target_id) { printf("%d ", i); found = 1; }
    }
    if (!found) printf("(отсутствует в инвентаре)\n");
    else printf("\n");
}