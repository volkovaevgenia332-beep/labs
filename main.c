#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#include "src/game.h"
#include "src/utils.h"
#include "src/player.h"
#include "src/map.h"

void action_time_management(Player *p, GameTime *t) {
    int choice = -1;
    while (choice != 4) {
        printf("\n--- УПРАВЛЕНИЕ ВРЕМЕНЕМ ---\n");
        printf(" 1. Посмотреть на часы\n 2. Ждать\n 3. Лечь спать\n 4. Назад\n");
        printf("Выбор: ");
        choice = get_safe_int();

        if (choice == 1) printf("Сейчас: День %d, %02d:00\n", t->day, t->hour);
        else if (choice == 2) {
            printf("Сколько часов ждать? ");
            int hours = get_safe_int();
            if (hours < 1) continue;
            if (t->hour + hours >= NIGHT_HOUR && t->hour < NIGHT_HOUR) {
                printf("Ошибка: нельзя пропустить 02:00! Идите спать.\n");
                continue;
            }
            t->hour = (t->hour + hours) % HOURS_IN_DAY;
            if (t->hour < hours) t->day++; 
            player_update_hunger(p, hours, 0);
            printf("Вы прождали %d часов. Сейчас %02d:00\n", hours, t->hour);
        } else if (choice == 3) {
            t->day++; t->hour = 8;
            p->hp = p->max_hp; p->stamina = p->max_stamina; p->hunger = MAX_HUNGER;
            printf("Вы выспались! День %d, 08:00. Силы восстановлены.\n", t->day);
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(NULL)); 

    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    GameTime time_tracker = {1, 8}; 
    load_item_catalogue(item_names);
    
    Player *player = NULL;
    int choice = -1;

    do {
        printf("\n======= ФЕРМА =======\n");
        printf(" 1. Создать / пересоздать персонажа\n");
        printf(" 2. Карточка персонажа\n");
        printf(" 3. Управление временем\n");
        printf(" 4. Посмотреть инвентарь\n");
        printf(" 5. Положить предмет\n");
        printf(" 6. Выбросить предмет\n");
        printf(" 7. Любимый ресурс\n");
        printf(" 8. Поиск предмета\n");
        printf(" 9. Записать в дневник\n");
        printf(" 10. Проверить рецепты\n");
        printf(" 11. Отправиться в приключение\n");
        printf(" 12. Выход\n");
        printf("Выберите действие: ");
        choice = get_safe_int();

        // Защита: если персонажа нет, доступны только 1 и 12
        if (player == NULL && choice != 1 && choice != 12) {
            printf("Ошибка: Сначала создайте персонажа!\n");
            continue;
        }

        switch(choice) {
            case 1: 
                if (player != NULL) {
                    printf("Точно удалить? (1 - Да, 0 - Нет): ");
                    if (get_safe_int() == 1) { player_destroy(player); player = player_create(); }
                } else player = player_create();
                break;
            case 2: player_print_card(player, time_tracker, item_names); break;
            case 3: action_time_management(player, &time_tracker); break;
            case 4: print_inventory(player->inventory, item_names, INVENTORY_SIZE); break;
            case 5: action_put_item(player->inventory, item_names, INVENTORY_SIZE); break;
            case 6: action_drop_item(player->inventory, INVENTORY_SIZE); break;
            case 7: action_favorite_resource(player->inventory, item_names, INVENTORY_SIZE); break;
            case 8: action_search_by_name(player->inventory, item_names, INVENTORY_SIZE); break;
            case 9: action_save_to_diary(player->name, time_tracker, player->inventory, item_names, INVENTORY_SIZE); break;
            case 10: action_check_recipes(player->inventory, item_names, INVENTORY_SIZE); break;
            case 11: action_adventure(player, &time_tracker, item_names); break;
            case 12: printf("Выход...\n"); break;
            default: printf("Неверный пункт меню.\n"); break;
        }
    } while (choice != 12);

    if (player != NULL) player_destroy(player);
    return 0;
}