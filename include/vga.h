#ifndef VGA_H
#define VGA_H

#include <types.h>

#define VGA_TEXT_BUFFER (uint16_t *)0xC00B8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define CHAR_ATTR_BYTE 0x07


void clear_screen();
void update_cursor();
void enable_cursor(uint8_t start, uint8_t end);
void cursor_scroll();
void putc(char c);
void puts(char *s);
void putu32(uint32_t value, uint8_t base);
void blink();
void invert_cell(int pos);
void blink_cursor();
void backSpace();
void printf(const char *fmt, ...);

#endif
