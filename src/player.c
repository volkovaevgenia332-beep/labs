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

    // Выдаём стартовый инвентарь (немного еды и ресурсов)
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
    
    // временный вывод инвентаря прямо здесь (потом перенесу в player)
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int id = p->inventory[i];
        printf("  [%d] %s\n", i, item_names[id]);
    }
    printf("===================================\n\n");
}

// Обновление голода (вызывается при ходьбе и течении времени)
void player_update_hunger(Player *p, int hours_passed, int steps_passed) {
    if (hours_passed > 0) {
        p->hunger -= hours_passed * HUNGER_PER_HOUR;
    }
    if (steps_passed >= 5) {
        p->hunger -= (steps_passed / 5) * HUNGER_PER_5_STEPS;
    }

    if (p->hunger < 0) p->hunger = 0;

    // Если голод ниже 20, герой теряет HP при ходьбе
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
        
        p->inventory[slot] = ITEM_EMPTY; // Предмет исчезает
    } else {
        printf("В этом слоте не еда!\n");
    }
}
