#include <types.h>
#include <io.h>
#include <vga.h>


uint16_t cursor_row = 0;
uint16_t cursor_col = 0;


void enable_cursor(uint8_t start, uint8_t end) {
    outb(0x3D4, 0x0A);  // select cursor start register
    outb(0x3D5, (inb(0x3D5) & 0xC0) | start);  // keep top 2 bits, set start

    outb(0x3D4, 0x0B);  // select cursor end register
    outb(0x3D5, (inb(0x3D5) & 0xE0) | end);   // keep top 3 bits, set end
}

void update_cursor() {
    uint16_t pos = cursor_row * SCREEN_WIDTH + cursor_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void cursor_scroll() {
    uint16_t *buffer = VGA_TEXT_BUFFER;
    for (uint16_t row = 1; row < SCREEN_HEIGHT; row++) {
        for (uint16_t i = 0; i < SCREEN_WIDTH; i++) {
            buffer[(row - 1) * SCREEN_WIDTH + i] = buffer[row * SCREEN_WIDTH + i];
        }
    }

    for (uint16_t i = 0; i < SCREEN_WIDTH; i++) {
        buffer[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + i] = 0xFFF;
    }
    cursor_row = SCREEN_HEIGHT - 1;
    cursor_col = 0;
    update_cursor();
    return;
}

void putc(char c) {
    uint16_t *buffer = VGA_TEXT_BUFFER + cursor_row * SCREEN_WIDTH + cursor_col;

    // handle some special characters
    switch (c) {
        case '\n':  // new line
            cursor_col = 0;
            cursor_row++;

            if (cursor_row >= SCREEN_HEIGHT) {
                cursor_scroll();
            }
            update_cursor();
            return;
        case '\b':
            backSpace();
            return;
        case '\r': // start of row
            cursor_col = 0;
            
            // update_cursor();
            return;
        case '\t': // tab
            // for (uint16_t i = 0; i < 4; i++) {
            //     putc(' ');
            // }
            // putu32(cursor_col);
            // putc(' ');
            // putu32(cursor_row);
            return;
        default:
            break;
    }

    buffer[0] = c | CHAR_ATTR_BYTE << 8;

    cursor_col++;

    if (cursor_col >= SCREEN_WIDTH) {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= SCREEN_HEIGHT) {
            cursor_scroll();
        }

    }
    update_cursor();
    return;
}

void puts(char *s) {
    for (uint16_t i = 0; s[i] != '\0'; i++) {
        putc(s[i]);
    }
    return;
}

void putInt(uint32_t value) {
    char buf[22];
    int pos = 21;
    buf[pos] = '\0';
    bool sign = false;

    if (value == 0) {
        putc('0');
        return;
    } 

    if (value < 0) {
        sign = true;
        value *= -1;
    }

    while (value > 0 && pos) {
        buf[--pos] = '0' + (value % 10);
        value /= 10;
    }
    if (sign) {
        buf[--pos] = sign;
    }
    puts(&buf[pos]);

    return;
}
void putu32(uint32_t value, uint8_t base) {
    char buf[11];
    int pos = 10;
    buf[pos] = '\0';
    char *digits = "0123456789ABCDEF";

    if (value == 0) {
        putc('0');
        return;
    } 

    while (value > 0 && pos) {
        buf[--pos] = digits[value % base];
        value /= base;
    }

    //return error for too big
    // if (pos < 0) {

    // }
    puts(&buf[pos]);

    return;
}
 
void clear_screen() {
    uint16_t *buffer = VGA_TEXT_BUFFER;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) { 
        buffer[i] = 0xFFF;      
    }
    cursor_col = 0;
    cursor_row = 0;
    // enable_cursor(0, 15);
    update_cursor();
}
// void blink() {
//     uint16_t *buffer = VGA_TEXT_BUFFER + cursor_row * SCREEN_WIDTH + cursor_col;
//     uint8_t ch = buffer[0] & 0xFF;  
//     uint8_t attr = (buffer[0] >> 8) & 0xFF;

//     uint8_t fg = attr & 0x0F;           // bits 0-3
//     uint8_t bg = (attr >> 4) & 0x07;    // bits 4-6
//     uint8_t blink = attr & 0x80;        // optional blink

//     uint8_t new_attr = (fg << 4) | bg | blink;
//     buffer[0] = (new_attr << 8) | ch;
//     return;
// }

// void invert_cell(int pos) {
//     volatile unsigned char *video = (unsigned char*)0xB8000;

//     unsigned char ch = video[pos * 2];          // character
//     unsigned char attr = video[pos * 2 + 1];    // attribute

//     unsigned char new_attr = ~attr;

//     video[pos * 2 + 1] = new_attr;
// }

// void blink_cursor() {
//     uint16_t* buf = VGA_TEXT_BUFFER;
//     uint16_t* cell = &buf[cursor_row * SCREEN_WIDTH + cursor_col];
//     *cell ^= 0x0800;  // toggle background bit
// }


void backSpace() {  
    if (cursor_col == 0 && cursor_row != 0) {
        cursor_row--;
        cursor_col = SCREEN_WIDTH;
    }


    uint16_t *buffer = VGA_TEXT_BUFFER + cursor_row * SCREEN_WIDTH + --cursor_col;

    buffer[0] = 0xFFF;

    update_cursor();
}

void printf(const char* fmt, ...){
    int *args = (int *)(&fmt + 1); // get second argument from stack

    while (*fmt) {

        if (*fmt != '%') {
            putc(*fmt++);
            continue;
        }
        fmt++;

        switch (*fmt) {
            case 'c':
                putc((char)*args++);
                break;
            case 'd':
                putInt((uint32_t)*args++);
                break;
            case 's':
                puts((char *)*args++);
                break;
            case 'x':
                putu32((uint32_t)*args++, 16);
                break;
        }
        *fmt++;
    }



}