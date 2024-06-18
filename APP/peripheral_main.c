/********************************** (C) COPYRIGHT *******************************
 * File Name          : peripheral_main.c
 * Author             : Senyao
 * Version            : V1.2
 * Date               : 2024/06/09
 * Description        : Peripheral slave application main function and task system initialization
 *********************************************************************************

/******************************************************************************/
/* Header file contains */
#include "CONFIG.h"
#include "HAL.h"
#include "gattprofile.h"
#include "peripheral.h"
#include "LCD_config.h"
#include "LCD.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if(defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] = {0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif

/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   Main loop
 *
 * @return  none
 */
__attribute__((section(".highcode")))
__attribute__((noinline))
void Main_Circulation(void)
{
    while(1)
    {
        TMOS_SystemProcess();
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main function
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();

#ifdef DEBUG
    USART_Printf_Init( 115200 );
#endif
    // ��ʼ�� BLE
    WCHBLE_Init();
    // ��ʼ��Ӳ����
    HAL_Init();
    // ��ʼ�� GAP ��ɫ
    GAPRole_PeripheralInit();
    // ��ʼ����Χ�豸
    Peripheral_Init();

    // ��ʼ�� LCD
    LCD_Init();
	LCD_direction(1);
	LCD_Clear(0x0000);
	// ������ѭ��
    Main_Circulation();
}

/******************************** endfile @ main ******************************/
