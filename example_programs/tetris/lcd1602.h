#pragma once
#include <stdint.h>

void lcd_init(void);
void lcd_putc(char c);
void lcd_puts(const char *s);
void lcd_gotoxy(uint8_t row, uint8_t col);

void lcd_cursor_visible(uint8_t on);
void lcd_cursor_blink(uint8_t on);
void lcd_cursor_left(void);
void lcd_cursor_right(void);

// HD44780: bis zu 8 Custom-Chars (Slots 0..7), Bitmap: 8 Zeilen à 5 Bit
void lcd_create_char(uint8_t slot, const uint8_t bitmap[8]);