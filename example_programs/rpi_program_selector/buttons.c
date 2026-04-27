#include "buttons.h"
#include "pico/stdlib.h"

#define BTN_UP    2
#define BTN_DOWN  3
#define BTN_LEFT  4
#define BTN_RIGHT 5

static bool debounce_press(uint pin)
{
    if (gpio_get(pin) == 0)
    {
        sleep_ms(20);
        if (gpio_get(pin) == 0)
        {
            while (gpio_get(pin) == 0)
            {
                sleep_ms(10);
            }
            sleep_ms(20);
            return true;
        }
    }
    return false;
}

void buttons_init(void)
{
    const uint pins[] = {BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT};

    for (int i = 0; i < 4; i++)
    {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], GPIO_IN);
        gpio_pull_up(pins[i]);
    }
}

bool button_up_pressed(void)
{
    return debounce_press(BTN_UP);
}

bool button_down_pressed(void)
{
    return debounce_press(BTN_DOWN);
}

bool button_left_pressed(void)
{
    return debounce_press(BTN_LEFT);
}

bool button_right_pressed(void)
{
    return debounce_press(BTN_RIGHT);
}