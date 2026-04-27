#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "pico/stdlib.h"
#include "lcd1602.h"
#include "aht10.h"
#include "weather_station.h"

static void lcd_write_line_padded(int line, const char *fmt, ...)
{
    char tmp[64];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(tmp, sizeof(tmp), fmt, ap);
    va_end(ap);

    char buf[17];
    size_t n = strlen(tmp);
    if (n > 16) n = 16;

    memcpy(buf, tmp, n);
    while (n < 16) buf[n++] = ' ';
    buf[16] = '\0';

    lcd_gotoxy(line, 0);
    lcd_puts(buf);
}

static int round_to_int10(float x)
{
    return (int)(x * 10.0f + (x >= 0.0f ? 0.5f : -0.5f));
}

void lcd_show_th_no_floatfmt(float t, float rh)
{
    if (!(t == t))
    {
        lcd_write_line_padded(0, "Temp:   ----");
    }
    else
    {
        int ti = round_to_int10(t);
        lcd_write_line_padded(0, "Temp:%3d.%1dC", ti / 10, abs(ti % 10));
    }

    if (!(rh == rh))
    {
        lcd_write_line_padded(1, "RH:     ----");
    }
    else
    {
        int hi = round_to_int10(rh);
        lcd_write_line_padded(1, "RH:%4d.%1d%%", hi / 10, abs(hi % 10));
    }
}

void display_tmp_and_humid(aht10_t sensor)
{
    float t, rh;
    if (aht10_read(&sensor, &t, &rh))
    {
        lcd_show_th_no_floatfmt(t, rh);
    }
}