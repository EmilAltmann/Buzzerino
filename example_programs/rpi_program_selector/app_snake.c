#include "app_snake.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "lcd1602.h"

// D-Pad Pins (Buzzerino), active-low mit Pullups
#define SW_UP        2
#define SW_DOWN      3
#define SW_LEFT      4
#define SW_RIGHT     5
#define JS_BUTTON    18

// "Subpixel"-Grid: 16 Spalten, 6 Reihen
#define W 16
#define H 6

#define TICK_MS 180

typedef struct
{
    uint8_t x, y;
} pt_t;

typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} dir_t;

// ---------- 3-band charset ----------
static void make_3band_char(uint8_t mask3, uint8_t out[8])
{
    for (int i = 0; i < 8; i++) out[i] = 0x00;

    if (mask3 & 0x1) { out[0] = 0x1F; out[1] = 0x1F; }
    if (mask3 & 0x2) { out[3] = 0x1F; out[4] = 0x1F; }
    if (mask3 & 0x4) { out[6] = 0x1F; out[7] = 0x1F; }
}

static void load_subpixel_charset(void)
{
    uint8_t bmp[8];

    for (uint8_t m = 0; m < 8; m++)
    {
        make_3band_char(m, bmp);
        lcd_create_char(m, bmp);
    }
}

// ---------- framebuffer ----------
static uint8_t fb0[W];
static uint8_t fb1[W];

static inline void fb_clear(void)
{
    memset(fb0, 0, sizeof(fb0));
    memset(fb1, 0, sizeof(fb1));
}

static inline void fb_set_cell(uint8_t x, uint8_t y)
{
    if (x >= W || y >= H) return;

    if (y < 3)
    {
        uint8_t bit = (y == 0) ? 0x1 : (y == 1) ? 0x2 : 0x4;
        fb0[x] |= bit;
    }
    else
    {
        uint8_t y2 = y - 3;
        uint8_t bit = (y2 == 0) ? 0x1 : (y2 == 1) ? 0x2 : 0x4;
        fb1[x] |= bit;
    }
}

static inline void fb_or_band(uint8_t x, uint8_t lcd_row, uint8_t band)
{
    if (x >= W) return;

    uint8_t bit = (band == 0) ? 0x1 : (band == 1) ? 0x2 : 0x4;

    if (lcd_row == 0) fb0[x] |= bit;
    else              fb1[x] |= bit;
}

static void fb_render(void)
{
    lcd_gotoxy(0, 0);
    for (int x = 0; x < W; x++) lcd_putc((char)(fb0[x] & 0x7));

    lcd_gotoxy(1, 0);
    for (int x = 0; x < W; x++) lcd_putc((char)(fb1[x] & 0x7));
}

// ---------- RNG ----------
static inline uint32_t rng_u32(void)
{
    static uint32_t s = 0;
    if (!s) s = time_us_32() ^ 0xA5A5A5A5u;

    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    return s;
}

static inline uint8_t rand_u8(uint8_t max_exclusive)
{
    return (uint8_t)(rng_u32() % max_exclusive);
}

// ---------- snake ----------
#define SNAKE_MAX 96

static pt_t snake[SNAKE_MAX];
static int snake_len = 4;
static int score = 0;

static pt_t apple;
static int apple_active = 0;

static inline int is_opposite(dir_t a, dir_t b)
{
    return (a == DIR_UP    && b == DIR_DOWN) ||
           (a == DIR_DOWN  && b == DIR_UP)   ||
           (a == DIR_LEFT  && b == DIR_RIGHT)||
           (a == DIR_RIGHT && b == DIR_LEFT);
}

static int snake_contains(uint8_t x, uint8_t y)
{
    for (int i = 0; i < snake_len; i++)
    {
        if (snake[i].x == x && snake[i].y == y) return 1;
    }
    return 0;
}

static void place_apple(void)
{
    for (int tries = 0; tries < 80; tries++)
    {
        uint8_t x = rand_u8(W);
        uint8_t y = rand_u8(H);

        if (!snake_contains(x, y))
        {
            apple.x = x;
            apple.y = y;
            apple_active = 1;
            return;
        }
    }

    for (uint8_t y = 0; y < H; y++)
    {
        for (uint8_t x = 0; x < W; x++)
        {
            if (!snake_contains(x, y))
            {
                apple.x = x;
                apple.y = y;
                apple_active = 1;
                return;
            }
        }
    }

    apple_active = 0;
}

static void snake_reset(void)
{
    snake_len = 4;
    score = 0;

    uint8_t sx = 6, sy = 2;
    for (int i = 0; i < snake_len; i++)
    {
        snake[i].x = sx + i;
        snake[i].y = sy;
    }

    place_apple();
}

static int snake_step(dir_t dir)
{
    pt_t head = snake[snake_len - 1];
    pt_t nh = head;

    switch (dir)
    {
        case DIR_UP:    nh.y--; break;
        case DIR_DOWN:  nh.y++; break;
        case DIR_LEFT:  nh.x--; break;
        case DIR_RIGHT: nh.x++; break;
    }

    if (nh.x >= W || nh.y >= H) return 0;

    int ate = (apple_active && nh.x == apple.x && nh.y == apple.y);

    for (int i = 0; i < snake_len; i++)
    {
        if (!ate && i == 0) continue;
        if (snake[i].x == nh.x && snake[i].y == nh.y) return 0;
    }

    if (ate)
    {
        if (snake_len < SNAKE_MAX)
        {
            snake[snake_len] = nh;
            snake_len++;
        }
        else
        {
            memmove(&snake[0], &snake[1], sizeof(pt_t) * (snake_len - 1));
            snake[snake_len - 1] = nh;
        }

        score++;
        place_apple();
    }
    else
    {
        memmove(&snake[0], &snake[1], sizeof(pt_t) * (snake_len - 1));
        snake[snake_len - 1] = nh;
    }

    return 1;
}

static void draw_frame(void)
{
    fb_clear();

    for (int i = 0; i < snake_len; i++)
        fb_set_cell(snake[i].x, snake[i].y);

    if (apple_active)
    {
        uint8_t lcd_row = (apple.y < 3) ? 0 : 1;
        uint8_t band    = (uint8_t)(apple.y % 3);
        fb_or_band(apple.x, lcd_row, band);
    }

    fb_render();
}

// ---------- interrupt input ----------
static volatile dir_t g_dir_req = DIR_RIGHT;
static volatile uint32_t g_last_irq_us = 0;
static volatile uint32_t g_btn_events = 0;

#define EV_UP       (1u << 0)
#define EV_DOWN     (1u << 1)
#define EV_LEFT     (1u << 2)
#define EV_RIGHT    (1u << 3)
#define EV_BACK     (1u << 4)

static void snake_gpio_irq(uint gpio, uint32_t events)
{
    (void)events;

    uint32_t now = time_us_32();
    if ((uint32_t)(now - g_last_irq_us) < 70000u) return;
    g_last_irq_us = now;

    if      (gpio == SW_UP)     { g_dir_req = DIR_UP;    g_btn_events |= EV_UP; }
    else if (gpio == SW_DOWN)   { g_dir_req = DIR_DOWN;  g_btn_events |= EV_DOWN; }
    else if (gpio == SW_LEFT)   { g_dir_req = DIR_LEFT;  g_btn_events |= EV_LEFT; }
    else if (gpio == SW_RIGHT)  { g_dir_req = DIR_RIGHT; g_btn_events |= EV_RIGHT; }
    else if (gpio == JS_BUTTON) { g_btn_events |= EV_BACK; }
}

static void input_init_irqs(void)
{
    gpio_init(SW_UP);
    gpio_set_dir(SW_UP, GPIO_IN);
    gpio_pull_up(SW_UP);

    gpio_init(SW_DOWN);
    gpio_set_dir(SW_DOWN, GPIO_IN);
    gpio_pull_up(SW_DOWN);

    gpio_init(SW_LEFT);
    gpio_set_dir(SW_LEFT, GPIO_IN);
    gpio_pull_up(SW_LEFT);

    gpio_init(SW_RIGHT);
    gpio_set_dir(SW_RIGHT, GPIO_IN);
    gpio_pull_up(SW_RIGHT);

    gpio_init(JS_BUTTON);
    gpio_set_dir(JS_BUTTON, GPIO_IN);
    gpio_pull_up(JS_BUTTON);

    gpio_set_irq_enabled_with_callback(SW_UP, GPIO_IRQ_EDGE_FALL, true, &snake_gpio_irq);
    gpio_set_irq_enabled(SW_DOWN,    GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(SW_LEFT,    GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(SW_RIGHT,   GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(JS_BUTTON,  GPIO_IRQ_EDGE_FALL, true);
}

static void input_deinit_irqs(void)
{
    gpio_set_irq_enabled(SW_UP,     GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(SW_DOWN,   GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(SW_LEFT,   GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(SW_RIGHT,  GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(JS_BUTTON, GPIO_IRQ_EDGE_FALL, false);
}

static void show_game_over(int score_val)
{
    char line0[17];
    char line1[17];

    snprintf(line0, sizeof(line0), "   GAME OVER    ");
    snprintf(line1, sizeof(line1), "Score:%6d", score_val);

    int len = (int)strlen(line1);
    while (len < 16) line1[len++] = ' ';
    line1[16] = '\0';

    lcd_gotoxy(0, 0);
    lcd_puts(line0);
    lcd_gotoxy(1, 0);
    lcd_puts(line1);
}

void app_snake_start(void)
{
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Snake           ");
    lcd_gotoxy(1, 0);
    lcd_puts("JS Btn = Back   ");
    sleep_ms(1000);

    load_subpixel_charset();

    g_dir_req = DIR_RIGHT;
    g_last_irq_us = 0;
    g_btn_events = 0;

    input_init_irqs();

    snake_reset();
    dir_t dir = DIR_RIGHT;

    absolute_time_t next = make_timeout_time_ms(TICK_MS);

    while (1)
    {
        if (g_btn_events & EV_BACK)
        {
            input_deinit_irqs();
            g_btn_events = 0;
            lcd_clear();
            return;
        }

        dir_t req = g_dir_req;
        if (!is_opposite(dir, req))
            dir = req;

        if (!snake_step(dir))
        {
            show_game_over(score);

            g_btn_events = 0;

            while (1)
            {
                if (g_btn_events & EV_BACK)
                {
                    input_deinit_irqs();
                    g_btn_events = 0;
                    lcd_clear();
                    return;
                }

                if (g_btn_events & EV_UP)
                {
                    snake_reset();
                    dir = DIR_RIGHT;
                    g_dir_req = DIR_RIGHT;
                    g_btn_events = 0;
                    sleep_ms(200);
                    break;
                }

                sleep_ms(10);
            }
        }

        draw_frame();

        sleep_until(next);
        next = delayed_by_ms(next, TICK_MS);
    }
}