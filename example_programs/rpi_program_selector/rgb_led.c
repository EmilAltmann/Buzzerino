#include "rgb_led.h"

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <math.h>

#define PIN_R 20
#define PIN_G 21
#define PIN_B 22

#define PWM_TOP 255
#define PWM_DIV 64.0f

static inline uint16_t apply_gamma(float x, float gamma, float brightness)
{
    if (x < 0) x = 0;
    if (x > 1) x = 1;

    float y = powf(x, gamma) * brightness;
    if (y > 1) y = 1;

    return (uint16_t)lroundf(y * PWM_TOP);
}

static void hsv_to_rgb(float h, float s, float v, float *r, float *g, float *b)
{
    float c = v * s;
    float hp = fmodf(h / 60.0f, 6.0f);
    float x = c * (1.0f - fabsf(fmodf(hp, 2.0f) - 1.0f));
    float m = v - c;
    float rr, gg, bb;

    if (hp < 1)      { rr = c; gg = x; bb = 0; }
    else if (hp < 2) { rr = x; gg = c; bb = 0; }
    else if (hp < 3) { rr = 0; gg = c; bb = x; }
    else if (hp < 4) { rr = 0; gg = x; bb = c; }
    else if (hp < 5) { rr = x; gg = 0; bb = c; }
    else             { rr = c; gg = 0; bb = x; }

    *r = rr + m;
    *g = gg + m;
    *b = bb + m;
}

static void pwm_init_pin(uint pin)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice, PWM_TOP);
    pwm_set_clkdiv(slice, PWM_DIV);
    pwm_set_enabled(slice, true);
}

static void set_rgb(float r, float g, float b, float gamma, float brightness)
{
    uint slice_r = pwm_gpio_to_slice_num(PIN_R);
    uint chan_r  = pwm_gpio_to_channel(PIN_R);
    uint slice_g = pwm_gpio_to_slice_num(PIN_G);
    uint chan_g  = pwm_gpio_to_channel(PIN_G);
    uint slice_b = pwm_gpio_to_slice_num(PIN_B);
    uint chan_b  = pwm_gpio_to_channel(PIN_B);

    uint16_t lr = apply_gamma(r, gamma, brightness);
    uint16_t lg = apply_gamma(g, gamma, brightness);
    uint16_t lb = apply_gamma(b, gamma, brightness);

    pwm_set_chan_level(slice_r, chan_r, lr);
    pwm_set_chan_level(slice_g, chan_g, lg);
    pwm_set_chan_level(slice_b, chan_b, lb);
}

void start_rainbow_no_return(void)
{
    pwm_init_pin(PIN_R);
    pwm_init_pin(PIN_G);
    pwm_init_pin(PIN_B);

    float brightness = 0.5f;
    float gamma = 2.2f;
    float hue = 0.0f;

    while (1)
    {
        float r, g, b;
        hsv_to_rgb(hue, 1.0f, 1.0f, &r, &g, &b);
        set_rgb(r, g, b, gamma, brightness);

        hue += 1.0f;
        if (hue >= 360.0f) hue -= 360.0f;

        sleep_ms(15);
    }
}