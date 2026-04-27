#include "app_sidescroller.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "lcd1602.h"

// Buttons (Buzzerino)
#define SW_UP    2
#define SW_LEFT  4
#define BUZZER   19

#define LCD_COLS 16
#define PLAYER_X 1
#define TICK_MS  90

// Custom Char Slots (0..7)
enum
{
    CH_DINO_A = 0,
    CH_DINO_B = 1,
    CH_CACTUS = 2,
    CH_ROCK   = 3,
    CH_PTERO  = 4,
    CH_GROUND = 5,
};

static const uint8_t dino_a[8] = {
    0b00100, 0b01110, 0b00111, 0b01110, 0b11100, 0b00100, 0b01010, 0b10001
};

static const uint8_t dino_b[8] = {
    0b00100, 0b01110, 0b00111, 0b01110, 0b11100, 0b00100, 0b10010, 0b01001
};

static const uint8_t cactus[8] = {
    0b00100, 0b00100, 0b10101, 0b10101, 0b01110, 0b00100, 0b00100, 0b00100
};

static const uint8_t rock[8] = {
    0b00000, 0b00000, 0b00110, 0b01111, 0b11111, 0b11110, 0b01100, 0b00000
};

static const uint8_t ptero[8] = {
    0b00000, 0b00000, 0b10001, 0b01010, 0b00100, 0b01110, 0b00000, 0b00000
};

static const uint8_t ground[8] = {
    0, 0, 0, 0, 0, 0, 0, 0b11111
};

typedef enum
{
    OB_NONE = 0,
    OB_CACTUS,
    OB_ROCK,
    OB_PTERO
} ob_type_t;

typedef struct
{
    int x;
    int row;   // 0 oder 1
    ob_type_t t;
    int active;
} obstacle_t;

typedef struct
{
    obstacle_t obs[2];
    int spawn_in;
    int jump_ticks;
    int anim;
    int score;
    int game_over;
} game_t;

static inline void buzzer_beep_ms(int ms)
{
    gpio_put(BUZZER, 1);
    sleep_ms(ms);
    gpio_put(BUZZER, 0);
}

// RNG (xorshift32)
static inline uint32_t rng_u32(void)
{
    static uint32_t s = 0;
    if (s == 0) s = time_us_32() ^ 0xA5A5A5A5;

    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    return s;
}

static int rand_range(int a, int b)
{
    return a + (rng_u32() % (uint32_t)(b - a + 1));
}

static void load_sprites(void)
{
    lcd_create_char(CH_DINO_A, dino_a);
    lcd_create_char(CH_DINO_B, dino_b);
    lcd_create_char(CH_CACTUS, cactus);
    lcd_create_char(CH_ROCK, rock);
    lcd_create_char(CH_PTERO, ptero);
    lcd_create_char(CH_GROUND, ground);
}

static void game_input_init(void)
{
    gpio_init(SW_UP);
    gpio_set_dir(SW_UP, GPIO_IN);
    gpio_pull_up(SW_UP);

    gpio_init(SW_LEFT);
    gpio_set_dir(SW_LEFT, GPIO_IN);
    gpio_pull_up(SW_LEFT);

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_put(BUZZER, 0);
}

static void reset_game(game_t *g)
{
    memset(g, 0, sizeof(*g));

    for (int i = 0; i < 2; i++)
        g->obs[i].active = 0;

    g->spawn_in = rand_range(8, 14);
}

static uint8_t char_for_obstacle(const obstacle_t *o)
{
    switch (o->t)
    {
        case OB_CACTUS: return CH_CACTUS;
        case OB_ROCK:   return CH_ROCK;
        case OB_PTERO:  return CH_PTERO;
        default:        return ' ';
    }
}

static void spawn_obstacle(game_t *g)
{
    int slot = -1;

    for (int i = 0; i < 2; i++)
    {
        if (!g->obs[i].active)
        {
            slot = i;
            break;
        }
    }

    if (slot < 0) return;

    obstacle_t *o = &g->obs[slot];
    o->active = 1;
    o->x = LCD_COLS - 1;

    uint32_t r = rng_u32() % 10;

    if (r < 5)
    {
        o->t = OB_CACTUS;
        o->row = 1;
    }
    else if (r < 8)
    {
        o->t = OB_ROCK;
        o->row = 1;
    }
    else
    {
        o->t = OB_PTERO;
        o->row = 0;
    }

    int min_gap = 6;
    int max_gap = 12;

    if (g->score > 400)
    {
        min_gap = 5;
        max_gap = 10;
    }

    if (g->score > 800)
    {
        min_gap = 4;
        max_gap = 8;
    }

    g->spawn_in = rand_range(min_gap, max_gap);
}

static void render(const game_t *g)
{
    uint8_t row0[LCD_COLS], row1[LCD_COLS];

    for (int i = 0; i < LCD_COLS; i++)
    {
        row0[i] = ' ';
        row1[i] = CH_GROUND;
    }

    int player_row = (g->jump_ticks > 0) ? 0 : 1;
    uint8_t dch = (g->anim ? CH_DINO_B : CH_DINO_A);

    if (player_row == 0) row0[PLAYER_X] = dch;
    else                 row1[PLAYER_X] = dch;

    for (int i = 0; i < 2; i++)
    {
        const obstacle_t *o = &g->obs[i];
        if (!o->active) continue;
        if (o->x < 0 || o->x >= LCD_COLS) continue;

        if (o->row == 0) row0[o->x] = char_for_obstacle(o);
        else             row1[o->x] = char_for_obstacle(o);
    }

    char s[5];
    snprintf(s, sizeof(s), "%4d", g->score);
    for (int i = 0; i < 4; i++) row0[LCD_COLS - 4 + i] = (uint8_t)s[i];

    lcd_gotoxy(0, 0);
    for (int i = 0; i < LCD_COLS; i++) lcd_putc((char)row0[i]);

    lcd_gotoxy(1, 0);
    for (int i = 0; i < LCD_COLS; i++) lcd_putc((char)row1[i]);
}

static void render_game_over(int score)
{
    char line0[17], line1[17];

    snprintf(line0, sizeof(line0), "   GAME OVER    ");
    snprintf(line1, sizeof(line1), "Score:%6d", score);

    int len = (int)strlen(line1);
    while (len < 16) line1[len++] = ' ';
    line1[16] = '\0';

    lcd_gotoxy(0, 0);
    lcd_puts(line0);
    lcd_gotoxy(1, 0);
    lcd_puts(line1);
}

void app_sidescroller_start(void)
{
    game_t g;

    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Sidescroller    ");
    lcd_gotoxy(1, 0);
    lcd_puts("Up=Jump Left=Out");
    sleep_ms(1200);

    load_sprites();
    game_input_init();
    reset_game(&g);

    bool prev_up = true;
    bool prev_left = true;

    absolute_time_t next = make_timeout_time_ms(TICK_MS);

    while (1)
    {
        bool up_now = gpio_get(SW_UP);       // pullup: 1 = nicht gedrueckt
        bool left_now = gpio_get(SW_LEFT);

        bool up_pressed = (prev_up && !up_now);
        bool left_pressed = (prev_left && !left_now);

        prev_up = up_now;
        prev_left = left_now;

        if (left_pressed)
        {
            gpio_put(BUZZER, 0);
            lcd_clear();
            return;
        }

        if (!g.game_over)
        {
            if (up_pressed && g.jump_ticks == 0)
            {
                g.jump_ticks = 4;
                buzzer_beep_ms(12);
            }

            g.anim ^= 1;

            if (g.jump_ticks > 0)
                g.jump_ticks--;

            if (g.spawn_in > 0)
                g.spawn_in--;

            if (g.spawn_in == 0)
                spawn_obstacle(&g);

            int player_row = (g.jump_ticks > 0) ? 0 : 1;

            for (int i = 0; i < 2; i++)
            {
                obstacle_t *o = &g.obs[i];
                if (!o->active) continue;

                if (o->x == PLAYER_X && o->row == player_row)
                {
                    g.game_over = 1;
                    buzzer_beep_ms(200);
                }

                o->x--;

                if (o->x < 0)
                    o->active = 0;
            }

            g.score++;
            render(&g);
        }
        else
        {
            render_game_over(g.score);

            if (up_pressed)
            {
                reset_game(&g);
                sleep_ms(200);
            }
        }

        sleep_until(next);
        next = delayed_by_ms(next, TICK_MS);
    }
}