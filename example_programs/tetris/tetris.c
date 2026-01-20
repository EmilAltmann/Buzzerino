#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "lcd1602.h"

// Joystick Buzzerino
#define JS_BUTTON 18
#define JS_Y_PIN  27   // ADC1
#define JS_Y_ADC  1

// Grid 16x6 (Subpixel)
#define W 16
#define H 6

#define TICK_MS 220

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

// framebuffer: 2 lcd rows, 16 cols, mask 0..7
static uint8_t fb0[W], fb1[W];
static inline void fb_clear(void){ memset(fb0,0,sizeof(fb0)); memset(fb1,0,sizeof(fb1)); }
static inline void fb_or_cell(uint8_t x, uint8_t y){
    if (x>=W || y>=H) return;
    uint8_t band = (uint8_t)(y % 3);
    uint8_t bit  = (band==0)?1:(band==1)?2:4;
    if (y<3) fb0[x] |= bit; else fb1[x] |= bit;
}
static void fb_render(void){
    lcd_gotoxy(0,0); for(int x=0;x<W;x++) lcd_putc((char)(fb0[x]&7));
    lcd_gotoxy(1,0); for(int x=0;x<W;x++) lcd_putc((char)(fb1[x]&7));
}

// ---------- Input ----------
static void input_init(void){
    adc_init();
    adc_gpio_init(JS_Y_PIN);
    adc_select_input(JS_Y_ADC);

    gpio_init(JS_BUTTON);
    gpio_set_dir(JS_BUTTON, GPIO_IN);
    gpio_pull_up(JS_BUTTON);
}

static int btn_pressed_edge(void){
    static bool prev=true;
    bool now=gpio_get(JS_BUTTON);
    bool edge=(prev && !now);
    prev=now;
    return edge;
}

// Joystick Y: -1,0,+1 (mit Deadzone)
static int joy_y_dir(void){
    uint16_t v=adc_read();
    const int mid=2048;
    const int dead=350;
    // ggf. invertieren je nach Hardware:
    if (v < mid - dead) return +1; // nach oben
    if (v > mid + dead) return -1; // nach unten
    return 0;
}

// ---------- Tetris pieces (4x4 bitmasks) ----------
typedef struct { uint16_t r[4]; } piece_t;

// Bits: wir nutzen 4x4 in den unteren 4 Bits jeder Zeile (LSB = x0)
static const piece_t pieces[5] = {
    // I
    {{ 0b0000,
       0b1111,
       0b0000,
       0b0000 }},
    // O
    {{ 0b0000,
       0b0110,
       0b0110,
       0b0000 }},
    // T
    {{ 0b0000,
       0b0111,
       0b0010,
       0b0000 }},
    // L
    {{ 0b0000,
       0b0111,
       0b0001,
       0b0000 }},
    // S
    {{ 0b0000,
       0b0011,
       0b0110,
       0b0000 }},
};

static uint16_t rot_row(uint16_t m[4], int y){ return m[y] & 0xF; }

// rotate 4x4 clockwise
static void rotate_cw(const uint16_t in[4], uint16_t out[4]){
    // out[y][x] = in[3-x][y]
    for(int y=0;y<4;y++){
        uint16_t row=0;
        for(int x=0;x<4;x++){
            int bit = (in[3-x] >> y) & 1;
            row |= (bit << x);
        }
        out[y]=row;
    }
}

// ---------- Game state ----------
static uint8_t field[W][H]; // 0 empty, 1 filled
static int score=0;

typedef struct {
    int id;
    int rot;      // 0..3
    int x, y;     // top-left of 4x4 box in grid coords
    uint16_t shape[4];
} active_t;

static active_t cur;

// RNG simple
static inline uint32_t rng_u32(void){
    static uint32_t s=0;
    if(!s) s = time_us_32() ^ 0xA5A5A5A5u;
    s ^= s<<13; s ^= s>>17; s ^= s<<5;
    return s;
}

static void build_shape(active_t *a){
    // start from base piece r[], rotate rot times
    uint16_t tmp[4], tmp2[4];
    for(int i=0;i<4;i++) tmp[i]=pieces[a->id].r[i];

    for(int k=0;k<a->rot;k++){
        rotate_cw(tmp, tmp2);
        for(int i=0;i<4;i++) tmp[i]=tmp2[i];
    }
    for(int i=0;i<4;i++) a->shape[i]=tmp[i] & 0xF;
}

static int collides(const active_t *a, int nx, int ny, const uint16_t shape[4]){
    for(int yy=0; yy<4; yy++){
        uint16_t row = shape[yy];
        for(int xx=0; xx<4; xx++){
            if ((row >> xx) & 1) {
                int gx = nx + xx;
                int gy = ny + yy;
                if (gx < 0 || gx >= W || gy < 0 || gy >= H) return 1;
                if (field[gx][gy]) return 1;
            }
        }
    }
    return 0;
}

static int spawn_piece(void){
    cur.id = (int)(rng_u32() % 5);
    cur.rot = 0;
    build_shape(&cur);

    // Start links, mittig in y
    cur.x = 0;
    cur.y = (H/2) - 2; // so dass 4x4 box grob zentriert
    if (cur.y < 0) cur.y = 0;
    if (cur.y > H-4) cur.y = H-4;

    // wenn sofort Kollision => game over
    if (collides(&cur, cur.x, cur.y, cur.shape)) return 0;
    return 1;
}

static void lock_piece(void){
    for(int yy=0; yy<4; yy++){
        uint16_t row = cur.shape[yy];
        for(int xx=0; xx<4; xx++){
            if ((row >> xx) & 1) {
                int gx = cur.x + xx;
                int gy = cur.y + yy;
                if (gx>=0 && gx<W && gy>=0 && gy<H) field[gx][gy]=1;
            }
        }
    }
}

// Clear "full columns" (because gravity is left->right)
static void clear_full_columns(void){
    for(int x=0; x<W; x++){
        int full = 1;
        for(int y=0; y<H; y++){
            if (!field[x][y]) { full=0; break; }
        }
        if (full) {
            // shift everything left of x one column to the right? (we want gap to disappear towards left)
            // For left->right gravity: removing a column should pull all columns LEFT of it to the RIGHT by 1
            // simpler: move columns [0..x-1] one step to the right, and clear column 0.
            for(int xx=x; xx>0; xx--){
                for(int y=0;y<H;y++) field[xx][y] = field[xx-1][y];
            }
            for(int y=0;y<H;y++) field[0][y]=0;

            score++;

            // after shifting, re-check same x (because new content moved into it)
            x--;
        }
    }
}

static void render(void){
    fb_clear();

    // field
    for(int x=0;x<W;x++)
        for(int y=0;y<H;y++)
            if(field[x][y]) fb_or_cell((uint8_t)x,(uint8_t)y);

    // active piece
    for(int yy=0;yy<4;yy++){
        uint16_t row=cur.shape[yy];
        for(int xx=0;xx<4;xx++){
            if((row>>xx)&1){
                fb_or_cell((uint8_t)(cur.x+xx),(uint8_t)(cur.y+yy));
            }
        }
    }

    fb_render();
}

static void show_game_over(void){
    char l0[17], l1[17];
    snprintf(l0,sizeof(l0),"   GAME OVER    ");
    snprintf(l1,sizeof(l1),"Score:%6d",score);
    int n=(int)strlen(l1);
    while(n<16) l1[n++]=' ';
    l1[16]=0;
    lcd_gotoxy(0,0); lcd_puts(l0);
    lcd_gotoxy(1,0); lcd_puts(l1);
}

int main(){
    stdio_init_all();
    lcd_init();
    load_subpixel_charset();
    input_init();

    memset(field,0,sizeof(field));
    score=0;
    if(!spawn_piece()){
        show_game_over();
        while(!btn_pressed_edge()) sleep_ms(10);
    }

    absolute_time_t next = make_timeout_time_ms(TICK_MS);

    while(1){
        // Input: move in y
        int dy = joy_y_dir();
        if (dy != 0) {
            int ny = cur.y + dy;
            if (!collides(&cur, cur.x, ny, cur.shape)) cur.y = ny;
        }

        // Rotate on button
        if (btn_pressed_edge()) {
            uint16_t rotated[4];
            rotate_cw(cur.shape, rotated);

            // simple "wall kick" in y: try current y, then y-1, then y+1
            if (!collides(&cur, cur.x, cur.y, rotated)) {
                for(int i=0;i<4;i++) cur.shape[i]=rotated[i];
            } else if (!collides(&cur, cur.x, cur.y-1, rotated)) {
                cur.y -= 1;
                for(int i=0;i<4;i++) cur.shape[i]=rotated[i];
            } else if (!collides(&cur, cur.x, cur.y+1, rotated)) {
                cur.y += 1;
                for(int i=0;i<4;i++) cur.shape[i]=rotated[i];
            }
        }

        // Gravity step: move right
        if (!collides(&cur, cur.x+1, cur.y, cur.shape)) {
            cur.x += 1;
        } else {
            // lock, clear columns, spawn new
            lock_piece();
            clear_full_columns();
            if (!spawn_piece()) {
                show_game_over();
                // restart on button
                while(!btn_pressed_edge()) sleep_ms(10);
                memset(field,0,sizeof(field));
                score=0;
                spawn_piece();
            }
        }

        render();

        sleep_until(next);
        next = delayed_by_ms(next, TICK_MS);
    }
}