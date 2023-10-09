#pragma once
#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <stdint.h>
#include <stdio.h>
#include "define.h"
#include "graphics_card_macros.h"

int(change_mode)(uint16_t mode);
int(map_virtual_machine)(uint16_t mode);
void (draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
//int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
//void vg_draw_hline(uint16_t x, uint16_t y, uint16_t width, uint32_t color);
int vg_draw_matrix(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
uint8_t (R)(uint32_t color);
uint8_t (G)(uint32_t color);
uint8_t (B)(uint32_t color);
uint32_t (build_RGB)(uint8_t r, uint8_t g, uint8_t b);
uint8_t (get_indexed_color)(uint row, uint col, uint32_t first, uint8_t no_rectangles, uint8_t step);
uint32_t (get_direct_color)(uint row, uint col, uint32_t first, uint8_t no_rectangles, uint8_t step);
void draw_xpm(xpm_image_t img, uint16_t x, uint16_t y);
int (controller_info)(vg_vbe_contr_info_t * info);
