#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h> //генерация рандомных чисел


// защита от магических чисел  (константы)
#define INVENTORY_SIZE 10
#define HOURS_IN_DAY 24
#define MAX_ITEMS 10
#define MAX_NAME_LEN 32

//ограничения для генерации карты
#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30

// список предметов в игре
#define ITEM_EMPTY 0 
#define ITEM_WOOD 1
#define ITEM_STONE 2
#define ITEM_SEEDS 3
#define ITEM_IRON 4
#define ITEM_GOLD 5
#define ITEM_POTION 6
#define ITEM_LEATHER 7
#define ITEM_DIAMOND 8

// общее кол-во уникальных предметов
//#define TOTAL_UNIQUE_ITEMS 9

typedef struct {
    int day;
    int hour;
} GameTime;

// защита от неправильного ввода(дурака)
int get_safe_int() {
    int value;
    while (scanf("%d", &value) !=1) {
        printf("Ошибка! Введено не число. Пожалуйста, введите корректное число: ");
        while (getchar() != '\n');
    
    }
    while (getchar() != '\n');
    return value;
}

//безопасное чтение строки без символа переноса строки
void get_safe_string (char * buffer, int max_len) {
    fgets(buffer, max_len, stdin);
    // удаление символа переноса строки \n, который автоматически захватывает fgets
    buffer[strcspn(buffer, "\n")]= '\0';
}

//ФУНКЦИИ РАБОТЫ С КАТАЛОГОМ И ИНВЕНТАРЕМ

//загрузка названий предметов из файла или значения по умолч
void load_item_catalogue (char names[MAX_ITEMS][MAX_NAME_LEN]){
    FILE *file = fopen("items.txt", "r");
    if (file == NULL) {
        printf("[Предупреждение]: Файл items.txt не найден! Загружены имена по умолчанию.\n");
        // заполнение по умолчанию (защита от падения проги)
        strcpy(names[0], "Пусто");
        strcpy(names[1], "Дерево");
        strcpy(names[2], "Камень");
        strcpy(names[3], "Семена_пшеницы");
        strcpy(names[4], "Железо");
        strcpy(names[5], "Золото");
        strcpy(names[6], "Зелье");
        strcpy(names[7], "Кожа");
        strcpy(names[8], "Алмаз");
        strcpy(names[9], "Ткань");
        return;
    }

    int id;
    char temp_name[MAX_NAME_LEN];
    while (fscanf(file, "%d %31s", &id, temp_name)==2) {
        if (id >= 0 && id < MAX_ITEMS) {
            strncpy (names[id], temp_name, MAX_NAME_LEN -1);
            names[id][MAX_NAME_LEN - 1] = '\0';
        }
    }
    fclose(file);
    printf("[Успех]: Каталог предметов успешно загружен из items.txt. \n");
}

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

// посмотреть на часы
void action_show_time(GameTime t) {
    printf("Текущее время: День %d, %02d:00\n", t.day, t.hour);
}

//промотать время(поработать)
void action_work(GameTime *t) {
    printf("Сколько часов вы хотите поработать? ");
    int hours = get_safe_int();
    if (hours < 0) {
        printf("Ошибка! Время работы не может быть отрицательным.\n");
        return;
    }
    t->hour += hours;
    t->day += t->hour / HOURS_IN_DAY;
    t->hour = t->hour % HOURS_IN_DAY;
    printf("Вы успешно поработали! Время перемотано.\n");
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

    //поиск ID, которому соответствует название в каталоге предметов
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

    //поиск этого ID внутри инвентаря игрока
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

//запись состояния в дневник фермера
void action_save_to_diary(const char *farmer_name, GameTime t, const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    FILE *file = fopen("diary.txt", "a"); // a — режим дозаписи в конец файла
    if (file == NULL) {
        printf("Ошибка! Не удалось открыть или создать файл diary.txt.\n");
        return;
    }

    fprintf(file, "=== ЗАПИСЬ В ДНЕВНИКЕ ===\n");
    fprintf(file, "Фермер: %s\n", farmer_name);
    fprintf(file, "Время: День %d, %02d:00\n", t.day, t.hour);
    fprintf(file, "Содержимое рюкзака:\n");
    for (int i = 0; i < size; i++) {
        fprintf(file, "  Слот %d: %s\n", i, names[inv[i]]);
    }
    fprintf(file, "-------------------------\n\n");

    fclose(file);
    printf("[Успех]: Запись красиво сохранена в файл diary.txt!\n");
}

//проверка рецептов крафта
void action_check_recipes(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    FILE *fin = fopen("input.txt", "r");
    FILE *fout = fopen("output.txt", "w"); // w — создание и полная перезапись файла вывода

    if (fin == NULL) {
        printf("[Ошибка]: Входной файл рецептов input.txt не найден! Нечего обрабатывать.\n");
        if (fout) fclose(fout);
        return;
    }
    if (fout == NULL) {
        printf("[Ошибка]: Не удалось создать файл результатов output.txt.\n");
        fclose(fin);
        return;
    }

    // счет текущего количества ресурсов у игрока
    int player_resources[MAX_ITEMS] = {0};
    for (int i = 0; i < size; i++) {
        player_resources[inv[i]]++;
    }

    char line[128];
    printf("--- Проверка рецептов крафта ---\n");

    //чтение input.txt построчно
    while (fgets(line, sizeof(line), fin)) {
        //удаление технического переноса строки \n
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        char recipe_name[MAX_NAME_LEN] = {0};
        int required_ids[MAX_ITEMS] = {0}; //массив для подсчета требований рецепта
        
        //разбиваю строку, сначала вытаскиваю название до двоеточия
        char *token = strtok(line, ":");
        if (token != NULL) {
            strncpy(recipe_name, token, MAX_NAME_LEN - 1);
        }

        //вытаскиваю все ID предметов, идущие через пробел после двоеточия
        token = strtok(NULL, " ");
        while (token != NULL) {
            int id = atoi(token);
            if (id >= 0 && id < MAX_ITEMS) {
                required_ids[id]++;
            }
            token = strtok(NULL, " ");
        }

        //сравнение требований рецепта с тем, что есть в рюкзаке фермера
        int has_enough = 1;
        char missing_report[128] = "";

        for (int id = 0; id < MAX_ITEMS; id++) {
            if (required_ids[id] > player_resources[id]) {
                has_enough = 0;
                //дозапись имени компонента, которого не хватило, в строку
                strcat(missing_report, names[id]);
                strcat(missing_report, " ");
            }
        }

        //вывод вердикта на экран и дубликат его в выходной файл output.txt
        if (has_enough) {
            printf("Рецепт '%s': [ДОСТУПНО]\n", recipe_name);
            fprintf(fout, "Рецепт '%s': [ДОСТУПНО]\n", recipe_name);
        } else {
            printf("Рецепт '%s': [НЕ ХВАТАЕТ РЕСУРСОВ] (Нужно: %s)\n", recipe_name, missing_report);
            fprintf(fout, "Рецепт '%s': [НЕ ХВАТАЕТ РЕСУРСОВ] (Недостает: %s)\n", recipe_name, missing_report);
        }
    }

    fclose(fin);
    fclose(fout);
    printf("\n[Успех]: Полный отчет о крафте сохранен в файл output.txt!\n");
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
                printf("@ "); //вывод героя поверх клетки
            } else {
                printf("%c ", map[y][x]);
            }
        }
        printf("\n");
    }
}


  //ГЛАВНАЯ ФУНКЦИЯ

  int main() {
    


    char farmer_name[MAX_NAME_LEN] = "Аноним";
    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    GameTime time = {1, 8}; // Стартовое время: День 1, 8:00 утра

    //загрузка каталога предметов и инициализация рюкзака
    load_item_catalogue(item_names);
    int inventory[INVENTORY_SIZE] = {
        ITEM_WOOD, ITEM_EMPTY, ITEM_STONE, ITEM_SEEDS, ITEM_LEATHER,
        ITEM_GOLD, ITEM_DIAMOND, ITEM_POTION, ITEM_IRON, ITEM_WOOD
    };

    //запрос имени фермера строго при старте программы
    printf("\nДобро пожаловать на ферму! Введите имя вашего персонажа: ");
    get_safe_string(farmer_name, MAX_NAME_LEN);

    int choice = -1;
    do {
        //шапка меню динамически приветствует игрока по имени
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
        printf(" 10. Выход\n");
        printf("Выберите действие (1-10): ");

        choice = get_safe_int();
        printf("\n");

        switch(choice) {
            case 1:  action_show_time(time); break;
            case 2:  action_work(&time); break;
            case 3:  print_inventory(inventory, item_names, INVENTORY_SIZE); break;
            case 4:  action_put_item(inventory, item_names, INVENTORY_SIZE);
            case 5:  action_drop_item(inventory, INVENTORY_SIZE); break;
            case 6:  action_favorite_resource(inventory, item_names, INVENTORY_SIZE); break;
            case 7:  action_search_by_name(inventory, item_names, INVENTORY_SIZE); break;
            case 8:  action_save_to_diary(farmer_name, time, inventory, item_names, INVENTORY_SIZE); break;
            case 9:  action_check_recipes(inventory, item_names, INVENTORY_SIZE); break;
            case 10: printf("Завершение работы программы. До встречи на ферме, %s!\n", farmer_name); break;
            default: printf("Неверный пункт меню! Выберите число от 1 до 10.\n"); break;
        }
    } while (choice != 10); //цикл работает, пока не введут 10

    return 0;
}