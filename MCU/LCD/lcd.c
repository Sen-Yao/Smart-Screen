/*********************************************************************
 * File Name          : lcd.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : file for lcd screen.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include <LCD_config.h>
#include "lcd.h"
#include "debug.h"


static uint16_t width;
static uint16_t height;

#define LCD_SOFT_RESET          (0x01)
#define LCD_READ_ID             (0x04)
#define LCD_READ_DISPLAY_STATUS (0x09)
#define LCD_ON                  (0x28)
#define LCD_OFF                 (0x29)

#define LCD_SET_X               (0x2a)
#define LCD_SET_Y               (0x2B)
#define LCD_MEM_WRITE           (0x2C)
#define LCD_MEM_READ            (0x2e)



#define CHAR_WIDTH 16
#define CHAR_HEIGHT 16


/*********************************************************************
 * @fn      LCD_WR_REG
 *
 * @brief   Write an 8-bit command to the LCD screen
 *
 * @param   data - Command value to be written
 *
 * @return  none
 */
static void LCD_WR_REG(uint8_t data)
{
    LCD_CS_CLR;
    LCD_DC_CLR;
    SPI2_Write(data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_WR_DATA
 *
 * @brief   Write an 8-bit data to the LCD screen
 *
 * @param   data - the data to write
 *
 */
static void LCD_WR_DATA(uint8_t data)
{
    LCD_CS_CLR;
    LCD_DC_SET;
    SPI2_Write(data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_WriteReg
 *
 * @brief   write a data to the register
 *
 * @param   LCD_Reg - register address
 *          LCD_RegValue - the data to write
 *
 * @return  none
 */
static void LCD_WriteReg(uint8_t LCD_Reg, uint8_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/*********************************************************************
 * @fn      Lcd_WriteData_16Bit
 *
 * @brief   write two bytes to the lcd screen
 *
 * @param   Data - the data to write
 *
 * @return  none
 */
void Lcd_WriteData_16Bit(uint16_t Data)
{
    LCD_CS_CLR;
    LCD_DC_SET;
    SPI2_Write(Data >> 8);
    SPI2_Write(Data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_GPIOInit
 *
 * @brief   Configuring the control gpio for the lcd screen
 *
 * @return  none
 */
void LCD_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LCD_LED_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_DC_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_RST_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_CS_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = LCD_LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_LED_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_DC_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_DC_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_RST_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      LCD_RESET
 *
 * @brief   reset the lcd screen by the rst pin
 *
 * @return  none
 */
void LCD_RESET(void)
{
    LCD_RST_CLR;
    Delay_Ms(100);
    LCD_RST_SET;
    Delay_Ms(50);
}

/*********************************************************************
 * @fn      LCD_Init
 *
 * @brief   Initialization LCD screen
 *
 * @return  none
 */
void LCD_Init(void)
{
    SPI2_Init();
    SPI2_DMA_Init();
    LCD_GPIOInit();
    LCD_RESET();
    LCD_LED_SET;

    LCD_WR_REG(0xCF);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC9);  //C1
    LCD_WR_DATA(0X30);
    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0X12);
    LCD_WR_DATA(0X81);
    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x85);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x7A);
    LCD_WR_REG(0xCB);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x02);
    LCD_WR_REG(0xF7);
    LCD_WR_DATA(0x20);
    LCD_WR_REG(0xEA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xC0);   //Power control
    LCD_WR_DATA(0x1B);  //VRH[5:0]
    LCD_WR_REG(0xC1);   //Power control
    LCD_WR_DATA(0x00);  //SAP[2:0];BT[3:0] 01
    LCD_WR_REG(0xC5);   //VCM control
    LCD_WR_DATA(0x30);  //3F
    LCD_WR_DATA(0x30);  //3C
    LCD_WR_REG(0xC7);   //VCM control2
    LCD_WR_DATA(0XB7);
    LCD_WR_REG(0x36);  // Memory Access Control
    LCD_WR_DATA(0x08);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);
    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1A);
    LCD_WR_REG(0xB6);  // Display Function Control
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0xA2);
    LCD_WR_REG(0xF2);  // 3Gamma Function Disable
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x26);  //Gamma curve selected
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xE0);  //Set Gamma
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x28);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0XA9);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0XE1);  //Set Gamma
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x2B);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x3f);
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xef);
    LCD_WR_REG(0x11);  //Exit Sleep
    Delay_Ms(120);
    LCD_WR_REG(0x29);  //display on

    // LCD_direction(USE_HORIZONTAL);
}

/*********************************************************************
 * @fn      LCD_SetWindows
 *
 * @brief   Setting LCD display window
 *
 * @param   xStar -  the bebinning x coordinate of the LCD display window
 *          yStar -  the bebinning y coordinate of the LCD display window
 *          xEnd -  the endning x coordinate of the LCD display window
 *          yEnd -  the endning y coordinate of the LCD display window
 *
 * @return  none
 */
void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
    LCD_WR_REG(LCD_SET_X);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(0x00FF & xStar);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(0x00FF & xEnd);

    LCD_WR_REG(LCD_SET_Y);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(0x00FF & yStar);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(0x00FF & yEnd);
    LCD_WR_REG(LCD_MEM_WRITE);
}

/*********************************************************************
 * @fn      LCD_direction
 *
 * @brief   Setting the display direction of LCD screen
 *
 * @param   direction -   direction:0-0 degree  1-90 degree  2-180 degree  3-270 degree
 *
 * @return  none
 */
void LCD_direction(uint8_t direction)
{
    switch (direction)
    {
        case 0:
            width  = LCD_W;
            height = LCD_H;
            LCD_WriteReg(0x36, (1 << 3) | (0 << 6) | (0 << 7));  //BGR==1,MY==0,MX==0,MV==0
            break;
        case 1:
            width  = LCD_H;
            height = LCD_W;
            LCD_WriteReg(0x36, (1 << 3) | (0 << 7) | (1 << 6) | (1 << 5));  //BGR==1,MY==1,MX==0,MV==1
            break;
        case 2:
            width  = LCD_W;
            height = LCD_H;
            LCD_WriteReg(0x36, (1 << 3) | (1 << 6) | (1 << 7));  //BGR==1,MY==0,MX==0,MV==0
            break;
        case 3:
            width  = LCD_H;
            height = LCD_W;
            LCD_WriteReg(0x36, (1 << 3) | (1 << 7) | (1 << 5));  //BGR==1,MY==1,MX==0,MV==1
            break;
        default:
            break;
    }
}

/*********************************************************************
 * @fn      LCD_Clear
 *
 * @brief   Full screen filled LCD screen
 *
 * @param   Color -   Filled color
 *
 * @return  none
 */
void LCD_Clear(uint16_t Color)
{
    unsigned int i, m;
    LCD_SetWindows(0, 0, width - 1, height - 1);
    LCD_CS_CLR;
    LCD_DC_SET;
    for (i = 0; i < height; i++)
    {
        for (m = 0; m < width; m++)
        {
            Lcd_WriteData_16Bit(Color);
        }
    }
    LCD_CS_SET;
}



void fill_screen_buffer(uint8_t screen_buffer[SCREEN_HEIGHT / 8][SCREEN_WIDTH], int start_row, int start_col, uint8_t* hanzi_buffer) {
    // ����ÿһ��
    for (int row = 0; row < MAX_ROWS; row++) {
        if (start_row + row * CHAR_HEIGHT >= SCREEN_HEIGHT || start_row + row * CHAR_HEIGHT < - CHAR_HEIGHT) {
            continue;
        }
        // ����ÿһ���е�ÿһ������
        for (int word = 0; word < WORD_PER_LINE; word++) {
            if (start_col + word * CHAR_WIDTH >= SCREEN_WIDTH || start_col + word * CHAR_WIDTH < - CHAR_WIDTH) {
                continue;
            }
             // ���㵱ǰ������ buffer �е���ʼ��ַ
            // ����ÿ�����ֵ�ÿ���ֽڣ�32�ֽڣ���һ���ֽڸ����˺���� 8 �����أ����������Ҫ��� 8 �����ص�ֵ������д�� screen_buffer �Ķ�Ӧλ�á�
            for (int byte = 0; byte < BITE_PER_WORD; byte++) {
                // �����ֽڵ�ֵ
                uint8_t value = hanzi_buffer[(row * WORD_PER_LINE + word) * BITE_PER_WORD + byte];
                // ��ǰ�ֽڵĺ��� 8 �����У����������������
                int pixel_row = start_row + row * CHAR_HEIGHT + byte / 2;
                // ��ǰ�ֽڵĺ��� 8 �����У����������������
                int pixel_col = start_col + word * CHAR_WIDTH + (CHAR_WIDTH / 2) * (byte % 2);

                // printf("value=%2x", value)
                // ���ں��� 8 �����е�ÿһλ���Ӹߵ��ͣ���������
                for (int i = 7; i >= 0; i--) {
                    if (pixel_col + 7 - i > SCREEN_WIDTH || pixel_col + 7 - i <= 0){
                    	continue;
                    }
                    // ʹ��λ������ÿһλ���Ӹߵ��͡����λ��ʾ���������ء���Ҫд���λ�÷ֲ��ڲ�ͬ�� screen_buffer λ����
                    // bit ��ʾ�ӵ͵���λ�ĵ� i λ�ı���
                    uint8_t bit_value = (value >> i) & 1;
                    // �˱��ض�Ӧ�������� screen_buffer[pixel_row / 8][pixel_col + i] ����ֽڵĴӸߵ������� pixel_row % 8 ������
                    // ʹ����Ԫ������������Ŀ��λ�õ�λ���ٽ��л����㸳ֵ
                    // screen_buffer ���λ�����Ϸ������λ�����·�
                    screen_buffer[pixel_row / 8][pixel_col + 7 - i] = (screen_buffer[pixel_row / 8][pixel_col + 7 - i] & ~(1 << (7 - pixel_row % 8))) | (bit_value << (7 - pixel_row % 8));
                    // printf("[%d,%d]��Ԫ��ֵΪ%2x, �Ѿ�����%d����Ϊ%d\n",pixel_row / 8, pixel_col + 7 - i, screen_buffer[pixel_row / 8][pixel_col + 7 - i], 8 - pixel_row % 8, bit_value);
                }
            }
        }
    }
}


void draw_screen_buffer(uint8_t screen_buffer[SCREEN_HEIGHT/8][SCREEN_WIDTH], uint16_t word_color)
{
    // ������Ļ������
    // ������Ԫ��
	for (int x = 0; x < SCREEN_WIDTH; x++) {
		for (int y = 0; y < SCREEN_HEIGHT / 8; y++) {
            // ��ȡ��ǰ�ֽڵ������ 8 �����ص�ֵ
            // x�� y ��ʾ��ǰ�ֽڶ�Ӧ�����Ϸ����ص��к͸����С���ʱ�ֽڵ����λ��ʾ�����ص�������;
			LCD_SetWindows(x, 8 * y, x, 8 * y+8);
			for (int bit = 7; bit >= 0; bit--) {
				Lcd_WriteData_16Bit((screen_buffer[y][x] & (1 << bit)) ? word_color : 0x0000);
			}
        }
    }
}



int global_scrolling_step = 0;
uint8_t screen_buffer[SCREEN_HEIGHT / 8][SCREEN_WIDTH] = {0};
uint8_t global_num_rows = 1;


void scroll_hanzi_display(uint8_t* hanzi_buffer, ScrollEffect effect, int step_size, uint16_t word_color, uint8_t num_row, uint8_t num_col)
{

    // hanzi_rows: ��������
    // hanzi_cols: ��������
    memset(screen_buffer, 0, sizeof(screen_buffer));
    switch (effect) {
        case SCROLL_LEFT_TO_RIGHT:
            fill_screen_buffer(screen_buffer, 10, global_scrolling_step, hanzi_buffer);
            if (global_scrolling_step >= SCREEN_WIDTH){
                    global_scrolling_step = - num_col * CHAR_WIDTH;
                }
			else {
				global_scrolling_step += step_size;
			}
            break;
        case SCROLL_RIGHT_TO_LEFT:
        	fill_screen_buffer(screen_buffer, 10, global_scrolling_step, hanzi_buffer);
        	if (global_scrolling_step <= - num_col * CHAR_WIDTH){
					global_scrolling_step = SCREEN_WIDTH;
				}
			else {
				global_scrolling_step -= step_size;
			}
			break;
        case SCROLL_TOP_TO_BOTTOM:
            fill_screen_buffer(screen_buffer, global_scrolling_step, 10, hanzi_buffer);
        	if (global_scrolling_step >= SCREEN_HEIGHT){
					global_scrolling_step = -num_row * CHAR_HEIGHT;
				}
			else {
				global_scrolling_step += step_size;
			}

            break;
		case SCROLL_BOTTOM_TO_TOP:
			fill_screen_buffer(screen_buffer, global_scrolling_step, 10, hanzi_buffer);
			if (global_scrolling_step <= - num_row * CHAR_HEIGHT){
					global_scrolling_step = SCREEN_HEIGHT;
				}
			else {
				global_scrolling_step -= step_size;
			}
			break;
    }
    draw_screen_buffer(screen_buffer, word_color);

}

