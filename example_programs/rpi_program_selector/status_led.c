#include "status_led.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "cyw43.h"
#include "hardware/gpio.h"

#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 25
#endif

static repeating_timer_t led_timer;
static bool led_on = false;

static bool led_timer_cb_on_pico(repeating_timer_t *rt)
{
    (void)rt;
    led_on = !led_on;
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    return true; // repeat indefinitely
}

static bool led_timer_cb_on_cyw43(repeating_timer_t *rt)
{
    (void)rt;
    led_on = !led_on;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    return true; // repeat indefinitely
}

void status_led_start(void)
{
    bool has_cyw43 = false;

    // Try to start the CYW43 chip (only present on Pico W)
    int rc = cyw43_arch_init();
    if (rc == 0) {
        // Error-checked LED access to detect the chip
        rc = cyw43_gpio_set(&cyw43_state, CYW43_WL_GPIO_LED_PIN, true);
        if (rc == 0) {
            // CYW43 present -> LED is connected to the CYW43 chip
            has_cyw43 = true;
        } else {
            // Chip not present/not started -> fallback to GPIO LED
            cyw43_arch_deinit();
        }
    }

    // Set builtin LED blinking at 1Hz via timer
    if (has_cyw43) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        add_repeating_timer_ms(500, led_timer_cb_on_cyw43, NULL, &led_timer);
    } else {
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        add_repeating_timer_ms(500, led_timer_cb_on_pico, NULL, &led_timer);
    }
}
