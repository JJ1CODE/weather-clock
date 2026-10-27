#include "lvgl_port.h"

#include "lcd_spi.h"
#include "lvgl/lvgl.h"

#define LCD_WIDTH  240
#define LCD_HEIGHT 320
#define DRAW_LINES 20

/* One partial RGB565 buffer; four-byte alignment is required by LVGL. */
static uint8_t draw_buffer[LCD_WIDTH * DRAW_LINES * 2] __attribute__((aligned(4)));

static void lcd_flush(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    uint32_t width = (uint32_t)(area->x2 - area->x1 + 1);
    uint32_t height = (uint32_t)(area->y2 - area->y1 + 1);
    uint16_t byte_count = (uint16_t)(width * height * 2U);

    ST7789_SetWindow((uint16_t)area->x1, (uint16_t)area->y1,(uint16_t)area->x2, (uint16_t)area->y2);
    ST7789_WriteData(pixels, byte_count);

    /* SPI transmit is blocking, so LVGL can reuse the buffer now. */
    lv_display_flush_ready(display);
}

bool LVGL_Port_Init(void)
{
    lv_display_t *display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    if (display == NULL) {
        return false;
    }

    /* ST7789 expects the high RGB565 byte first on its 8-bit SPI bus. */
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565_SWAPPED);
    lv_display_set_buffers(display, draw_buffer, NULL, sizeof(draw_buffer),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, lcd_flush);
    return true;
}
