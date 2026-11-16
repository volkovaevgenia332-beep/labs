#ifndef GAME_H
#define GAME_H


// ОБЩИЕ КОНСТАНТЫ (защита от магических чисел)


// Размеры
#define INVENTORY_SIZE   10
#define MAX_ITEMS        13   // 0-12 (включая 3 новых еды)
#define MAX_NAME_LEN     32
#define HOURS_IN_DAY     24
#define NIGHT_HOUR       2    // 02:00 — граница сна

// Карта
#define MIN_MAP_SIZE     5
#define MAX_MAP_SIZE     30
#define PORTAL_THRESHOLD 3    // переходов через край до открытия портала

// Стартовые характеристики
#define START_HP         100
#define MAX_HP           100
#define START_STAMINA    50
#define MAX_STAMINA      50
#define START_GOLD       50
#define START_HUNGER     100
#define MAX_HUNGER       100
#define HUNGER_LOW       20   // порог голода

// Расход/восстановление
#define STEP_STAMINA_COST        1
#define FLOOR_STAMINA_COST       5
#define FLOOR_TIME_COST          1     // часов
#define HUNGER_PER_HOUR          2
#define HUNGER_PER_5_STEPS       3
#define STARVE_HP_LOSS           1     // HP за шаг при голоде
#define SLEEP_RESTORE_STAMINA    50    // до max
#define SLEEP_RESTORE_HP         100   // до max

// Награда
#define BASE_GOLD_REWARD         10    // * номер этажа
#define EMERGENCY_GOLD_PENALTY   0.5   // половина золота

// ID предметов (базовые)
#define ITEM_EMPTY        0
#define ITEM_WOOD         1
#define ITEM_STONE        2
#define ITEM_SEEDS        3
#define ITEM_IRON         4
#define ITEM_GOLD         5
#define ITEM_POTION       6
#define ITEM_LEATHER      7
#define ITEM_DIAMOND      8
#define ITEM_CLOTH        9

// ID предметов-еды (вариант)
#define ITEM_BREAD        10
#define ITEM_APPLE        11
#define ITEM_SOUP         12

// Символы карты
#define CELL_EMPTY        '.'
#define CELL_TREE         'T'
#define CELL_STONE        'R'
#define CELL_CAMP         '^'
#define CELL_PORTAL       'P'
#define CELL_HERO         '@'


// ПЕРЕЧИСЛЕНИЯ (enum)


// Направления движения в приключении
typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_QUIT
} Direction;

// Пункты главного меню
typedef enum {
    MENU_EXIT         = 0,
    MENU_CREATE_PLAYER,    // 1
    MENU_SHOW_CARD,        // 2
    MENU_TIME_MANAGE,      // 3
    MENU_INVENTORY,        // 4
    MENU_PUT_ITEM,         // 5
    MENU_DROP_ITEM,        // 6
    MENU_FAVORITE,         // 7
    MENU_SEARCH,           // 8
    MENU_SAVE_DIARY,       // 9
    MENU_CHECK_RECIPES,    // 10
    MENU_ADVENTURE,        // 11
    MENU_QUIT              // 12
} MenuItem;

// Пункты подменю времени
typedef enum {
    TIME_SHOW = 1,
    TIME_WAIT,
    TIME_SLEEP,
    TIME_BACK
} TimeAction;


// СТРУКТУРЫ ДЛЯ ВРЕМЕНИ (нужна утилитам)


typedef struct {
    int day;
    int hour;
} GameTime;

#endif // GAME_H