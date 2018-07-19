#include <efi.h>
#include <stdarg.h>

#include "print.h"
#include "graphics.h"
#include "kprint.h"
#include "util.h"

static UINTN curr_x, curr_y = 0;

static void newline() {
    curr_x = 0;
    curr_y += 14;
}

static void scroll_up_one_line() {
    //We should be making it back to 756 and then scrolling up by one
    //character height.
    int diff = curr_y - 756;
    diff += 14;
    curr_y -= diff;
    kscroll(diff);
}

static void print_char(char character) {
    //Protect against really long lines with wrapping.
    //1017 is the last multiple of 9 (character width) in 1024
    if(curr_x >= 1017) {
        newline();
    }

    if(curr_y >= 756) {
        scroll_up_one_line();
    }

    kprint_char(curr_x, curr_y, 0xFFFFFF, character);
    curr_x += 9; 
}

static void print_signed_integer(int data) {
    char num_buff[12];
    itoa(num_buff, 12, data);
    for(int i = 0; i < 12 && num_buff[i] != '\0'; i++)
        print_char(num_buff[i]);
}

static void print_hex_integer(int data) {
    char num_buff[17];
    itox(num_buff, 17, data);
    for(int i = 0; i < 17 && num_buff[i] != '\0'; i++)
        print_char(num_buff[i]);
}

void printf(char *str, ...) {
    va_list inputs;
    va_start(inputs, str);

    for(char *character = str; *character != '\0'; character++) {

        if(*character == '\n') {
            //TODO: Add support for abitrarily sized characters.
            newline();
        } else if(*character == '%') {
            //The character should point to a % sign and therefore is a valid formatter 
            switch(*(++character)) {
                case 'd':
                    //Integer, signed, four bytes.
                    print_signed_integer(va_arg(inputs, int));
                    break; 
                case 'x':
                    //Integer, in Hexidecimal.
                    print_hex_integer(va_arg(inputs, int));
                    break;
            }
        } else {
            print_char(*character);
        }
    }
    va_end(inputs);
}
