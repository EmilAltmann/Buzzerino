#ifndef LCD1602_H
#define LCD1602_H

#include <stdint.h>

void lcd_init(void);
void lcd_putc(char c);
void lcd_puts(const char *s);
void lcd_cursor_left(void);
void lcd_cursor_right(void);
void lcd_gotoxy(uint8_t row, uint8_t col);   // 0-based

void lcd_cursor_visible(uint8_t on);
void lcd_cursor_blink(uint8_t on);

#endif