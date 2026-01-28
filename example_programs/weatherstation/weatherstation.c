#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "lcd1602.h"
#include "aht10.h"

// RGB Pins (Buzzerino)
#define RGB_R_PIN 20
#define RGB_G_PIN 21
#define RGB_B_PIN 22

// Setze auf 1 wenn deine RGB LED common-anode ist (Farben sonst "falsch"/invertiert)
#define RGB_INVERT 0

static inline float clampf(float x, float a, float b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}
static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

typedef struct {
    uint slice;
    uint chan;
} pwm_pin_t;

static pwm_pin_t pwm_r, pwm_g, pwm_b;

static pwm_pin_t pwm_setup_pin(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    uint chan  = pwm_gpio_to_channel(gpio);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 4.0f);
    pwm_config_set_wrap(&cfg, 65535);
    pwm_init(slice, &cfg, true);

    pwm_pin_t p = {slice, chan};
    return p;
}

static void pwm_write(pwm_pin_t p, uint16_t level) {
    pwm_set_chan_level(p.slice, p.chan, level);
}

static void rgb_set(float r, float g, float b) {
    r = clampf(r, 0.f, 1.f);
    g = clampf(g, 0.f, 1.f);
    b = clampf(b, 0.f, 1.f);

    uint16_t R = (uint16_t)(r * 65535.0f);
    uint16_t G = (uint16_t)(g * 65535.0f);
    uint16_t B = (uint16_t)(b * 65535.0f);

#if RGB_INVERT
    R = 65535 - R; G = 65535 - G; B = 65535 - B;
#endif

    pwm_write(pwm_r, R);
    pwm_write(pwm_g, G);
    pwm_write(pwm_b, B);
}

// ---------- Heat Index (NOAA), Eingabe in °C, RH% ----------
static float heat_index_c(float t_c, float rh) {
    float t_f = t_c * 9.0f/5.0f + 32.0f;
    float R = rh;

    float hi_f =
        -42.379f + 2.04901523f*t_f + 10.14333127f*R
        - 0.22475541f*t_f*R - 0.00683783f*t_f*t_f
        - 0.05481717f*R*R + 0.00122874f*t_f*t_f*R
        + 0.00085282f*t_f*R*R - 0.00000199f*t_f*t_f*R*R;

    return (hi_f - 32.0f) * 5.0f/9.0f;
}

static float feels_like_c(float t_c, float rh) {
    if (t_c >= 27.0f && rh >= 40.0f) return heat_index_c(t_c, rh);
    return t_c;
}

// ---------- Biowetter-Farbskala ----------
static void color_from_feels(float f, float *r, float *g, float *b) {
    // Blau: sehr kalt
    // Grün: moderat
    // Orange/Rot: heiß
    if (f <= 5.0f) { *r=0; *g=0; *b=1; return; }

    if (f < 20.0f) {
        float t = (f - 5.0f) / 15.0f; // 0..1
        *r = 0.0f;
        *g = t;
        *b = 1.0f - t;
        return;
    }

    if (f < 28.0f) { *r=0; *g=1; *b=0; return; }

    if (f < 35.0f) {
        float t = (f - 28.0f) / 7.0f;
        *r = lerp(0.0f, 1.0f, t);
        *g = lerp(1.0f, 0.4f, t); // wird orange
        *b = 0.0f;
        return;
    }

    *r=1; *g=0; *b=0;
}

int main() {
    stdio_init_all();

    lcd_init();
    lcd_gotoxy(0,0); lcd_puts("Bioweather init ");
    lcd_gotoxy(1,0); lcd_puts("AHT10...        ");

    // Sensor init (deine Klasse)
    aht10_t dev;
    if (!aht10_begin(&dev)) {
        lcd_gotoxy(1,0); lcd_puts("AHT10 FAIL      ");
        while (1) tight_loop_contents();
    }

    // RGB PWM init
    pwm_r = pwm_setup_pin(RGB_R_PIN);
    pwm_g = pwm_setup_pin(RGB_G_PIN);
    pwm_b = pwm_setup_pin(RGB_B_PIN);

    while (1) {
        float t = 0.0f, rh = 0.0f;
        if (!aht10_read(&dev, &t, &rh)) {
            lcd_gotoxy(0,0); lcd_puts("AHT10 read FAIL ");
            lcd_gotoxy(1,0); lcd_puts("                ");
            rgb_set(0,0,0);
            sleep_ms(500);
            continue;
        }

        float f = feels_like_c(t, rh);

        float r,g,b;
        color_from_feels(f, &r, &g, &b);
        rgb_set(r,g,b);

        // LCD-Ausgabe
        char l0[17], l1[17];
        // l0: T und RH
        snprintf(l0, sizeof(l0), "T:%4.1f%cC RH:%3.0f%c", t, 0xDF, rh,0x25);
        // l1: feels-like
        snprintf(l1, sizeof(l1), "Feel:%4.1f%cC       ", f, 0xDF);
        l0[16]=0; l1[16]=0;

        lcd_gotoxy(0,0); lcd_puts(l0);
        lcd_gotoxy(1,0); lcd_puts(l1);

        sleep_ms(1000);
    }
}