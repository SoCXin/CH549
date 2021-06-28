#ifndef __BEEP_H_
#define __BEEP_H_
/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File
 * @Author  xiaowine@cee0.com
 * @date
 * @version V1.0
 *************************************************
 * @brief   -->>
 ****************************************************************************
 * @attention
 * Powered By Xiaowine
 * <h2><center>&copy;  Copyright(C) cee0.com 2015-2019</center></h2>
 * All rights reserved
 *
 **/

#include ".\Public\CH547.H"
#include ".\Public\DEBUG.H"
#include ".\GPIO\GPIO.H"
#include ".\Timer\Timer.H"
#include ".\TouchKey\TouchKey.H"
#include ".\UART\UART.H"
#include ".\PWM\PWM.H"

#define beepON SetPWM3Dat(32)
#define beepOFF SetPWM3Dat(64)
typedef struct
{
    unsigned char mode : 1;
    unsigned char bits3 : 3;
    unsigned char bits4 : 4;
} _BEEP_bits;

typedef union
{
    _BEEP_bits bits;
    unsigned char byte;
} _BEEP_STATE;

extern _BEEP_STATE beepState;
#define BEEPMODE beepState.bits.mode
#define BEEPLONG beepState.bits.bits3
#define BEEPSHORT beepState.bits.bits4
extern UINT8 beepCount;
extern UINT8 beepLongCount;

void beepInit(void);
void beepShortBee(void);
#endif
