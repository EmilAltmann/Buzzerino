#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "lcd1602.h"

// Joystick (Buzzerino)
#define JS_BUTTON 18
#define JS_Y_PIN  27   // ADC1 (GPIO27)
#define JS_Y_ADC  1

// Subpixel-grid
#define W 16
#define H 6

#define PADDLE_X (W - 1)
#define PADDLE_H 4

#define TICK_MS 120

// ---------- 3-band charset ----------
static void make_3band_char(uint8_t mask3, uint8_t out[8]) {
    for (int i = 0; i < 8; i++) out[i] = 0x00;
    if (mask3 & 0x1) { out[0] = 0x1F; out[1] = 0x1F; }
    if (mask3 & 0x2) { out[3] = 0x1F; out[4] = 0x1F; }
    if (mask3 & 0x4) { out[6] = 0x1F; out[7] = 0x1F; }
}
static void load_subpixel_charset(void) {
    uint8_t bmp[8];
    for (uint8_t m = 0; m < 8; m++) {
        make_3band_char(m, bmp);
        lcd_create_char(m, bmp);
    }
}

// ---------- framebuffer ----------
static uint8_t fb0[W], fb1[W];

static inline void fb_clear(void) {
    memset(fb0, 0, sizeof(fb0));
    memset(fb1, 0, sizeof(fb1));
}

// setzt genau EIN Band (y%3) an Position (x,y)
static inline void fb_or_cell(uint8_t x, uint8_t y) {
    if (x >= W || y >= H) return;
    uint8_t band = (uint8_t)(y % 3);
    uint8_t bit  = (band == 0) ? 1 : (band == 1) ? 2 : 4;
    if (y < 3) fb0[x] |= bit;
    else       fb1[x] |= bit;
}

static void fb_render(void) {
    lcd_gotoxy(0,0);
    for (int x = 0; x < W; x++) lcd_putc((char)(fb0[x] & 7));
    lcd_gotoxy(1,0);
    for (int x = 0; x < W; x++) lcd_putc((char)(fb1[x] & 7));
}

// ---------- Spielzustand ----------
static uint8_t blocks[W][H]; // 1 = block vorhanden (Subpixel-block)
static int score = 0;

// Ball
static int bx, by;
static int vx, vy;

// Paddle top y
static int py = 2;

// ---------- Input ----------
static void input_init(void) {
    adc_init();
    adc_gpio_init(JS_Y_PIN);
    adc_select_input(JS_Y_ADC);

    gpio_init(JS_BUTTON);
    gpio_set_dir(JS_BUTTON, GPIO_IN);
    gpio_pull_up(JS_BUTTON);
}

static int btn_pressed_edge(void) {
    static bool prev = true;
    bool now = gpio_get(JS_BUTTON);
    bool edge = (prev && !now);
    prev = now;
    return edge;
}

// Zentrierung + Joystick Richtung FIXED:
// du sagst invertiert -> wir drehen die Zuordnung um:
// kleiner ADC => paddle runter, größer ADC => paddle hoch
static int py_update_with_centering(int py_current) {
    uint16_t v = adc_read();      // 0..4095
    int max_py = H - PADDLE_H;    // 4
    int center_py = max_py / 2;   // 2

    const int mid  = 2048;
    const int dead = 350;

    int dy = 0;

    // INVERTED mapping:
    if (v < (mid - dead)) dy = +1;       // stick "hoch" (bei dir) -> paddle nach unten
    else if (v > (mid + dead)) dy = -1;  // stick "runter" -> paddle nach oben
    else {
        if (py_current < center_py) dy = +1;
        else if (py_current > center_py) dy = -1;
    }

    int py_new = py_current + dy;
    if (py_new < 0) py_new = 0;
    if (py_new > max_py) py_new = max_py;
    return py_new;
}

// ---------- Init/Reset ----------
static void reset_level(void) {
    memset(blocks, 0, sizeof(blocks));
    score = 0;

    // linke Hälfte mit Subpixel-"Steinen" füllen: x=0..6, alle y
    for (int x = 0; x <= 6; x++) {
        for (int y = 0; y < H; y++) blocks[x][y] = 1;
    }

    py = (H - PADDLE_H) / 2; // 2

    bx = 10; by = 2;
    vx = -1; vy = 1;
}

// ---------- Physik ----------
static int paddle_hit(int y) {
    if (y < py || y >= py + PADDLE_H) return 99;
    int rel = y - py; // 0 oder 1
    return (rel == 0) ? -1 : +1; // oben/unten
}

static void step_game(int *game_over) {
    int nx = bx + vx;
    int ny = by + vy;

    // top/bottom
    if (ny < 0)  { ny = 0;   vy = +1; }
    if (ny >= H) { ny = H-1; vy = -1; }

    // left wall
    if (nx < 0)  { nx = 0;   vx = +1; }

    // paddle at right edge
    if (nx == PADDLE_X) {
        int hit = paddle_hit(ny);
        if (hit != 99) {
            vx = -1;
            vy = (hit == -1) ? -1 : +1;
            nx = PADDLE_X - 1;
        } else {
            *game_over = 1;
            return;
        }
    }

    // block collision
    if (nx >= 0 && nx < W && ny >= 0 && ny < H && blocks[nx][ny]) {
        blocks[nx][ny] = 0;
        score++;

        // bounce x
        vx = -vx;
        nx = bx + vx;
        ny = by + vy;

        if (ny < 0)  { ny = 0;   vy = +1; }
        if (ny >= H) { ny = H-1; vy = -1; }
        if (nx < 0)  { nx = 0;   vx = +1; }
    }

    bx = nx; by = ny;
}

// ---------- Render ----------
static void render(void) {
    fb_clear();

    // blocks: als 1 Subpixel pro Zelle
    for (int x=0; x<W; x++) {
        for (int y=0; y<H; y++) {
            if (blocks[x][y]) fb_or_cell((uint8_t)x, (uint8_t)y);
        }
    }

    // paddle: 2 Subpixel hoch
    for (int i=0; i<PADDLE_H; i++) {
        fb_or_cell(PADDLE_X, (uint8_t)(py + i));
    }

    // ball
    fb_or_cell((uint8_t)bx, (uint8_t)by);

    fb_render();
}

static void show_game_over(void) {
    char l0[17], l1[17];
    snprintf(l0, sizeof(l0), "   GAME OVER    ");
    snprintf(l1, sizeof(l1), "Score:%6d", score);
    int n = (int)strlen(l1);
    while (n < 16) l1[n++] = ' ';
    l1[16] = 0;
    lcd_gotoxy(0,0); lcd_puts(l0);
    lcd_gotoxy(1,0); lcd_puts(l1);
}

int main() {
    stdio_init_all();
    lcd_init();
    load_subpixel_charset();
    input_init();

    reset_level();

    absolute_time_t next = make_timeout_time_ms(TICK_MS);

    while (1) {
        py = py_update_with_centering(py);

        int game_over = 0;
        step_game(&game_over);

        if (game_over) {
            show_game_over();
            while (!btn_pressed_edge()) sleep_ms(10);
            reset_level();
            sleep_ms(200);
        } else {
            render();
        }

        sleep_until(next);
        next = delayed_by_ms(next, TICK_MS);
    }
}