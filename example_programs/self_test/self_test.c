#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "cyw43.h" 
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "lcd1602.h"
#include "aht10.h"
#include "weather_station.h"
#include "rgb_led.h"

#define SW_UP 2
#define SW_DOWN 3
#define SW_LEFT 4
#define SW_RIGHT 5

#define JS_BUTTON 18
#define JS_X_PIN 26
#define JS_Y_PIN 27
#define JS_X_ADC 0
#define JS_Y_ADC 1

#define BUZZER 19

#define NTC_PIN 28
#define NTC_ADC 2

#define RGB_R_PIN 20
#define RGB_G_PIN 21
#define RGB_B_PIN 22

#ifndef PICO_DEFAULT_LED_PIN
    #define PICO_DEFAULT_LED_PIN 25
#endif

static repeating_timer_t led_timer;
static bool led_on = false;

static bool led_timer_cb_on_pico(repeating_timer_t *rt)
{
    led_on = !led_on;
    gpio_put(PICO_DEFAULT_LED_PIN,led_on);
    return true; // repeat indefinitely
}

static bool led_timer_cb_on_cyw43(repeating_timer_t *rt)
{
    led_on = !led_on;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    return true; // repeat indefinitely
}

// Calculate temperature from ADC raw value (°C) using Beta model
double ntc_temp_c_from_adc(uint16_t adc_raw)
{
    const double R_fixed = 120000.0; // Ohm (Pulldown)
    const double R0 = 100000.0;      // Ohm (NTC at 25°C)
    const double Beta = 3950.0;      // K (NTC-Beta)
    const double T0 = 298.15;        // K (25°C)

    // Clamp extremes to avoid division by zero / ln(0)
    if (adc_raw <= 0)
        adc_raw = 1;
    if (adc_raw >= 4095)
        adc_raw = (uint16_t)4095 - 1;

    // Voltage divider: V = Vcc * R_fixed / (R_ntc + R_fixed)
    // -> R_ntc = R_fixed * (4095 / adc_raw - 1)
    double R_ntc = R_fixed * (4095 / (double)adc_raw - 1.0);

    // Beta-equation
    double inv_T = 1.0 / T0 + (1.0 / Beta) * log(R_ntc / R0);
    double T_c = 1.0 / inv_T - 273.15;

    return T_c;
}

int main()
{
    stdio_init_all(); // initialize core functionality

    bool has_cyw43 = false;

    // Initialise the Wi-Fi chip
    int rc = cyw43_arch_init();    // versucht den CYW43-Chip zu starten
    if (rc == 0) {
      // Fehlerprüfendes Setzen der LED:
      rc = cyw43_gpio_set(&cyw43_state, CYW43_WL_GPIO_LED_PIN, true);
      if (rc == 0) {
        // CYW43 ist vorhanden; benutze cyw43_gpio_set für die LED
        has_cyw43 = true;
      } else {
        // Fehler: Chip nicht vorhanden/nicht gestartet -> Fallback
        cyw43_arch_deinit();
        has_cyw43 = false;
      }
    } else {
        has_cyw43 = false;
      // Init fehlgeschlagen -> Fallback
    }

    // Set buildin LED blinking at 1Hz via timer
    if (has_cyw43) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        add_repeating_timer_ms(500, led_timer_cb_on_cyw43, NULL, &led_timer);
    } else {
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
        gpio_put(PICO_DEFAULT_LED_PIN,false);
        add_repeating_timer_ms(500, led_timer_cb_on_pico, NULL, &led_timer);
    }

    //----------- TEST LCD -----------------

    // init lcd and start display
    lcd_init();
    lcd_gotoxy(0, 0);
    lcd_puts("Welcome to the  "); // pad to 16 chars so display will be cleanly overriden
    lcd_gotoxy(1, 0);
    lcd_puts("self test :)    ");
    sleep_ms(1000);

    //----------- TEST DPAD ----------------

    // init GPIO for dpad
    gpio_init(SW_UP);
    gpio_init(SW_DOWN);
    gpio_init(SW_LEFT);
    gpio_init(SW_RIGHT);
    // init GPIO for dpad as inputs
    gpio_set_dir(SW_UP, GPIO_IN);
    gpio_set_dir(SW_DOWN, GPIO_IN);
    gpio_set_dir(SW_LEFT, GPIO_IN);
    gpio_set_dir(SW_RIGHT, GPIO_IN);
    // add pullups for dpad GPIO
    gpio_pull_up(SW_UP);
    gpio_pull_up(SW_DOWN);
    gpio_pull_up(SW_LEFT);
    gpio_pull_up(SW_RIGHT);

    lcd_gotoxy(0, 0);
    lcd_puts("press UP        ");
    lcd_gotoxy(1, 0);
    lcd_puts("to continue     ");
    // wait for UP button to be pressed
    while (gpio_get(SW_UP))
    {
    }

    lcd_gotoxy(0, 0);
    lcd_puts("press DOWN      ");
    lcd_gotoxy(1, 0);
    lcd_puts("to continue     ");
    // wait for DOWN button to be pressed
    while (gpio_get(SW_DOWN))
    {
    }

    lcd_gotoxy(0, 0);
    lcd_puts("press LEFT      ");
    lcd_gotoxy(1, 0);
    lcd_puts("to continue     ");
    // wait for LEFT button to be pressed
    while (gpio_get(SW_LEFT))
    {
    }

    lcd_gotoxy(0, 0);
    lcd_puts("press RIGHT     ");
    lcd_gotoxy(1, 0);
    lcd_puts("to continue     ");
    // wait for RIGHT button to be pressed
    while (gpio_get(SW_RIGHT))
    {
    }

    lcd_gotoxy(0, 0);
    lcd_puts("DPAD is working!");
    lcd_gotoxy(1, 0);
    lcd_puts("                ");
    sleep_ms(1000);

    //-------------- TEST JOYSTICK ------------

    // init adc (analogue to digital converter)
    adc_init();

    // set gpio for joystick inputs
    adc_gpio_init(JS_X_PIN);
    adc_gpio_init(JS_Y_PIN);
    gpio_init(JS_BUTTON);
    gpio_set_dir(JS_BUTTON, GPIO_IN);
    gpio_pull_up(JS_BUTTON);

    lcd_gotoxy(0, 0);
    lcd_puts("press JOYSTICK  ");
    lcd_gotoxy(1, 0);
    lcd_puts("to continue     ");
    // wait for joystick button to be pressed
    while (gpio_get(JS_BUTTON))
    {
    }

    // test joystick Y axis:
    // select joystick Y axis
    adc_select_input(JS_Y_ADC);

    lcd_gotoxy(0, 0);
    lcd_puts("move JOYSTICK   ");
    lcd_gotoxy(1, 0);
    lcd_puts("up to continue  ");
    while (adc_read() < 3800)
    {
        // uncomment for reading values
        // printf("%d\n",adc_read());
    }

    lcd_gotoxy(0, 0);
    lcd_puts("move JOYSTICK   ");
    lcd_gotoxy(1, 0);
    lcd_puts("down to continue");
    while (adc_read() > 250)
    {
        // uncomment for reading values
        // printf("%d\n",adc_read());
    }

    // test joystick X axis:
    // select joystick X axis
    adc_select_input(JS_X_ADC);

    lcd_gotoxy(0, 0);
    lcd_puts("move JOYSTICK   ");
    lcd_gotoxy(1, 0);
    lcd_puts("left to continue");
    while (adc_read() > 250)
    {
        // uncomment for reading values
        // printf("%d\n",adc_read());
    }

    lcd_gotoxy(0, 0);
    lcd_puts("move JOYSTICK   ");
    lcd_gotoxy(1, 0);
    lcd_puts("right to continue");
    while (adc_read() < 3800)
    {
        // uncomment for reading values
        // printf("%d\n",adc_read());
    }

    lcd_gotoxy(0, 0);
    lcd_puts("JOYSTICK        ");
    lcd_gotoxy(1, 0);
    lcd_puts("is working!     ");
    sleep_ms(1000);

    //------------ TEST BUZZER --------------

    // configure buzzer output
    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);

    bool buzzer_on = true;

    // turn buzzer on
    gpio_put(BUZZER, buzzer_on);

    lcd_gotoxy(0, 0);
    lcd_puts("Does the BUZZER ");
    lcd_gotoxy(1, 0);
    lcd_puts("beep? Press UP  ");

    while (gpio_get(SW_UP))
    {
        // toggle 10 times per second for nicer tone :)
        gpio_put(BUZZER, buzzer_on);
        buzzer_on = !buzzer_on;
        sleep_ms(100);
    }
    // turn buzzer of
    gpio_put(BUZZER, false);

    //------------ TEST NTC ----------------

    // init NTC (negative Temperature Coefficient) for ADC use
    adc_gpio_init(NTC_PIN);

    // select the NTC to be sampled by the ADC
    adc_select_input(NTC_ADC);

    lcd_gotoxy(0, 0);
    lcd_puts("Test NTC temp-  ");
    lcd_gotoxy(1, 0);
    lcd_puts("erature Sensor  ");
    sleep_ms(1000);

    // wait for user input
    while (gpio_get(SW_UP))
    {
        lcd_gotoxy(0, 0);
        lcd_puts("Press up if temp");
        lcd_gotoxy(1, 0);

        // sample NTC 16 times
        const int samples = 16;
        uint32_t sum = 0;
        for (int i = 0; i < samples; ++i)
        {
            sum += adc_read(); // 12-bit, 0..4095
        }
        uint16_t raw = (uint16_t)(sum / samples);
        double t_c = ntc_temp_c_from_adc(raw);

        printf("ADC=%u, T=%.2f C\n", raw, t_c);

        // format the output for a nice layout
        int len = snprintf(NULL, 0, "ok: %.2f%CC    ", t_c, 0xDF);
        char *result = malloc(len + 1);
        snprintf(result, len + 1, "ok: %.2f%cC    ", t_c, 0xDF);
        lcd_puts(result);
        free(result);

        sleep_ms(100);
    }

    //------------ TEST AHT10 --------------

    lcd_gotoxy(0, 0);
    lcd_puts("Test            ");
    lcd_gotoxy(1, 0);
    lcd_puts("Weather Station ");
    sleep_ms(1000);

    aht10_t sensor;
    if (!aht10_begin(&sensor))
    {
        while (1)
            tight_loop_contents();
    }

    while (gpio_get(SW_UP))
    {
        for (int i = 0; i < 40; i++)
        {
            if (!gpio_get(SW_UP)){break;}
            display_tmp_and_humid(sensor);
            sleep_ms(100);
        }
        if (!gpio_get(SW_UP)){break;}

        lcd_gotoxy(0, 0);
        lcd_puts("If values look  ");
        lcd_gotoxy(1, 0);
        lcd_puts("ok press up:    ");

        for (int i = 0; i < 10; i++) {
            if (!gpio_get(SW_UP)){break;}
            sleep_ms(100);
        }
    }

    //------------ TEST RGB LED ------------

    lcd_gotoxy(0, 0);
    lcd_puts("TEST RGB LED    ");
    lcd_gotoxy(1, 0);
    lcd_puts("                ");
    sleep_ms(1000);

    gpio_init(RGB_R_PIN);
    gpio_init(RGB_G_PIN);
    gpio_init(RGB_B_PIN);
    gpio_set_dir(RGB_R_PIN, GPIO_OUT);
    gpio_set_dir(RGB_G_PIN, GPIO_OUT);
    gpio_set_dir(RGB_B_PIN, GPIO_OUT);

    // turn red led on
    gpio_put(RGB_R_PIN, true);

    lcd_gotoxy(0, 0);
    lcd_puts("Is led red      ");
    lcd_gotoxy(1, 0);
    lcd_puts("then press up.  ");
    while (gpio_get(SW_UP))
    {
    }
    gpio_put(RGB_R_PIN, false);

    // turn green led on
    gpio_put(RGB_G_PIN, true);
    sleep_ms(1000);
    lcd_gotoxy(0, 0);
    lcd_puts("Is led green    ");
    lcd_gotoxy(1, 0);
    lcd_puts("then press up.  ");
    while (gpio_get(SW_UP))
    {
    }
    gpio_put(RGB_G_PIN, false);

    // turn blue led on
    gpio_put(RGB_B_PIN, true);
    sleep_ms(1000);
    lcd_gotoxy(0, 0);
    lcd_puts("Is led blue     ");
    lcd_gotoxy(1, 0);
    lcd_puts("then press up.  ");
    while (gpio_get(SW_UP))
    {
    }
    gpio_put(RGB_B_PIN, false);

    //-------- CELEBRATION --------

    lcd_gotoxy(0, 0);
    lcd_puts("Congratulations:");
    lcd_gotoxy(1, 0);
    lcd_puts("board is ready! ");

    // goto rainbow routine
    start_rainbow_no_return();
    // this function will never return!!!
}
