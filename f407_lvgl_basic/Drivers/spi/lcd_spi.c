/*
 * lcd_spi.c
 *
 *  Created on: Oct 20, 2026
 *      Author: JUNJIEC
 */
#include "lcd_spi.h"
#include "main.h"       // 包含 HAL 库定义和引脚宏

#define LCD_W 240
#define LCD_H 320
#define CS_Down()  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)//选中
#define CS_UP()  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)

#define DC_UP() HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)// 发送数据
#define DC_Down() HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)//发送命令


extern SPI_HandleTypeDef hspi1;


	 void  ST7789_WriteCmd(uint8_t cmd)
	{

		   CS_Down();
		    DC_Down();
		    HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
		    CS_UP();
	}
	 void  ST7789_WriteData(const uint8_t* data,uint16_t len)
	{

		 	CS_Down();
		    DC_UP();
		    HAL_SPI_Transmit(&hspi1, (uint8_t *)data, len, 1000);
		    CS_UP();

	}

	void ST7789_Init(void)
	{
		 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

		    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
		    HAL_Delay(20);
		    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
		    HAL_Delay(120);

		    ST7789_WriteCmd(0x11);              // SLPOUT：退出休眠
		    HAL_Delay(120);

		    ST7789_WriteCmd(0x3A);              // COLMOD：像素格式
		    uint8_t format = 0x55;              // RGB565，16 bit/像素
		    ST7789_WriteData(&format, 1);

		    ST7789_WriteCmd(0x36);              // MADCTL：方向、RGB/BGR
		    uint8_t direction = 0x40;
		    ST7789_WriteData(&direction, 1);

		    ST7789_WriteCmd(0x29);              // DISPON：开显示
		    HAL_Delay(20);

		    HAL_GPIO_WritePin(LCD_Blacklight_GPIO_Port,LCD_Blacklight_Pin, GPIO_PIN_SET);


	}
	void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
	{
		 uint8_t data[4];

		    ST7789_WriteCmd(0x2A);              // CASET：列范围
		    data[0] = x0 >> 8; data[1] = x0;
		    data[2] = x1 >> 8; data[3] = x1;
		    ST7789_WriteData(data, 4);

		    ST7789_WriteCmd(0x2B);              // RASET：行范围
		    data[0] = y0 >> 8; data[1] = y0;
		    data[2] = y1 >> 8; data[3] = y1;
		    ST7789_WriteData(data, 4);

		    ST7789_WriteCmd(0x2C);              // RAMWR：接下来写像素
	}
	void ST7789_FillColor(uint16_t color)
	{

		 uint8_t pixels[256];                 // 一次发送 128 个像素
		    for (uint16_t i = 0; i < 128; i++) {
		        pixels[2 * i]     = color >> 8;  // 高字节先发
		        pixels[2 * i + 1] = color;
		    }

		    ST7789_SetWindow(0, 0, LCD_W - 1, LCD_H - 1);
		    for (uint32_t i = 0; i < (LCD_W * LCD_H) / 128; i++) {
		        ST7789_WriteData(pixels, sizeof(pixels));
		    }


	}
	void ST7789_ColorTest(void)
	{
		  ST7789_FillColor(0xF800); // 红
		    HAL_Delay(1000);
		    ST7789_FillColor(0x07E0); // 绿
		    HAL_Delay(1000);
		    ST7789_FillColor(0x001F); // 蓝



	}
