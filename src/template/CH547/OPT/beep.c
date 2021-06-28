/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    beep.c
 * @Author  xiaowine@cee0.com
 * @date
 * @version V1.0
 *************************************************
 * @brief   标注系统信息
 ****************************************************************************
 * @attention
 * Powered By Xiaowine
 * <h2><center>&copy;  Copyright(C) cee0.com 2015-2019</center></h2>
 * All rights reserved
 *
 **/
#include "beep.h"
#include "user_type.h"

bit beepFlag = 0;

_BEEP_STATE beepState = {0, 0, 0};

UINT8 beepCount     = 3;
UINT8 beepLongCount = 0;

void beepInit(void)
{
    /* 时钟 频率设置 */
    SetPWMClkDiv(4);     // PWM时钟配置,FREQ_SYS/4
    SetPWMCycle64Clk();  // PWM周期 FREQ_SYS/4/64
    SetPWM3Dat(64);
    PWM_SEL_CHANNEL(CH3, Enable);
}

void beepShortBee(void)
{
    if (BEEPMODE)
    {
        beepFlag = 1;
        beepON;
        beepCount = 0;
    }
    else
    {
        if (beepLongCount)
        {
            if (flag500ms)
            {
                if (beepFlag)
                {
                    beepFlag = 0;
                    beepOFF;
                    beepLongCount--;
                }
                else
                {
                    beepFlag = 1;
                    beepON;
                }
            }
            beepCount = 0;
            return;
        }
        if (flag63ms)
        {
            if (beepFlag)
            {
                beepFlag = 0;
                beepOFF;
            }
            else if (beepCount)
            {
                beepCount--;
                beepFlag = 1;
                beepON;
                return;
            }
        }
    }
}
