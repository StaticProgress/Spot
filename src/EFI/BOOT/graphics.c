#include <efi.h>
#include <efilib.h>

#include "graphics.h"

void draw_pixel_no_reserved_rgb(UINTN x, UINTN y, UINT32 color) {
	UINT8 *frame_ptr = (UINT8 *)video_output.frame_buffer_base + (y * video_output.all_modes[video_output.cur_mode]->h_res + x) * 3;
	*frame_ptr++ = 0xFF & (color >> 16);
	*frame_ptr++ = 0xFF & (color >> 8);
	*frame_ptr++ = 0xFF & color;
}
void draw_pixel_no_reserved_bgr(UINTN x, UINTN y, UINT32 color) {
	UINT8 *frame_ptr = (UINT8 *)video_output.frame_buffer_base + (y * video_output.all_modes[video_output.cur_mode]->h_res + x) * 3;
	*frame_ptr++ = 0xFF & color;
	*frame_ptr++ = 0xFF & (color >> 8);
	*frame_ptr++ = 0xFF & (color >> 16);
}
void draw_pixel_reserved_rgb(UINTN x, UINTN y, UINT32 color) {
	UINT32 *frame_ptr = (UINT32 *)video_output.frame_buffer_base + y * video_output.all_modes[video_output.cur_mode]->h_res + x;
	*frame_ptr++ = color << 2;
}
void draw_pixel_reserved_bgr(UINTN x, UINTN y, UINT32 color) {
	UINT32 *frame_ptr = (UINT32 *)video_output.frame_buffer_base + y * video_output.all_modes[video_output.cur_mode]->h_res + x;
	/* totally stole this from bit twiddling hacks */
	color = ((color >> 8) & 0xFF00FF) | ((color & 0xFF00FF) << 8); // swap bytes
	color = (color >> 16) | (color << 16);                         // swap 2-byte long pairs
	*frame_ptr++ = color;
}

DRAW_PIXEL_FN select_draw_pixel(EFI_PIXEL_BITMASK *bitmask, EFI_GRAPHICS_PIXEL_FORMAT format) {
	// Don't know if this is entirely correct, UEFI color info is messed up and doesn't correspond to actual order
	if (format == PixelRedGreenBlueReserved8BitPerColor || (bitmask->ReservedMask && (bitmask->RedMask & 0xFF00))) return draw_pixel_reserved_rgb;
	if (format == PixelBlueGreenRedReserved8BitPerColor || (bitmask->ReservedMask && (bitmask->RedMask & 0xFF000000))) return draw_pixel_reserved_bgr;
	if (!bitmask->ReservedMask && (bitmask->RedMask & 0xFF)) return draw_pixel_no_reserved_rgb;
	if (!bitmask->ReservedMask && (bitmask->RedMask & 0xFF0000)) return draw_pixel_no_reserved_bgr;
	return NULL;
}

void clear_screen() {
	DRAW_PIXEL_FN draw = CUR_MODE->draw_pixel;
	for (UINT32 i = 0; i < video_output.all_modes[video_output.cur_mode]->v_res; i++)
		for (UINT32 j = 0; j < video_output.all_modes[video_output.cur_mode]->h_res; j++) draw(j, i, 0x0);
}

void draw_rect(UINTN x, UINTN y, UINTN width, UINTN height, UINT32 color) {
    UINTN orig_x = x;
    UINTN end_x = x + width;
    UINTN end_y = y + height;
    if(end_x > CUR_MODE->h_res)
        end_x = CUR_MODE->h_res;
    if(end_y > CUR_MODE->v_res)
        end_y = CUR_MODE->v_res;
    
    DRAW_PIXEL_FN draw = CUR_MODE->draw_pixel;
    for( ; y < end_y; y++)
        for(x = orig_x; x < end_x; x++)
            draw(x, y, color);
}
