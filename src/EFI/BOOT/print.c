#include <efi.h>

#include "print.h"
#include "graphics.h"
#include "kprint.h"

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

void printf(char *str) {
    for(char *character = str; *character != '\0'; character++) {
        //Protect against really long lines with wrapping.
        //1017 is the last multiple of 9 (character width) in 1024
        if(curr_x >= 1017) {
            newline();
        }

        if(curr_y >= 756) {
            scroll_up_one_line();
        }

        if(*character == '\n') {
            //TODO: Add support for abitrarily sized characters.
            newline();
        } else {
            kprint_char(curr_x, curr_y, 0xFFFFFF, *character);
            curr_x += 9;
        }
    }
}
