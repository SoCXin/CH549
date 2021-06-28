#ifndef __SERIAL_H_
#define __SERIAL_H_
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
#include ".\UART\UART.H"
#include "user_type.h"

void serialOpt(void);
void serialInit(void);
void serialSend(void);
void serialRxReceive(void);
void serialRxProcess(UINT8* serialDataIn);

#endif
