#include "map.h"
#include "utils.h"
#include "entity.h" // Подключаем сущности
#include <stdio.h>
#include <stdlib.h>

// Математика по модулю (тор)
int wrap_forward(int coord, int size) { 
    return (coord + 1) % size; 
}

int wrap_backward(int coord, int size) { 
    return (coord - 1 + size) % size; 
}

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

// Генерация карты с массивом сущностей
Map* map_generate(int floor) {
    Map *m = (Map*)malloc(sizeof(Map));
    if (!m) return NULL;

    m->width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    m->height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    m->floor = floor;
    m->portal_active = 0;
    m->edge_transitions = 0;

    // Создаем пустую сетку
    m->grid = (char**)malloc(m->height * sizeof(char*));
    for (int i = 0; i < m->height; i++) {
        m->grid[i] = (char*)malloc(m->width * sizeof(char));
        for (int j = 0; j < m->width; j++) m->grid[i][j] = CELL_EMPTY;
    }

    // Костер
    m->camp_x = rand() % m->width;
    m->camp_y = rand() % m->height;
    m->grid[m->camp_y][m->camp_x] = CELL_CAMP;

    // Герой
    do {
        m->hero_x = rand() % m->width;
        m->hero_y = rand() % m->height;
    } while (m->hero_x == m->camp_x && m->hero_y == m->camp_y);

    // МАССИВ СУЩНОСТЕЙ
    int max_entities = (m->width * m->height) * 30 / 100; // Выделяем под 30% карты
    m->entities = (Entity*)malloc(max_entities * sizeof(Entity));
    m->entity_count = 0;

    int mult = floor; // Сложность растет с этажом
    if (mult < 1) mult = 1;

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if ((x == m->hero_x && y == m->hero_y) || (x == m->camp_x && y == m->camp_y)) continue;

            int r = rand() % 100;
            Entity e;
            
            if (r < 15) { // Деревья
                e = (Entity){ENT_TREE, CELL_TREE, x, y, 5 * mult, 5 * mult, 0, ITEM_WOOD, 1, 0, 0};
                m->grid[y][x] = CELL_TREE;
            } else if (r < 25) { // Камни
                e = (Entity){ENT_ROCK, CELL_STONE, x, y, 10 * mult, 10 * mult, 0, ITEM_STONE, 1, 0, 0};
                m->grid[y][x] = CELL_STONE;
            } else if (r < 30) { // Аптечки
                e = (Entity){ENT_HEAL, '+', x, y, 1, 1, 0, ITEM_EMPTY, 1, 0, 0};
                m->grid[y][x] = '+';
            } else if (r < 33) { // Монстры
                e = (Entity){ENT_MONSTER, 'M', x, y, 10 * mult, 10 * mult, 3 * mult, ITEM_EMPTY, 1, 5, 10};
                m->grid[y][x] = 'M';
            } else if (r < 35) { // Дичь (Вариант 4)
                e = (Entity){ENT_ANIMAL, 'r', x, y, 1, 1, 0, ITEM_GOLD, 1, 0, 50};
                m->grid[y][x] = 'r';
            } else {
                continue; // Пустая клетка, сущность не создаем
            }
            
            m->entities[m->entity_count++] = e;
        }
    }
    return m;
}

// Удаление карты (с выводом адресов)
void map_destroy(Map *m) {
    if (!m) return;
    for (int i = 0; i < m->height; i++) free(m->grid[i]);
    free(m->grid);
    
    // ОЧИСТКА СУЩНОСТЕЙ 
    if (m->entities) {
        printf("Память массива сущностей по адресу %p освобождена\n", (void*)m->entities);
        free(m->entities);
    }
    
    printf("Память структуры карты по адресу %p освобождена\n", (void*)m);
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


// Логика движения монстров и дичи (Вариант 4)
void map_update_entities(Map *m, int hero_x, int hero_y) {
    for (int i = 0; i < m->entity_count; i++) {
        Entity *e = &m->entities[i];
        if (!e->is_alive) continue;

        // --- ВАРИАНТ 4: ПУГЛИВАЯ ДИЧЬ (КРОЛИК) ---
        if (e->type == ENT_ANIMAL) {
            // Считаем расстояние от кролика до героя
            int dist_x = abs(e->x - hero_x);
            int dist_y = abs(e->y - hero_y);
            
            // Если герой в радиусе 3 клеток — кролик пугается и бежит
            if (dist_x <= 3 && dist_y <= 3) {
                int new_x = e->x;
                int new_y = e->y;

                // Выбираем, в какую сторону бежать (по X или по Y)
                // Если по X расстояние больше, бежим по X, иначе по Y
                if (dist_x > dist_y) {
                    new_x += (e->x > hero_x) ? 1 : -1; 
                } else {
                    new_y += (e->y > hero_y) ? 1 : -1;
                }

                // Проверяем, можно ли сделать шаг (не выходим за границы и не в стену)
                if (new_x >= 0 && new_x < m->width && new_y >= 0 && new_y < m->height) {
                    if (m->grid[new_y][new_x] == CELL_EMPTY) {
                        // Стираем кролика со старого места
                        m->grid[e->y][e->x] = CELL_EMPTY; 
                        // Переносим координаты
                        e->x = new_x; 
                        e->y = new_y;
                        // Рисуем на новом месте
                        m->grid[e->y][e->x] = e->symbol; 
                    }
                }
            }
        }
        // --- МОНСТРЫ (БЛУЖДАНИЕ) ---
        else if (e->type == ENT_MONSTER) {
            // Монстр делает случайный шаг в одну из 4 сторон
            int dir = rand() % 4;
            int nx = e->x;
            int ny = e->y;
            
            if (dir == 0) ny--;      // Вверх
            else if (dir == 1) ny++; // Вниз
            else if (dir == 2) nx--; // Влево
            else if (dir == 3) nx++; // Вправо

            // Проверка границ и пустой клетки
            if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
                if (m->grid[ny][nx] == CELL_EMPTY) {
                    m->grid[e->y][e->x] = CELL_EMPTY;
                    e->x = nx; 
                    e->y = ny;
                    m->grid[e->y][e->x] = e->symbol;
                }
            }
        }
    }
}

void action_adventure(Player *p, GameTime *t, const char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    if (p->stamina <= 0) { printf("Ошибка: нет выносливости!\n"); return; }
    if (t->hour == NIGHT_HOUR) { printf("Ошибка: ночью все спят!\n"); return; }

    Map *m = map_generate(1);
    int in_adventure = 1;
    int total_resources_gathered = 0;
    int total_monsters_killed = 0;
    char last_event[128] = "Вы вошли в лес...";

    while (in_adventure) {
        // --- ФАЗА 3: RENDER (Отрисовка) ---
        printf("\033[2J\033[H"); // Очистка экрана 
        printf("\n--------------------------------------------------\n");
        printf("\n--------------------------------------------------\n");
        printf("HP: %d/%d | Сила: %d | Вын: %d/%d | Сытость: %d\n", p->hp, p->max_hp, p->attack, p->stamina, p->max_stamina, p->hunger);
        printf("Золото: %d | Этаж: %d | Время: %02d:00\n", p->gold, m->floor, t->hour);
        printf("--------------------------------------------------\n");
        map_display(m);
        printf("[Событие]: %s\n", last_event);
        printf("Управление (W,A,S,D - шаг, E - поесть, Q - выход): ");

        // --- ФАЗА 1: INPUT (Ввод) ---
        char move = getchar();
        if (move != '\n' && move != EOF) while (getchar() != '\n');
        if (move >= 'a' && move <= 'z') move -= 32;

        if (move == 'Q') {
            if (m->hero_x == m->camp_x && m->hero_y == m->camp_y) {
                printf("Вы вернулись на ферму!\n");
                in_adventure = 0;
            } else {
                snprintf(last_event, sizeof(last_event), "Выйти можно только на костре (^).");
            }
            continue;
        }

        if (move == 'E') {
            player_eat(p, item_names);
            snprintf(last_event, sizeof(last_event), "Вы перекусили.");
            continue;
        }

        MoveResult r = map_calc_next_position(m->hero_x, m->hero_y, m->width, m->height, move);
        if (!r.valid) { snprintf(last_event, sizeof(last_event), "Неверная команда!"); continue; }

        // --- ФАЗА 2: UPDATE (Обновление мира) ---
        
        // Ищем сущность на клетке, куда хотим шагнуть
        Entity *target = entity_get_at(m->entities, m->entity_count, r.x, r.y);

        if (target) {
            // Взаимодействие вместо шага (тратим 2 выносливости)
            if (p->stamina < 2) {
                snprintf(last_event, sizeof(last_event), "Не хватает сил для действия!");
                continue;
            }
            p->stamina -= 2;

            // 1. РУБКА РЕСУРСОВ (Дерево T / Камень R)
            if (target->type == ENT_TREE || target->type == ENT_ROCK) {
                target->hp -= p->attack;
                snprintf(last_event, sizeof(last_event), "Вы ударили %c на %d урона! Осталось HP: %d", target->symbol, p->attack, target->hp);
                
                if (target->hp <= 0) {
                    entity_kill(target);
                    m->grid[target->y][target->x] = CELL_EMPTY; // Убираем с карты
                    total_resources_gathered++;
                    
                    // Пытаемся положить в инвентарь
                    int slot = -1;
                    for (int i = 0; i < INVENTORY_SIZE; i++) {
                        if (p->inventory[i] == ITEM_EMPTY) { slot = i; break; }
                    }
                    if (slot != -1) {
                        p->inventory[slot] = target->drop_item_id;
                        snprintf(last_event, sizeof(last_event), "Вы добыли: %s!", item_names[target->drop_item_id]);
                    } else {
                        snprintf(last_event, sizeof(last_event), "Рюкзак полон! Ресурс потерян.");
                    }
                }
            }
            // 2. АПТЕЧКА (+)
            else if (target->type == ENT_HEAL) {
                entity_kill(target);
                m->grid[target->y][target->x] = CELL_EMPTY;
                p->hp += 20; if (p->hp > p->max_hp) p->hp = p->max_hp;
                p->stamina += 10; if (p->stamina > p->max_stamina) p->stamina = p->max_stamina;
                snprintf(last_event, sizeof(last_event), "Вы съели ягоды! +20 HP, +10 Вын.");
            }
            // 3. БОЙ (Монстр M)
            else if (target->type == ENT_MONSTER) {
                // Бьем мы
                int dmg = p->attack - (target->dodge_chance > 0 ? (target->dodge_chance / 10) : 0); // Условная защита
                if (dmg < 1) dmg = 1;
                target->hp -= dmg;
                
                if (target->hp <= 0) {
                    entity_kill(target);
                    m->grid[target->y][target->x] = CELL_EMPTY;
                    total_monsters_killed++;
                    p->gold += 10 * m->floor; // Награда
                    snprintf(last_event, sizeof(last_event), "Монстр повержен! +%d золота.", 10 * m->floor);
                } else {
                    // Бьет он
                    int enemy_dmg = target->attack - (p->defense / 2);
                    if (enemy_dmg < 1) enemy_dmg = 1;
                    p->hp -= enemy_dmg;
                    snprintf(last_event, sizeof(last_event), "Бой! Вы нанесли %d, получили %d.", dmg, enemy_dmg);
                }
            }
            // 4. ДИЧЬ (Кролик r)
            else if (target->type == ENT_ANIMAL) {
                // Проверка на уклонение (Вариант 4)
                int dodge_roll = rand() % 100;
                if (dodge_roll < target->dodge_chance) {
                    snprintf(last_event, sizeof(last_event), "Кролик увернулся и убежал!");
                    // Кролик делает шаг в сторону
                } else {
                    entity_kill(target);
                    m->grid[target->y][target->x] = CELL_EMPTY;
                    p->gold += 50;
                    snprintf(last_event, sizeof(last_event), "Вы поймали пугливую дичь! +50 золота.");
                }
            }
        } else {
            // Клетка пуста — просто идем
            m->hero_x = r.x;
            m->hero_y = r.y;
            p->stamina -= STEP_STAMINA_COST;
            player_update_hunger(p, 0, 1);
            snprintf(last_event, sizeof(last_event), "Вы сделали шаг.");
        }

        // Движение монстров (Вариант 4)
        map_update_entities(m, m->hero_x, m->hero_y);

        // Проверки на смерть/обморок
        if (p->hp <= 0 || p->stamina <= 0) {
            printf("\n[ОБМОРОК! Вы потеряли сознание...]\n");
            p->gold /= 2;
            printf("Спасатели забрали половину золота. Поход окончен.\n");
            in_adventure = 0;
        }
    }
    // Записываем отчет в дневник перед выходом
    action_save_adventure_report(*t, m->floor, total_resources_gathered, total_monsters_killed);
    
    map_destroy(m); // Очистка памяти
}

