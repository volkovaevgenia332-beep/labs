#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#include "src/game.h"
#include "src/utils.h"
#include "src/player.h"

// Пока оставим функции инвентаря здесь
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

// Заглушки для старых функций (чтобы компилировалось)
void action_put_item(int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf("Введите индекс слота (от 0 до %d): ", size - 1);
    int slot = get_safe_int();
    if (slot < 0 || slot >= size) { printf("Ошибка индекса.\n"); return; }
    printf("Введите ID предмета: ");
    int id = get_safe_int();
    if (id >= 0 && id < MAX_ITEMS) inv[slot] = id;
}

void action_drop_item(int inv[], int size) {
    printf("Введите индекс слота: ");
    int slot = get_safe_int();
    if (slot >= 0 && slot < size) inv[slot] = ITEM_EMPTY;
}

void action_favorite_resource(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf("Любимый ресурс (заглушка).\n");
}

void action_search_by_name(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    printf("Поиск (заглушка).\n");
}

void free_map(char **map, int height) {
    if (map == NULL) return;
    for (int i = 0; i < height; i++) if (map[i] != NULL) free(map[i]);
    free(map);
}

void display_map(char **map, int width, int height, int hero_x, int hero_y) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x== hero_x && y == hero_y) printf("@ ");
            else printf("%c ", map[y][x]);
        }
        printf("\n");
    }
}

void action_adventure() { printf("Приключение (заглушка, карту сделаем на шаге 3).\n"); }


// ГЛАВНАЯ ФУНКЦИЯ
int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(NULL)); 

    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    GameTime time_tracker = {1, 8}; 

    load_item_catalogue(item_names);
    
    Player *player = NULL; // Изначально персонажа нет

    int choice = -1;
    do {
        printf("\n======= ФЕРМА =======\n");
        printf(" 1. Создать / пересоздать персонажа\n");
        printf(" 2. Карточка персонажа\n");
        printf(" 3. Управление временем\n");
        printf(" 4. Инвентарь\n");
        printf(" 5-9. (Другие действия)\n");
        printf(" 10. Отправиться в приключение\n");
        printf(" 11. Выход\n");
        printf("Выберите действие (1-11): ");

        choice = get_safe_int();
        printf("\n");

        switch(choice) {
            case 1: 
                if (player != NULL) {
                    printf("Персонаж уже существует. Точно хотите удалить его? (1 - Да, 0 - Нет): ");
                    int confirm = get_safe_int();
                    if (confirm == 1) {
                        player_destroy(player);
                        player = player_create();
                    } else {
                        printf("Отмена.\n");
                    }
                } else {
                    player = player_create();
                }
                break;
            case 2: 
                if (player == NULL) {
                    printf("Ошибка: персонаж не создан! Сначала выберите пункт 1.\n");
                } else {
                    player_print_card(player, time_tracker, item_names);
                }
                break;
            case 3: // Время (сделаем позже)
                if (!player) { printf("Сначала создайте персонажа.\n"); break; }
                action_show_time(time_tracker); 
                break;
            case 4: 
                if (!player) { printf("Сначала создайте персонажа.\n"); break; }
                print_inventory(player->inventory, item_names, INVENTORY_SIZE); 
                break;
            case 10: 
                if (!player) { printf("Сначала создайте персонажа.\n"); break; }
                action_adventure(); 
                break;
            case 11: 
                printf("Завершение работы программы.\n"); 
                break;
            default: 
                printf("Этот пункт пока в разработке.\n"); 
                break;
        }
    } while (choice != 11);

    // ВАЖНО: Очистка памяти при выходе!
    if (player != NULL) {
        player_destroy(player);
    }

    return 0;
}
