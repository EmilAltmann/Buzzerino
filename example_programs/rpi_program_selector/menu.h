#ifndef MENU_H
#define MENU_H

#include <stdint.h>

typedef void (*menu_action_t)(void);

typedef struct
{
    const char *title;
    menu_action_t action;
} menu_item_t;

void menu_init(const menu_item_t *items, uint8_t count);
void menu_run(void);

#endif