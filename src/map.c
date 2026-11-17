#include "map.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// Математика по модулю
int wrap_forward(int coord, int size) { return (coord + 1) % size; }
int wrap_backward(int coord, int size) { return (coord - 1 + size) % size; }

MoveResult map_calc_next_position(int hero_x, int hero_y, int width, int height, char move) {
    MoveResult res = { hero_x, hero_y, 0, 1 };
    switch (move) {
        case 'W': res.y = wrap_backward(hero_y, height); res.crossed_edge = (hero_y == 0); break;
        case 'S': res.y = wrap_forward(hero_y, height); res.crossed_edge = (hero_y == height - 1); break;
        case 'A': res.x = wrap_backward(hero_x, width); res.crossed_edge = (hero_x == 0); break;
        case 'D': res.x = wrap_forward(hero_x, width); res.crossed_edge = (hero_x == width - 1); break;
        default: res.valid = 0; break;
    }
    return res;
}

// Генерация карты
Map* map_generate(int floor) {
    Map *m = (Map*)malloc(sizeof(Map));
    if (!m) return NULL;

    m->width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    m->height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    m->floor = floor;
    m->portal_active = 0;
    m->edge_transitions = 0;

    m->grid = (char**)malloc(m->height * sizeof(char*));
    for (int i = 0; i < m->height; i++) {
        m->grid[i] = (char*)malloc(m->width * sizeof(char));
    }

    int obstacle_chance = 15 + rand() % 16;
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if ((rand() % 100) < obstacle_chance) m->grid[y][x] = (rand() % 2 == 0) ? CELL_TREE : CELL_STONE;
            else m->grid[y][x] = CELL_EMPTY;
        }
    }

    m->camp_x = rand() % m->width;
    m->camp_y = rand() % m->height;
    m->grid[m->camp_y][m->camp_x] = CELL_CAMP;

    do {
        m->hero_x = rand() % m->width;
        m->hero_y = rand() % m->height;
    } while (m->grid[m->hero_y][m->hero_x] == CELL_TREE || m->grid[m->hero_y][m->hero_x] == CELL_STONE);

    return m;
}

// Удаление карты (с выводом адреса по заданию)
void map_destroy(Map *m) {
    if (!m) return;
    for (int i = 0; i < m->height; i++) free(m->grid[i]);
    free(m->grid);
    printf("Память структуры по адресу %p освобождена\n", (void*)m);
    free(m);
}

// Вывод карты
void map_display(const Map *m) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if (x == m->hero_x && y == m->hero_y) printf("@ ");
            else printf("%c ", m->grid[y][x]);
        }
        printf("\n");
    }
}

// Главная функция приключения
void action_adventure(Player *p, GameTime *t, const char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    // Проверки допуска
    if (p->stamina <= 0) { printf("Ошибка: у вас нет выносливости для похода!\n"); return; }
    if (t->hour == NIGHT_HOUR) { printf("Ошибка: ночью (02:00) все спят, поход невозможен!\n"); return; }

    Map *m = map_generate(1);
    int total_steps = 0;
    int in_adventure = 1;

    printf("\n[Приключение началось! Этаж %d, размер %dx%d]\n", m->floor, m->width, m->height);

    while (in_adventure) {
        // HUD (Интерфейс)
        printf("\n--------------------------------------------------\n");
        printf("HP: %d/%d | Выносливость: %d/%d | Сытость: %d\n", p->hp, p->max_hp, p->stamina, p->max_stamina, p->hunger);
        printf("Золото: %d | Этаж: %d | Время: %02d:00\n", p->gold, m->floor, t->hour);
        printf("--------------------------------------------------\n");
        
        map_display(m);
        printf("Управление (W, A, S, D - шаг, E - поесть, Q - выйти на костре): ");

        char move = getchar();
        if (move != '\n' && move != EOF) while (getchar() != '\n');
        if (move >= 'a' && move <= 'z') move -= 32;

        if (move == 'Q') {
            if (m->hero_x == m->camp_x && m->hero_y == m->camp_y) {
                printf("Вы благополучно вернулись на ферму!\n");
                in_adventure = 0;
            } else {
                printf("Выйти можно только стоя на костре (^).\n");
            }
            continue;
        }

        if (move == 'E') {
            player_eat(p, item_names);  
            continue;
        }

        MoveResult r = map_calc_next_position(m->hero_x, m->hero_y, m->width, m->height, move);
        if (!r.valid) { printf("Неверная команда!\n"); continue; }

        if (m->grid[r.y][r.x] == CELL_TREE || m->grid[r.y][r.x] == CELL_STONE) {
            printf("Там препятствие!\n");
            continue;
        }

        // Движение
        m->hero_x = r.x;
        m->hero_y = r.y;
        total_steps++;
        p->stamina -= STEP_STAMINA_COST;
        player_update_hunger(p, 0, 1); // Голод падает за шаги

        if (r.crossed_edge) {
            m->edge_transitions++;
            printf("[Переход через край!]\n");
            if (m->edge_transitions >= PORTAL_THRESHOLD && !m->portal_active) {
                m->portal_active = 1;
                m->portal_x = rand() % m->width;
                m->portal_y = rand() % m->height;
                m->grid[m->portal_y][m->portal_x] = CELL_PORTAL;
                printf("Открылся портал (P)!\n");
            }
        }

        // Вход в портал (новый этаж)
        if (m->portal_active && m->hero_x == m->portal_x && m->hero_y == m->portal_y) {
            printf("\n[Вы перешли на новый этаж!]\n");
            p->stamina -= FLOOR_STAMINA_COST;
            t->hour += FLOOR_TIME_COST;
            if (t->hour >= HOURS_IN_DAY) { t->hour %= HOURS_IN_DAY; t->day++; }
            player_update_hunger(p, FLOOR_TIME_COST, 0);

            // Награда
            int reward = BASE_GOLD_REWARD * m->floor;
            p->gold += reward;
            printf("[Награда: +%d золота]\n", reward);

            int next_floor = m->floor + 1;
            map_destroy(m);
            m = map_generate(next_floor);
        }

        // Экстренная эвакуация (обморок)
        if (p->stamina <= 0 || t->hour == NIGHT_HOUR || p->hp <= 0) {
            printf("\n[ОБМОРОК! Вы потеряли сознание...]\n");
            int lost_gold = p->gold / 2;
            p->gold -= lost_gold;
            printf("Спасатели забрали половину золота (%d). Поход окончен.\n", lost_gold);
            in_adventure = 0;
        }
    }

    map_destroy(m);
}