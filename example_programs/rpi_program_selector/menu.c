#include "menu.h"
#include "lcd1602.h"
#include "buttons.h"
#include "pico/stdlib.h"

static const menu_item_t *menu_items = NULL;
static uint8_t menu_count = 0;
static uint8_t selected_index = 0;
static uint8_t top_index = 0;

static void lcd_print_line(uint8_t row, char prefix, const char *text)
{
    char buffer[17];
    int i = 0;

    buffer[i++] = prefix;

    while (i < 16 && *text)
    {
        buffer[i++] = *text++;
    }

    while (i < 16)
    {
        buffer[i++] = ' ';
    }

    buffer[16] = '\0';

    lcd_gotoxy(row, 0);
    lcd_puts(buffer);
}

static void menu_draw(void)
{
    lcd_clear();

    // Zeile 0
    if (top_index < menu_count)
    {
        char prefix = (top_index == selected_index) ? '>' : ' ';
        lcd_print_line(0, prefix, menu_items[top_index].title);
    }
    else
    {
        lcd_print_line(0, ' ', "");
    }

    // Zeile 1
    if ((top_index + 1) < menu_count)
    {
        char prefix = ((top_index + 1) == selected_index) ? '>' : ' ';
        lcd_print_line(1, prefix, menu_items[top_index + 1].title);
    }
    else
    {
        lcd_print_line(1, ' ', "");
    }
}

void menu_init(const menu_item_t *items, uint8_t count)
{
    menu_items = items;
    menu_count = count;
    selected_index = 0;
    top_index = 0;
}

void menu_run(void)
{
    if (!menu_items || menu_count == 0)
        return;

    menu_draw();

    while (1)
    {
        if (button_up_pressed())
        {
            if (selected_index > 0)
            {
                selected_index--;

                if (selected_index < top_index)
                    top_index = selected_index;

                menu_draw();
            }
        }

        if (button_down_pressed())
        {
            if (selected_index + 1 < menu_count)
            {
                selected_index++;

                if (selected_index > top_index + 1)
                    top_index = selected_index - 1;

                menu_draw();
            }
        }

        if (button_right_pressed())
        {
            lcd_clear();

            if (menu_items[selected_index].action)
                menu_items[selected_index].action();

            menu_draw();
        }

        sleep_ms(20);
    }
}