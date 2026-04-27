#include "app_selftest.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "lcd1602.h"
#include "aht10.h"
#include "weather_station.h"
#include "rgb_led.h"

// Buttons
#define SW_UP    2
#define SW_DOWN  3
#define SW_LEFT  4
#define SW_RIGHT 5

// Joystick
#define JS_BUTTON 18
#define JS_X_PIN  26
#define JS_Y_PIN  27
#define JS_X_ADC  0
#define JS_Y_ADC  1

// Other hardware
#define BUZZER    19
#define NTC_PIN   28
#define NTC_ADC   2

#define RGB_R_PIN 20
#define RGB_G_PIN 21
#define RGB_B_PIN 22

#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 25
#endif

static repeating_timer_t led_timer;
static bool led_on = false;

static bool led_timer_cb(repeating_timer_t *rt)
{
    (void)rt;
    led_on = !led_on;
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    return true;
}

static void lcd_write_2lines(const char *l0, const char *l1)
{
    lcd_gotoxy(0, 0);
    lcd_puts(l0);
    lcd_gotoxy(1, 0);
    lcd_puts(l1);
}

static void wait_release(uint pin)
{
    while (!gpio_get(pin))
    {
        sleep_ms(10);
    }
    sleep_ms(50);
}

static void wait_press(uint pin)
{
    while (gpio_get(pin))
    {
        sleep_ms(10);
    }
    sleep_ms(20);
}

static void wait_press_and_release(uint pin)
{
    wait_press(pin);
    wait_release(pin);
}

static void init_dpad(void)
{
    const uint pins[] = {SW_UP, SW_DOWN, SW_LEFT, SW_RIGHT};

    for (int i = 0; i < 4; i++)
    {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], GPIO_IN);
        gpio_pull_up(pins[i]);
    }
}

static void init_joystick(void)
{
    adc_init();
    adc_gpio_init(JS_X_PIN);
    adc_gpio_init(JS_Y_PIN);

    gpio_init(JS_BUTTON);
    gpio_set_dir(JS_BUTTON, GPIO_IN);
    gpio_pull_up(JS_BUTTON);
}

static void init_buzzer(void)
{
    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_put(BUZZER, 0);
}

static void init_rgb_gpio(void)
{
    gpio_init(RGB_R_PIN);
    gpio_init(RGB_G_PIN);
    gpio_init(RGB_B_PIN);

    gpio_set_dir(RGB_R_PIN, GPIO_OUT);
    gpio_set_dir(RGB_G_PIN, GPIO_OUT);
    gpio_set_dir(RGB_B_PIN, GPIO_OUT);

    gpio_put(RGB_R_PIN, 0);
    gpio_put(RGB_G_PIN, 0);
    gpio_put(RGB_B_PIN, 0);
}

static double ntc_temp_c_from_adc(uint16_t adc_raw)
{
    const double R_fixed = 120000.0;
    const double R0 = 100000.0;
    const double Beta = 3950.0;
    const double T0 = 298.15;

    if (adc_raw <= 0) adc_raw = 1;
    if (adc_raw >= 4095) adc_raw = 4094;

    double R_ntc = R_fixed * (4095 / (double)adc_raw - 1.0);
    double inv_T = 1.0 / T0 + (1.0 / Beta) * log(R_ntc / R0);
    double T_c = 1.0 / inv_T - 273.15;

    return T_c;
}

static void selftest_led_blink_start(void)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);

    add_repeating_timer_ms(500, led_timer_cb, NULL, &led_timer);
}

static void selftest_led_blink_stop(void)
{
    cancel_repeating_timer(&led_timer);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
}

static void test_dpad(void)
{
    lcd_write_2lines("press UP        ", "to continue     ");
    wait_press_and_release(SW_UP);

    lcd_write_2lines("press DOWN      ", "to continue     ");
    wait_press_and_release(SW_DOWN);

    lcd_write_2lines("press LEFT      ", "to continue     ");
    wait_press_and_release(SW_LEFT);

    lcd_write_2lines("press RIGHT     ", "to continue     ");
    wait_press_and_release(SW_RIGHT);

    lcd_write_2lines("DPAD is working!", "                ");
    sleep_ms(1000);
}

static void test_joystick(void)
{
    lcd_write_2lines("press JOYSTICK  ", "to continue     ");
    wait_press_and_release(JS_BUTTON);

    adc_select_input(JS_Y_ADC);
    lcd_write_2lines("move JOYSTICK   ", "up to continue  ");
    while (adc_read() < 3800) sleep_ms(10);

    lcd_write_2lines("move JOYSTICK   ", "down to continue");
    while (adc_read() > 250) sleep_ms(10);

    adc_select_input(JS_X_ADC);
    lcd_write_2lines("move JOYSTICK   ", "left to continue");
    while (adc_read() > 250) sleep_ms(10);

    lcd_write_2lines("move JOYSTICK   ", "right continue  ");
    while (adc_read() < 3800) sleep_ms(10);

    lcd_write_2lines("JOYSTICK        ", "is working!     ");
    sleep_ms(1000);
}

static void test_buzzer(void)
{
    bool buzzer_on = true;

    lcd_write_2lines("Does BUZZER beep", "UP if yes       ");

    while (gpio_get(SW_UP))
    {
        gpio_put(BUZZER, buzzer_on);
        buzzer_on = !buzzer_on;
        sleep_ms(100);
    }

    gpio_put(BUZZER, 0);
    wait_release(SW_UP);
}

static void test_ntc(void)
{
    adc_gpio_init(NTC_PIN);
    adc_select_input(NTC_ADC);

    lcd_write_2lines("Test NTC temp   ", "Sensor          ");
    sleep_ms(1000);

    while (gpio_get(SW_UP))
    {
        lcd_gotoxy(0, 0);
        lcd_puts("Press UP if temp");
        lcd_gotoxy(1, 0);

        const int samples = 16;
        uint32_t sum = 0;
        for (int i = 0; i < samples; ++i)
        {
            sum += adc_read();
        }

        uint16_t raw = (uint16_t)(sum / samples);
        double t_c = ntc_temp_c_from_adc(raw);

        char line[17];
        snprintf(line, sizeof(line), "ok: %.2f%cC    ", t_c, 0xDF);
        lcd_puts(line);

        sleep_ms(100);
    }

    wait_release(SW_UP);
}

static void test_aht10(void)
{
    lcd_write_2lines("Test            ", "Weather Station ");
    sleep_ms(1000);

    aht10_t sensor;
    if (!aht10_begin(&sensor))
    {
        lcd_write_2lines("AHT10 FAIL      ", "UP = skip       ");
        wait_press_and_release(SW_UP);
        return;
    }

    while (gpio_get(SW_UP))
    {
        for (int i = 0; i < 40; i++)
        {
            if (!gpio_get(SW_UP)) break;
            display_tmp_and_humid(sensor);
            sleep_ms(100);
        }

        if (!gpio_get(SW_UP)) break;

        lcd_write_2lines("If values look  ", "ok press up     ");

        for (int i = 0; i < 10; i++)
        {
            if (!gpio_get(SW_UP)) break;
            sleep_ms(100);
        }
    }

    wait_release(SW_UP);
}

static void test_rgb(void)
{
    lcd_write_2lines("TEST RGB LED    ", "                ");
    sleep_ms(1000);

    gpio_put(RGB_R_PIN, 1);
    lcd_write_2lines("Is led red      ", "then press up.  ");
    wait_press_and_release(SW_UP);
    gpio_put(RGB_R_PIN, 0);

    gpio_put(RGB_G_PIN, 1);
    lcd_write_2lines("Is led green    ", "then press up.  ");
    wait_press_and_release(SW_UP);
    gpio_put(RGB_G_PIN, 0);

    gpio_put(RGB_B_PIN, 1);
    lcd_write_2lines("Is led blue     ", "then press up.  ");
    wait_press_and_release(SW_UP);
    gpio_put(RGB_B_PIN, 0);
}

void app_selftest_start(void)
{
    selftest_led_blink_start();

    lcd_clear();
    lcd_write_2lines("Welcome to the  ", "self test :)    ");
    sleep_ms(1000);

    init_dpad();
    init_joystick();
    init_buzzer();
    init_rgb_gpio();

    test_dpad();
    test_joystick();
    test_buzzer();
    test_ntc();
    test_aht10();
    test_rgb();

    selftest_led_blink_stop();

    lcd_write_2lines("Congratulations:", "board is ready! ");
    sleep_ms(1500);

    start_rainbow_no_return();
}