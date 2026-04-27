#include "app_tetris.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "lcd1602.h"
#include "buttons.h"

// Joystick Buzzerino
#define JS_BUTTON 18
#define JS_Y_PIN  27
#define JS_Y_ADC  1

// Grid 16x6 (Subpixel)
#define W 16
#define H 6

#define TICK_MS 220

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
static uint8_t fb0[W], fb1[W];

static inline void fb_clear(void)
{
    memset(fb0, 0, sizeof(fb0));
    memset(fb1, 0, sizeof(fb1));
}

static inline void fb_or_cell(uint8_t x, uint8_t y)
{
    if (x >= W || y >= H) return;

    uint8_t band = (uint8_t)(y % 3);
    uint8_t bit  = (band == 0) ? 1 : (band == 1) ? 2 : 4;

    if (y < 3) fb0[x] |= bit;
    else       fb1[x] |= bit;
}

static void fb_render(void)
{
    lcd_gotoxy(0, 0);
    for (int x = 0; x < W; x++) lcd_putc((char)(fb0[x] & 7));

    lcd_gotoxy(1, 0);
    for (int x = 0; x < W; x++) lcd_putc((char)(fb1[x] & 7));
}

// ---------- Input ----------
static void tetris_input_init(void)
{
    adc_init();
    adc_gpio_init(JS_Y_PIN);
    adc_select_input(JS_Y_ADC);

    gpio_init(JS_BUTTON);
    gpio_set_dir(JS_BUTTON, GPIO_IN);
    gpio_pull_up(JS_BUTTON);
}

static int js_button_pressed_edge(void)
{
    static bool prev = true;
    bool now = gpio_get(JS_BUTTON);
    bool edge = (prev && !now);
    prev = now;
    return edge;
}

static int joy_y_dir(void)
{
    uint16_t v = adc_read();
    const int mid = 2048;
    const int dead = 350;

    if (v < mid - dead) return +1;
    if (v > mid + dead) return -1;
    return 0;
}

// ---------- Tetris pieces ----------
typedef struct
{
    uint16_t r[4];
} piece_t;

static const piece_t pieces[5] = {
    {{0b0000, 0b1111, 0b0000, 0b0000}}, // I
    {{0b0000, 0b0110, 0b0110, 0b0000}}, // O
    {{0b0000, 0b0111, 0b0010, 0b0000}}, // T
    {{0b0000, 0b0111, 0b0001, 0b0000}}, // L
    {{0b0000, 0b0011, 0b0110, 0b0000}}, // S
};

// ---------- Game state ----------
static uint8_t field[W][H];
static int score = 0;

typedef struct
{
    int id;
    int rot;
    int x, y;
    uint16_t shape[4];
} active_t;

static active_t cur;

// RNG
static inline uint32_t rng_u32(void)
{
    static uint32_t s = 0;
    if (!s) s = time_us_32() ^ 0xA5A5A5A5u;

    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    return s;
}

static void rotate_cw(const uint16_t in[4], uint16_t out[4])
{
    for (int y = 0; y < 4; y++)
    {
        uint16_t row = 0;
        for (int x = 0; x < 4; x++)
        {
            int bit = (in[3 - x] >> y) & 1;
            row |= (bit << x);
        }
        out[y] = row;
    }
}

static void build_shape(active_t *a)
{
    uint16_t tmp[4], tmp2[4];

    for (int i = 0; i < 4; i++) tmp[i] = pieces[a->id].r[i];

    for (int k = 0; k < a->rot; k++)
    {
        rotate_cw(tmp, tmp2);
        for (int i = 0; i < 4; i++) tmp[i] = tmp2[i];
    }

    for (int i = 0; i < 4; i++) a->shape[i] = tmp[i] & 0xF;
}

static int collides(const active_t *a, int nx, int ny, const uint16_t shape[4])
{
    (void)a;

    for (int yy = 0; yy < 4; yy++)
    {
        uint16_t row = shape[yy];

        for (int xx = 0; xx < 4; xx++)
        {
            if ((row >> xx) & 1)
            {
                int gx = nx + xx;
                int gy = ny + yy;

                if (gx < 0 || gx >= W || gy < 0 || gy >= H) return 1;
                if (field[gx][gy]) return 1;
            }
        }
    }

    return 0;
}

static int spawn_piece(void)
{
    cur.id = (int)(rng_u32() % 5);
    cur.rot = 0;
    build_shape(&cur);

    cur.x = 0;
    cur.y = (H / 2) - 2;

    if (cur.y < 0) cur.y = 0;
    if (cur.y > H - 4) cur.y = H - 4;

    if (collides(&cur, cur.x, cur.y, cur.shape)) return 0;
    return 1;
}

static void lock_piece(void)
{
    for (int yy = 0; yy < 4; yy++)
    {
        uint16_t row = cur.shape[yy];

        for (int xx = 0; xx < 4; xx++)
        {
            if ((row >> xx) & 1)
            {
                int gx = cur.x + xx;
                int gy = cur.y + yy;

                if (gx >= 0 && gx < W && gy >= 0 && gy < H)
                    field[gx][gy] = 1;
            }
        }
    }
}

static void clear_full_columns(void)
{
    for (int x = 0; x < W; x++)
    {
        int full = 1;

        for (int y = 0; y < H; y++)
        {
            if (!field[x][y])
            {
                full = 0;
                break;
            }
        }

        if (full)
        {
            for (int xx = x; xx > 0; xx--)
            {
                for (int y = 0; y < H; y++)
                    field[xx][y] = field[xx - 1][y];
            }

            for (int y = 0; y < H; y++)
                field[0][y] = 0;

            score++;
            x--;
        }
    }
}

static void render(void)
{
    fb_clear();

    for (int x = 0; x < W; x++)
    {
        for (int y = 0; y < H; y++)
        {
            if (field[x][y]) fb_or_cell((uint8_t)x, (uint8_t)y);
        }
    }

    for (int yy = 0; yy < 4; yy++)
    {
        uint16_t row = cur.shape[yy];

        for (int xx = 0; xx < 4; xx++)
        {
            if ((row >> xx) & 1)
            {
                fb_or_cell((uint8_t)(cur.x + xx), (uint8_t)(cur.y + yy));
            }
        }
    }

    fb_render();
}

static void show_game_over(void)
{
    char l0[17], l1[17];

    snprintf(l0, sizeof(l0), "   GAME OVER    ");
    snprintf(l1, sizeof(l1), "Score:%6d", score);

    int n = (int)strlen(l1);
    while (n < 16) l1[n++] = ' ';
    l1[16] = 0;

    lcd_gotoxy(0, 0);
    lcd_puts(l0);
    lcd_gotoxy(1, 0);
    lcd_puts(l1);
}

static void game_reset(void)
{
    memset(field, 0, sizeof(field));
    score = 0;
}

void app_tetris_start(void)
{
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Tetris          ");
    lcd_gotoxy(1, 0);
    lcd_puts("JS rot L=Back   ");
    sleep_ms(1000);

    load_subpixel_charset();
    tetris_input_init();
    game_reset();

    if (!spawn_piece())
    {
        show_game_over();

        while (1)
        {
            if (button_left_pressed())
            {
                lcd_clear();
                return;
            }

            if (js_button_pressed_edge())
            {
                game_reset();
                spawn_piece();
                sleep_ms(200);
                break;
            }

            sleep_ms(10);
        }
    }

    absolute_time_t next = make_timeout_time_ms(TICK_MS);

    while (1)
    {
        if (button_left_pressed())
        {
            lcd_clear();
            return;
        }

        int dy = joy_y_dir();
        if (dy != 0)
        {
            int ny = cur.y + dy;
            if (!collides(&cur, cur.x, ny, cur.shape))
                cur.y = ny;
        }

        if (js_button_pressed_edge())
        {
            uint16_t rotated[4];
            rotate_cw(cur.shape, rotated);

            if (!collides(&cur, cur.x, cur.y, rotated))
            {
                for (int i = 0; i < 4; i++) cur.shape[i] = rotated[i];
            }
            else if (!collides(&cur, cur.x, cur.y - 1, rotated))
            {
                cur.y -= 1;
                for (int i = 0; i < 4; i++) cur.shape[i] = rotated[i];
            }
            else if (!collides(&cur, cur.x, cur.y + 1, rotated))
            {
                cur.y += 1;
                for (int i = 0; i < 4; i++) cur.shape[i] = rotated[i];
            }
        }

        if (!collides(&cur, cur.x + 1, cur.y, cur.shape))
        {
            cur.x += 1;
        }
        else
        {
            lock_piece();
            clear_full_columns();

            if (!spawn_piece())
            {
                show_game_over();

                while (1)
                {
                    if (button_left_pressed())
                    {
                        lcd_clear();
                        return;
                    }

                    if (js_button_pressed_edge())
                    {
                        game_reset();
                        spawn_piece();
                        sleep_ms(200);
                        break;
                    }

                    sleep_ms(10);
                }
            }
        }

        render();

        sleep_until(next);
        next = delayed_by_ms(next, TICK_MS);
    }
}