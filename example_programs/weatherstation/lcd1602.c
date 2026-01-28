#include "lcd1602.h"
#include "pico/stdlib.h"

#define LCD_D7 6
#define LCD_D6 7
#define LCD_D5 8
#define LCD_D4 9
#define LCD_D3 10
#define LCD_D2 11
#define LCD_D1 12
#define LCD_D0 13

#define LCD_E 14
#define LCD_RS 15


static uint8_t _disp_ctrl = 0x0C; // D=1, C=0, B=0

static void lcd_pulse_e(void)
{
    gpio_put(LCD_E, 1);
    sleep_us(1); // ≥ 450 ns
    gpio_put(LCD_E, 0);
}

static void lcd_write8(uint8_t val)
{
    gpio_put(LCD_D0, val & 0x01);
    gpio_put(LCD_D1, val & 0x02);
    gpio_put(LCD_D2, val & 0x04);
    gpio_put(LCD_D3, val & 0x08);
    gpio_put(LCD_D4, val & 0x10);
    gpio_put(LCD_D5, val & 0x20);
    gpio_put(LCD_D6, val & 0x40);
    gpio_put(LCD_D7, val & 0x80);
    lcd_pulse_e();
}

static void lcd_cmd(uint8_t cmd)
{
    gpio_put(LCD_RS, 0);
    lcd_write8(cmd);

    // sleep times according to data sheet
    if (cmd == 0x01 || cmd == 0x02) // clear / home
        sleep_ms(2);
    else
        sleep_us(50);
}

static void lcd_data(uint8_t data)
{
    gpio_put(LCD_RS, 1);
    lcd_write8(data);
    sleep_us(50);
}


void lcd_init(void)
{
    // configure GPIO as outputs
    const uint lcd_pins[] = {
        LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6, LCD_D7,
        LCD_E, LCD_RS};
    for (uint i = 0; i < sizeof(lcd_pins) / sizeof(lcd_pins[0]); ++i)
    {
        gpio_init(lcd_pins[i]);
        gpio_set_dir(lcd_pins[i], GPIO_OUT);
        gpio_put(lcd_pins[i], 0);
    }

    sleep_ms(20); // >15 ms sleep after power-up

    /* base initialisation (8-Bit, 2-rows, 5×8) */
    lcd_cmd(0x38);       // Function Set   8-bit | 2 Lines | 5x8
    lcd_cmd(0x08);       // Display off
    lcd_cmd(0x01);       // Clear
    lcd_cmd(0x06);       // Entry Mode: cursor++, no shift
    lcd_cmd(_disp_ctrl); // Display ON, Cursor/Blink if set
}

void lcd_putc(char c)
{
    lcd_data((uint8_t)c);
}

void lcd_puts(const char *s)
{
    while (*s)
        lcd_putc(*s++);
}

void lcd_cursor_visible(uint8_t on)
{
    if (on)
        _disp_ctrl |= 0x02; // set bit1
    else
        _disp_ctrl &= ~0x02; // remove bit1
    lcd_cmd(_disp_ctrl);
}

void lcd_cursor_blink(uint8_t on)
{
    if (on)
        _disp_ctrl |= 0x01; // set bit0
    else
        _disp_ctrl &= ~0x01; // remove bit0
    lcd_cmd(_disp_ctrl);
}

void lcd_cursor_left(void) { lcd_cmd(0x10); }  // Cursor-Shift left
void lcd_cursor_right(void) { lcd_cmd(0x14); } // Cursor-Shift right

void lcd_gotoxy(uint8_t row, uint8_t col)
{
    static const uint8_t row_addr[2] = {0x00, 0x40}; // 1602 has 2 lines
    lcd_cmd(0x80 | (row_addr[row & 1] + (col & 0x0F)));
}