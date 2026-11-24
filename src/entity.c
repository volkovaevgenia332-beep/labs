
#include "entity.h"
#include <stdio.h>
#include <stdlib.h>

// Вспомогательная функция: поиск сущности по координатам
Entity* entity_get_at(Entity *entities, int count, int x, int y) {
    for (int i = 0; i < count; i++) {
        if (entities[i].is_alive && entities[i].x == x && entities[i].y == y) {
            return &entities[i];
        }
    }
    return NULL;
}

// Удаление сущности (помечаем мертвой)
void entity_kill(Entity *e) {
    if (e) e->is_alive = 0;
}