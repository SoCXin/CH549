/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH547 GPIO功能演示,普通输入输出测试；中断触发模式
                       注意包含DEBUG.C
*******************************************************************************/
#include ".\Public\CH547.H"
#include ".\Public\DEBUG.H"
#include ".\Timer\Timer.H"
#include ".\TouchKey\TouchKey.H"
#include "beep.h"
#include "user_type.h"
#include "serial.h"
#include "led.h"

#pragma NOAREGS
// sbit LED2 = P2 ^ 2;
sbit LED3 = P2 ^ 3;
sbit LED4 = P2 ^ 4;
sbit LED5 = P2 ^ 5;

volatile _TKS_FLAGA_type bitFlag;

/*******************************************************************************
 * Function Name  : LED_Port_Init
 * Description    : LED引脚初始化,推挽输出
 *                  P22~P25
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void getBitFlag(void)
{
    if (flag10)
    {
        flag10   = 0;
        flag10ms = 1;
    }
    if (flag63)
    {
        flag63   = 0;
        flag63ms = 1;
    }
    if (flag250)
    {
        flag250   = 0;
        flag250ms = 1;
    }
    if (flag500)
    {
        flag500   = 0;
        flag500ms = 1;
    }
}
void bitClear(void)
{
    flag10ms  = 0;
    flag63ms  = 0;
    flag250ms = 0;
    flag500ms = 0;
}

void main()
{
    CfgFsys();  // CH547时钟选择配置
    mDelaymS(20);
    mInitSTDIO();  //串口0初始化
    printf("Start @ChipID=%02X\n", (UINT16)CHIP_ID);
    LED_Port_Init();
    CH547WDTModeSelect(1);
#ifdef T0_INT
    timer0Init();
#endif
    TouchKey_Init();
    beepInit();
    serialInit();

    while (1)
    {
        if (flag1ms)
        {
            flag1ms = 0;
            getBitFlag();
        }
        if (flag10ms)
        {
            getKeyBitMap();
            if (KEY2)
            {
                printf("key2\n");
            }
        }
        beepShortBee();
        serialOpt();
        touchKeyGet();
        if (flag250ms)
        {
            CH547WDTFeed(0);
        }
        ledDisplay();
        if (bitFlag.byte)
        {
            bitClear();
        }
    }
}
