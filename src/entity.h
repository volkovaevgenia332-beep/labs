#ifndef ENTITY_H
#define ENTITY_H

#include "game.h"

// Типы объектов на карте
typedef enum {
    ENT_TREE,     // Дерево
    ENT_ROCK,     // Камень
    ENT_HEAL,     // Аптечка / ягоды
    ENT_MONSTER,  // Злой монстр
    ENT_ANIMAL    // Пугливая дичь (наш Вариант 4)
} EntityType;

// Структура одного объекта
typedef struct {
    EntityType type;
    char symbol;        // Символ для отрисовки ('T', 'R', '+', 'M', 'r')
    int x, y;           // Координаты на карте
    int hp, max_hp;     // Здоровье (или прочность для дерева)
    int attack;         // Урон (у монстров)
    int drop_item_id;   // ID предмета из items.txt, который выпадет при уничтожении
    int is_alive;       // 1 - стоит на карте, 0 - убит/собран
    
    // Поля для Варианта 4 (и для боевки)
    int crit_chance;    // Шанс крита в %
    int dodge_chance;   // Шанс уклонения в %
} Entity;


// Прототипы функций для работы с сущностями
Entity* entity_get_at(Entity *entities, int count, int x, int y);
void entity_kill(Entity *e);

#endif // ENTITY_H
