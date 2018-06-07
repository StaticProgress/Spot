#include <efi.h>
#include <efilib.h>

#define CUR_MODE video_output.all_modes[video_output.cur_mode]

typedef void (*DRAW_PIXEL_FN)(UINTN, UINTN, UINT32);

typedef struct VIDEO_MODE {
	UINT32 mode_id;
	UINT32 h_res;
	UINT32 v_res;
	UINT32 pixel_per_scan_line;
	DRAW_PIXEL_FN draw_pixel;
} VIDEO_MODE;

typedef struct VIDEO_DEVICE {
	UINT32 cur_mode;
	UINT32 max_mode;
	UINT8 *frame_buffer_base;
	VIDEO_MODE **all_modes;
} VIDEO_DEVICE;

void clear_screen();
void draw_rect(UINTN x, UINTN y, UINTN width, UINTN height, UINT32 color);
DRAW_PIXEL_FN select_draw_pixel(EFI_PIXEL_BITMASK *bitmask, EFI_GRAPHICS_PIXEL_FORMAT format);

extern VIDEO_DEVICE video_output;
