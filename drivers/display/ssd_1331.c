/*
* /drivers/display/ssd_1331.c
* A library for RGB OLED module
*
* Copyright (c) 2014 seeed technology inc.
* Copyright (c) 2012, Adafruit Industries.
*
* All rights reserved.
*
* This library is based on Adafruit's SSD1331-OLED-Driver-Library. Thanks to 
* their contribution to the code, we modify it and add more interface to 
* support our Seeed's Xadow RGB OLED 96*64 module.
*
* Below is the introduction of Adafruit's Color OLED module, we add it to here
* to express our thanks to them.
*
* ****************************************************************************
* This is a library for the 0.96" 16-bit Color OLED with SSD1331 driver chip
*
*  Pick one up today in the adafruit shop!
*  ------> http://www.adafruit.com/products/684
*
* These displays use SPI to communicate.
*
* Adafruit invests time and resources providing this open source code, 
* please support Adafruit and open-source hardware by purchasing 
* products from Adafruit!
*
* Written by Limor Fried/Ladyada for Adafruit Industries.
* Modifed by lawliet for Seeed Studio's RGB OLED module.
* BSD license, all text above must be included in any redistribution
* ******************************************************************************
*
* Software License Agreement (BSD License)
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holders nor the
* names of its contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#include <board.h>

#include "ssd_1331.h"

static ssd1331_config_t g_ssd1331_config;

static void spi_send_command(uint8_t c)
{
    gpio_toogle(0, g_ssd1331_config.dc_pin, g_ssd1331_config.dc_port);
    gpio_toogle(0, g_ssd1331_config.cs_pin, g_ssd1331_config.cs_port);

    spi_send_recv(g_ssd1331_config.spi_dev, &c, 1, NULL, 0);

    gpio_toogle(1, g_ssd1331_config.cs_pin, g_ssd1331_config.cs_port);
}

void ssd1331_display_init(ssd1331_config_t *config)
{
    gpio_configure(config->cs_pin, 0, GPIO_DIRECTION_OUT);
    gpio_configure(config->dc_pin, 0, GPIO_DIRECTION_OUT); 
 
    g_ssd1331_config = *config;

    spi_send_command(CMD_DISPLAY_OFF);          //Display Off
    spi_send_command(CMD_SET_CONTRAST_A);       //Set contrast for color A
    spi_send_command(0x91);                     //145
    spi_send_command(CMD_SET_CONTRAST_B);       //Set contrast for color B
    spi_send_command(0x50);                     //80
    spi_send_command(CMD_SET_CONTRAST_C);       //Set contrast for color C
    spi_send_command(0x7D);                     //125
    spi_send_command(CMD_MASTER_CURRENT_CONTROL);//master current control
    spi_send_command(0x06);                     //6
    spi_send_command(CMD_SET_PRECHARGE_SPEED_A);//Set Second Pre-change Speed For ColorA
    spi_send_command(0x64);                     //100
    spi_send_command(CMD_SET_PRECHARGE_SPEED_B);//Set Second Pre-change Speed For ColorB
    spi_send_command(0x78);                     //120
    spi_send_command(CMD_SET_PRECHARGE_SPEED_C);//Set Second Pre-change Speed For ColorC
    spi_send_command(0x64);                     //100
    spi_send_command(CMD_SET_REMAP);            //set remap & data format
    spi_send_command(0x72);                     //0x72              
    spi_send_command(CMD_SET_DISPLAY_START_LINE);//Set display Start Line
    spi_send_command(0x0);
    spi_send_command(CMD_SET_DISPLAY_OFFSET);   //Set display offset
    spi_send_command(0x0);
    spi_send_command(CMD_NORMAL_DISPLAY);       //Set display mode
    spi_send_command(CMD_SET_MULTIPLEX_RATIO);  //Set multiplex ratio
    spi_send_command(0x3F);
    spi_send_command(CMD_SET_MASTER_CONFIGURE); //Set master configuration
    spi_send_command(0x8E);
    spi_send_command(CMD_POWER_SAVE_MODE);      //Set Power Save Mode
    spi_send_command(0x00);                     //0x00
    spi_send_command(CMD_PHASE_PERIOD_ADJUSTMENT);//phase 1 and 2 period adjustment
    spi_send_command(0x31);                     //0x31
    spi_send_command(CMD_DISPLAY_CLOCK_DIV);    //display clock divider/oscillator frequency
    spi_send_command(0xF0);
    spi_send_command(CMD_SET_PRECHARGE_VOLTAGE);//Set Pre-Change Level
    spi_send_command(0x3A);
    spi_send_command(CMD_SET_V_VOLTAGE);        //Set vcomH
    spi_send_command(0x3E);
    spi_send_command(CMD_DEACTIVE_SCROLLING);   //disable scrolling
    spi_send_command(CMD_NORMAL_BRIGHTNESS_DISPLAY_ON);//set display on
}

void ssd1331_display_drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x < 0) || (x >= RGB_OLED_WIDTH) || (y < 0) || (y >= RGB_OLED_HEIGHT))
        return;
    //set column point
    spi_send_command(CMD_SET_COLUMN_ADDRESS);
    spi_send_command(x);
    spi_send_command(RGB_OLED_WIDTH-1);
    //set row point
    spi_send_command(CMD_SET_ROW_ADDRESS);
    spi_send_command(y);
    spi_send_command(RGB_OLED_HEIGHT-1);

    //fill 16bit colour

    gpio_toogle(1, g_ssd1331_config.dc_pin, g_ssd1331_config.dc_port);
    gpio_toogle(0, g_ssd1331_config.cs_pin, g_ssd1331_config.cs_port);

    spi_send_recv(g_ssd1331_config.spi_dev, &color, sizeof(color), NULL, 0);

    gpio_toogle(1, g_ssd1331_config.cs_pin, g_ssd1331_config.cs_port);
}

void ssd1331_display_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    if((x0 < 0) || (y0 < 0) || (x1 < 0) || (y1 < 0))
        return;

    if (x0 >= RGB_OLED_WIDTH)  x0 = RGB_OLED_WIDTH - 1;
    if (y0 >= RGB_OLED_HEIGHT) y0 = RGB_OLED_HEIGHT - 1;
    if (x1 >= RGB_OLED_WIDTH)  x1 = RGB_OLED_WIDTH - 1;
    if (y1 >= RGB_OLED_HEIGHT) y1 = RGB_OLED_HEIGHT - 1;

    spi_send_command(CMD_DRAW_LINE);//draw line
    spi_send_command(x0);//start column
    spi_send_command(y0);//start row
    spi_send_command(x1);//end column
    spi_send_command(y1);//end row
    spi_send_command((uint8_t)((color>>11)&0x1F));//R
    spi_send_command((uint8_t)((color>>5)&0x3F));//G
    spi_send_command((uint8_t)(color&0x1F));//B
}

void ssd1331_display_drawFrame(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t outColor, uint16_t fillColor)
{
    if((x0 < 0) || (y0 < 0) || (x1 < 0) || (y1 < 0))
        return;

    if (x0 >= RGB_OLED_WIDTH)  x0 = RGB_OLED_WIDTH - 1;
    if (y0 >= RGB_OLED_HEIGHT) y0 = RGB_OLED_HEIGHT - 1;
    if (x1 >= RGB_OLED_WIDTH)  x1 = RGB_OLED_WIDTH - 1;
    if (y1 >= RGB_OLED_HEIGHT) y1 = RGB_OLED_HEIGHT - 1;

    spi_send_command(CMD_FILL_WINDOW);//fill window
    spi_send_command(ENABLE_FILL);
    spi_send_command(CMD_DRAW_RECTANGLE);//draw rectangle
    spi_send_command(x0);//start column
    spi_send_command(y0);//start row
    spi_send_command(x1);//end column
    spi_send_command(y1);//end row
    spi_send_command((uint8_t)((outColor>>11)&0x1F));//R
    spi_send_command((uint8_t)((outColor>>5)&0x3F));//G
    spi_send_command((uint8_t)(outColor&0x1F));//B
    spi_send_command((uint8_t)((fillColor>>11)&0x1F));//R
    spi_send_command((uint8_t)((fillColor>>5)&0x3F));//G
    spi_send_command((uint8_t)(fillColor&0x1F));//B
}

void ssd1331_display_copyWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2)
{
    spi_send_command(CMD_COPY_WINDOW);//copy window
    spi_send_command(x0);//start column
    spi_send_command(y0);//start row
    spi_send_command(x1);//end column
    spi_send_command(y1);//end row
    spi_send_command(x2);//new column
    spi_send_command(y2);//new row
}

void ssd1331_display_dimWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    spi_send_command(CMD_DIM_WINDOW);//copy area
    spi_send_command(x0);//start column
    spi_send_command(y0);//start row
    spi_send_command(x1);//end column
    spi_send_command(y1);//end row
}

void ssd1331_display_clearWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    spi_send_command(CMD_CLEAR_WINDOW);//clear window
    spi_send_command(x0);//start column
    spi_send_command(y0);//start row
    spi_send_command(x1);//end column
    spi_send_command(y1);//end row
}

void ssd1331_display_setScolling(ScollingDirection direction, uint8_t rowAddr, uint8_t rowNum, uint8_t timeInterval)
{
    uint8_t scolling_horizontal = 0x0;
    uint8_t scolling_vertical = 0x0;
    switch(direction){
        case Horizontal:
            scolling_horizontal = 0x01;
            scolling_vertical = 0x00;
            break;
        case Vertical:
            scolling_horizontal = 0x00;
            scolling_vertical = 0x01;
            break;
        case Diagonal:
            scolling_horizontal = 0x01;
            scolling_vertical = 0x01;
            break;
        default:
            break;
    }

    spi_send_command(CMD_CONTINUOUS_SCROLLING_SETUP);
    spi_send_command(scolling_horizontal);
    spi_send_command(rowAddr);
    spi_send_command(rowNum);
    spi_send_command(scolling_vertical);
    spi_send_command(timeInterval);
    spi_send_command(CMD_ACTIVE_SCROLLING);
}

void ssd1331_display_enableScolling(bool enable)
{
    if(enable)
        spi_send_command(CMD_ACTIVE_SCROLLING);
    else
        spi_send_command(CMD_DEACTIVE_SCROLLING);
}

void ssd1331_display_setDisplayMode(DisplayMode mode)
{
    spi_send_command(mode);
}

void ssd1331_display_setDisplayPower(DisplayPower power)
{
    spi_send_command(power);
}
