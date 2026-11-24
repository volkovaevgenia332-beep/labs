#include "game.h" 
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// защита от неправильного ввода(дурака)
int get_safe_int(void) {
    int value;
    while (scanf("%d", &value) != 1) {
        printf("Ошибка! Введено не число. Пожалуйста, введите корректное число: ");
        while (getchar() != '\n');
    }
    while (getchar() != '\n');
    return value;
}

// безопасное чтение строки без символа переноса строки
void get_safe_string(char *buffer, int max_len) {
    fgets(buffer, max_len, stdin);
    // удаление символа переноса строки \n, который автоматически захватывает fgets
    buffer[strcspn(buffer, "\n")] = '\0';
}

// загрузка названий предметов из файла или значения по умолч
void load_item_catalogue(char names[MAX_ITEMS][MAX_NAME_LEN]) {
    // ИЗМЕНЕНО: добавлен путь data/
    FILE *file = fopen("data/items.txt", "r");
    if (file == NULL) {
        printf("[Предупреждение]: Файл data/items.txt не найден! Загружены имена по умолчанию.\n");
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
        // ДОБАВЛЕНО: новые предметы еды
        strcpy(names[10], "Хлеб");
        strcpy(names[11], "Яблоко");
        strcpy(names[12], "Похлёбка");
        return;
    }

    int id;
    char temp_name[MAX_NAME_LEN];
    while (fscanf(file, "%d %31s", &id, temp_name) == 2) {
        if (id >= 0 && id < MAX_ITEMS) {
            strncpy(names[id], temp_name, MAX_NAME_LEN - 1);
            names[id][MAX_NAME_LEN - 1] = '\0';
        }
    }
    fclose(file);
    printf("[Успех]: Каталог предметов успешно загружен из data/items.txt.\n");
}

// посмотреть на часы
void action_show_time(GameTime t) {
    printf("Текущее время: День %d, %02d:00\n", t.day, t.hour);
}

// промотать время(поработать)
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

// запись состояния в дневник фермера
void action_save_to_diary(const char *farmer_name, GameTime t, const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    // ИЗМЕНЕНО: добавлен путь data/
    FILE *file = fopen("data/diary.txt", "a"); // a — режим дозаписи в конец файла
    if (file == NULL) {
        printf("Ошибка! Не удалось открыть или создать файл data/diary.txt.\n");
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
    printf("[Успех]: Запись красиво сохранена в файл data/diary.txt!\n");
}

// проверка рецептов крафта
void action_check_recipes(const int inv[], const char names[MAX_ITEMS][MAX_NAME_LEN], int size) {
    // ИЗМЕНЕНО: добавлены пути data/
    FILE *fin = fopen("data/input.txt", "r");
    FILE *fout = fopen("data/output.txt", "w");

    if (fin == NULL) {
        printf("[Ошибка]: Входной файл рецептов data/input.txt не найден! Нечего обрабатывать.\n");
        if (fout) fclose(fout);
        return;
    }
    if (fout == NULL) {
        printf("[Ошибка]: Не удалось создать файл результатов data/output.txt.\n");
        fclose(fin);
        return;
    }

    int player_resources[MAX_ITEMS] = {0};
    for (int i = 0; i < size; i++) {
        player_resources[inv[i]]++;
    }
    
    char line[128]; 
    printf("--- Проверка рецептов крафта ---\n");

    while (fgets(line, sizeof(line), fin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        char recipe_name[MAX_NAME_LEN] = {0};
        int required_ids[MAX_ITEMS] = {0};
        
        char *token = strtok(line, ":");
        if (token != NULL) {
            strncpy(recipe_name, token, MAX_NAME_LEN - 1);
        }

        token = strtok(NULL, " ");
        while (token != NULL) {
            int id = atoi(token);
            if (id >= 0 && id < MAX_ITEMS) {
                required_ids[id]++;
            }
            token = strtok(NULL, " ");
        }

        int has_enough = 1;
        char missing_report[128] = ""; 

        for (int id = 0; id < MAX_ITEMS; id++) {
            if (required_ids[id] > player_resources[id]) {
                has_enough = 0;
                strcat(missing_report, names[id]);
                strcat(missing_report, " ");
            }
        }

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
    printf("\n[Успех]: Полный отчет о крафте сохранен в файл data/output.txt!\n");
}


void action_save_adventure_report(GameTime t, int floor, int resources, int monsters) {
    FILE *file = fopen("data/diary.txt", "a"); // "a" — режим дозаписи
    if (file == NULL) {
        printf("[Ошибка]: Не удалось открыть data/diary.txt для записи отчета.\n");
        return;
    }
    
    fprintf(file, "=== ОТЧЕТ О ПОХОДЕ ===\n");
    fprintf(file, "Время окончания: День %d, %02d:00\n", t.day, t.hour);
    fprintf(file, "Достигнутый этаж: %d\n", floor);
    fprintf(file, "Добыто ресурсов: %d\n", resources);
    fprintf(file, "Побеждено монстров: %d\n", monsters);
    fprintf(file, "----------------------\n\n");
    
    fclose(file);
    printf("[Успех]: Отчет о походе сохранен в data/diary.txt!\n");
}