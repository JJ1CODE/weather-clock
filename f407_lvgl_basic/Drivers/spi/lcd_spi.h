/*
 * lcd_spi.h
 *
 *  Created on: Oct 20, 2026
 *      Author: JUNJIEC
 */

#include "main.h"
#include "cmsis_os.h"

#ifndef TASK_LCD_SPI_H_
#define TASK_LCD_SPI_H_

void  ST7789_WriteCmd(uint8_t cmd);
void  ST7789_WriteData(const uint8_t * data, uint16_t len);

void ST7789_Init(void);
void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_FillColor(uint16_t color);
void ST7789_ColorTest(void);

#endif /* TASK_LCD_SPI_H_ */
