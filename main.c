#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#include "src/game.h"
#include "src/utils.h"

//ФУНКЦИИ РАБОТЫ С КАТАЛОГОМ И ИНВЕНТАРЕМ

// показать текущий инвентарь
void print_inventory (const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf ("======= СОДЕРЖИМОЕ ИНВЕНТАРЯ =======\n");
    for (int i = 0; i < size; i++) {
        int id = inv[i];
        if (id >= 0 && id < MAX_ITEMS) {
            printf("Слот %d: [%d] - %s\n" , i, id, names[id]);
        } else {
            printf("Слот %d: [%d] - (Неизвестный предмет)\n", i, id);
        }
    }
}

// положить предмет в слот
void action_put_item(int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf("Введите индекс слота (от 0 до %d): ", size - 1);
    int slot = get_safe_int();
    if (slot < 0 || slot >= size) {
        printf("Ошибка! Неверный индекс. Допустимый диапазон: 0 - %d\n", size - 1);
        return;
    }
    printf("Доступные предметы для добавления:\n");
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("  [%d] - %s\n", i, names[i]);
    }

    printf("Введите ID предмета (0-9): ");
    int id = get_safe_int();

    if (id < 0 || id >= MAX_ITEMS) {
        printf("Ошибка! Предмета с таким ID не существует.\n");
        return;
    }
    
    inv[slot] = id;
    printf("Предмет '%s' успешно помещен в слот %d.\n", names[id], slot);
}

//выбросить предмет
void action_drop_item(int inv[], int size) {
    printf("Введите индекс слота для очистки (от 0 до %d): ", size - 1);
    int slot = get_safe_int();
    if (slot < 0 || slot >= size) {
        printf("Ошибка! Неверный индекс слота.\n");
        return;
    }
    inv[slot] = ITEM_EMPTY;
    printf("Слот %d очищен.\n", slot);
}

//любимый ресурс
void action_favorite_resource(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf("=== Поиск любимого ресурса ===\n");
    int counts[MAX_ITEMS] = {0};

    for (int i = 0; i < size; i++) {
        if (inv[i] != ITEM_EMPTY) {
            counts[inv[i]]++;
        }
    }

    int max_slots = 0; 
    int fav_id = -1; 
    for (int id = 1; id < MAX_ITEMS; id++) {
        if (counts[id] > max_slots) {
            max_slots = counts[id];
            fav_id = id;
        }
    }

    if (fav_id == -1 || max_slots == 0) {
        printf("Рюкзак пуст! Любимый предмет не найден.\n");
    } else {
        printf("Любимый ресурс имеет ID: [%d] (%s)\n", fav_id, names[fav_id]);
        printf("Количество занимаемых слотов: %d\n", max_slots);
    }
}

//поиск предмета в рюкзаке по названию
void action_search_by_name(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    char search_name[MAX_NAME_LEN];
    printf("Введите текстовое название предмета для поиска: ");
    get_safe_string(search_name, MAX_NAME_LEN);

    int target_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(names[i], search_name) == 0) {
            target_id = i;
            break;
        }
    }

    if (target_id == -1 || target_id == ITEM_EMPTY) {
        printf("Предмета с названием '%s' нет в каталоге предметов или этот слот пуст.\n", search_name);
        return;
    }

    int found = 0;
    printf("Предмет '%s' найден в следующих слотах: ", search_name);
    for (int i = 0; i < size; i++) {
        if (inv[i] == target_id) {
            printf("%d ", i);
            found = 1;
        }
    }
    
    if (!found) {
        printf("вы не носите этот предмет с собой (в инвентаре отсутствует).\n");
    } else {
        printf("\n");
    }
}

//функции для работы с динамической картой

//функции освобождения памяти двумерного массива
void free_map(char **map, int height) {
    if (map == NULL) return;
    for (int i = 0; i < height; i++) {
        if (map[i] != NULL) {
            free(map[i]);
        }
    }
    free(map);
}

//вывод динамической карты на экран
void display_map(char **map, int width, int height, int hero_x, int hero_y) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x== hero_x && y == hero_y) {
                printf("@ "); 
            } else {
                printf("%c ", map[y][x]);
            }
        }
        printf("\n");
    }
}

int wrap_forward(int coord, int size) {
    return (coord + 1) % size;
}
int wrap_backward(int coord, int size) {
    return (coord - 1 + size) % size;
}

typedef struct {
    int x;             
    int y;             
    int crossed_edge;  
    int valid;         
} MoveResult;

MoveResult calc_next_position(int hero_x, int hero_y, int width, int height, char move) {
    MoveResult res = { hero_x, hero_y, 0, 1 };

    switch (move) {
        case 'W':
            res.y = wrap_backward(hero_y, height);
            res.crossed_edge = (hero_y == 0);
            break;
        case 'S':
            res.y = wrap_forward(hero_y, height);
            res.crossed_edge = (hero_y == height - 1);
            break;
        case 'A':
            res.x = wrap_backward(hero_x, width);
            res.crossed_edge = (hero_x == 0);
            break;
        case 'D':
            res.x = wrap_forward(hero_x, width);
            res.crossed_edge = (hero_x == width - 1);
            break;
        default:
            res.valid = 0;
            break;
    }
    return res;
}

char** generate_forest_ring(int *width, int *height, int *hero_x, int *hero_y, int *camp_x, int *camp_y, int *portal_active, int *portal_x, int *portal_y)
{
    *width  = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    *height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

    *portal_active = 0;
    *portal_x = -1;
    *portal_y = -1;

    char **map = (char**)malloc((*height) * sizeof(char*));
    if (map == NULL) return NULL;

    for (int i = 0; i < *height; i++) {
        map[i] = (char*)malloc((*width) * sizeof(char));
        if (map[i] == NULL) {
            for (int k = 0; k < i; k++) free(map[k]);
            free(map);
            return NULL;
        }
    }

    int obstacle_chance = 15 + rand() % 16;
    for (int y = 0; y < *height; y++) {
        for (int x = 0; x < *width; x++) {
            if ((rand() % 100) < obstacle_chance) {
                map[y][x] = (rand() % 2 == 0) ? 'T' : 'R';
            } else {
                map[y][x] = '.';
            }
        }
    }

    *camp_x = rand() % (*width);
    *camp_y = rand() % (*height);
    map[*camp_y][*camp_x] = '^';

    do {
        *hero_x = rand() % (*width);
        *hero_y = rand() % (*height);
    } while (map[*hero_y][*hero_x] == 'T' || map[*hero_y][*hero_x] == 'R');

    return map;
}

void action_adventure() {
    int width, height;
    int hero_x, hero_y;
    int camp_x, camp_y;
    int portal_active = 0;
    int portal_x = -1, portal_y = -1;
    int edge_transitions = 0; 

    char **map = generate_forest_ring(&width, &height, &hero_x, &hero_y, &camp_x, &camp_y, &portal_active, &portal_x, &portal_y);
    if (map == NULL) {
        printf("[Ошибка]: Не удалось выделить память под карту.\n");
        return;
    }

    printf("[Приключение началось! Вы попали в Лес-кольцо %dx%d]\n", width, height);

    int in_adventure = 1;

    while (in_adventure) {
        printf("\nПереходов через край: %d | Портал: %s\n", edge_transitions, portal_active ? "ОТКРЫТ (P)" : "ЗАКРЫТ");
        display_map(map, width, height, hero_x, hero_y);
        printf("Управление (W - вверх, A - влево, S - вниз, D - вправо, Q - сбежать домой): ");

        char move = getchar();
        if (move != '\n' && move != EOF) {
            while (getchar() != '\n'); 
        }

        if (move >= 'a' && move <= 'z') move -= 32;

        if (move == 'Q') {
            if (hero_x == camp_x && hero_y == camp_y) {
                printf("Вы погрелись у костра и благополучно вернулись на ферму!\n");
                in_adventure = 0;
            } else {
                printf("Ошибка! Сбежать домой можно только находясь на клетке костра (^).\n");
            }
            continue;
        }

        MoveResult r = calc_next_position(hero_x, hero_y, width, height, move);

        if (!r.valid) {
            printf("Неверная команда! Используйте W, A, S, D или Q.\n");
            continue;
        }

        if (map[r.y][r.x] == 'T' || map[r.y][r.x] == 'R') {
            printf("Ой! Там непреодолимое препятствие (%c).\n", map[r.y][r.x]);
            continue;
        }

        hero_x = r.x;
        hero_y = r.y;

        if (r.crossed_edge) {
            edge_transitions++;
            printf("[Телепортация через край карты!]\n");

            if (edge_transitions >= 3 && !portal_active) {
                do {
                    portal_x = rand() % width;
                    portal_y = rand() % height;
                } while (map[portal_y][portal_x] == 'T' || map[portal_y][portal_x] == 'R' || (portal_x == camp_x && portal_y == camp_y));

                map[portal_y][portal_x] = 'P';
                portal_active = 1;
                printf("Вы заблудились... Где-то на карте открылся таинственный портал (P)!\n");
            }
        }

        if (portal_active && hero_x == portal_x && hero_y == portal_y) {
            printf("\nВы вошли в портал! Старый лес исчезает, вас переносит в новую чащу...\n");
            free_map(map, height);

            edge_transitions = 0;
            map = generate_forest_ring(&width, &height, &hero_x, &hero_y, &camp_x, &camp_y, &portal_active, &portal_x, &portal_y);

            if (map == NULL) {
                printf("[Критическая ошибка]: Не удалось сгенерировать новую карту. Возврат на ферму.\n");
                return;
            }

            printf("[Сгенерирован новый лес размером %dx%d]\n", width, height);
        }
    }

    free_map(map, height);
    printf("[Память карты успешно освобождена. Возврат на ферму].\n");
}

// ГЛАВНАЯ ФУНКЦИЯ
int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(NULL)); 

    char farmer_name[MAX_NAME_LEN] = "Аноним";
    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    GameTime time_tracker = {1, 8}; 

    load_item_catalogue(item_names);
    
    int inventory[INVENTORY_SIZE] = {
        ITEM_WOOD, ITEM_EMPTY, ITEM_STONE, ITEM_SEEDS, ITEM_LEATHER,
        ITEM_GOLD, ITEM_DIAMOND, ITEM_POTION, ITEM_IRON, ITEM_WOOD
    };

    printf("\nДобро пожаловать на ферму! Введите имя вашего персонажа: ");
    get_safe_string(farmer_name, MAX_NAME_LEN);

    int choice = -1;
    do {
        printf("\n======= ФЕРМА ПЕРСОНАЖА: %s =======\n", farmer_name);
        printf(" 1. Посмотреть на часы\n");
        printf(" 2. Промотать время (Поработать)\n");
        printf(" 3. Посмотреть инвентарь\n");
        printf(" 4. Положить предмет в слот\n");
        printf(" 5. Выбросить предмет\n");
        printf(" 6. Любимый ресурс\n");
        printf(" 7. Поиск предмета в рюкзаке по названию\n");
        printf(" 8. Записать состояние в дневник фермера (diary.txt)\n");
        printf(" 9. Проверить рецепты крафта (input.txt)\n");
        printf(" 10. Отправиться в приключение\n");
        printf(" 11. Выход\n");
        printf("Выберите действие (1-11): ");

        choice = get_safe_int();
        printf("\n");

        switch(choice) {
            case 1:  action_show_time(time_tracker); break;
            case 2:  action_work(&time_tracker); break;
            case 3:  print_inventory(inventory, item_names, INVENTORY_SIZE); break;
            case 4:  action_put_item(inventory, item_names, INVENTORY_SIZE); break; 
            case 5:  action_drop_item(inventory, INVENTORY_SIZE); break;
            case 6:  action_favorite_resource(inventory, item_names, INVENTORY_SIZE); break;
            case 7:  action_search_by_name(inventory, item_names, INVENTORY_SIZE); break;
            case 8:  action_save_to_diary(farmer_name, time_tracker, inventory, item_names, INVENTORY_SIZE); break;
            case 9:  action_check_recipes(inventory, item_names, INVENTORY_SIZE); break;
            case 10: action_adventure(); break; 
            case 11: printf("Завершение работы программы. До встречи на ферме, %s!\n", farmer_name); break;
            default: printf("Неверный пункт меню! Выберите число от 1 до 11.\n"); break;
        }
    } while (choice != 11);

    return 0;
}
