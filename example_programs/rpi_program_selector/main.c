#include "pico/stdlib.h"
#include "lcd1602.h"
#include "buttons.h"
#include "menu.h"

#include "app_weather.h"
#include "app_breakout.h"
#include "app_sidescroller.h"
#include "app_snake.h"
#include "app_tetris.h"
#include "app_selftest.h"

int main()
{
    stdio_init_all();
    lcd_init();
    buttons_init();

    const menu_item_t main_menu[] = {
        {"Selbstest",      app_selftest_start},
        {"Wetterstation", app_weather_start},
        {"Breakout",      app_breakout_start},
        {"Sidescroller",  app_sidescroller_start},
        {"Snake",         app_snake_start},
        {"Tetris",        app_tetris_start}
    };

    menu_init(main_menu, sizeof(main_menu) / sizeof(main_menu[0]));
    menu_run();

    return 0;
}