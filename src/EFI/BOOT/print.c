#include "print.h"
#include "graphics.h"
#include "kprint.h"

void printf(char *str) {
    kprint_m(video_output.frame_buffer_base, 0xFFFFFF, str);
}
