/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    serial.c
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

#include "serial.h"
#include "led.h"
#include "beep.h"
#include ".\TouchKey\TouchKey.H"

UINT8 rxCount            = 0;
UINT8 rxStep             = 0;
UINT8 rxBuff[30]         = {0};
UINT8 txBuff[30]         = {0};
UINT8 txCount            = 0;
bit txComplete           = 1;
bit rxDone               = 0;
UINT8C protocolHeader[2] = {0xff, 0xa5};

void serialInit(void)
{
    IP_EX |= bIP_UART1;
    CH549UART1Init();
}

void serialOpt(void)
{
    serialSend();
    serialRxReceive();
}
UINT8 getCheckSum(UINT8* dat)
{
    UINT8 checkSum = 0;
    UINT8 i;
    for (i = 0; i < (*(dat + 3) + 4); i++)
    {
        checkSum += *(dat + i);
    }
    return checkSum;
}
void serialSend(void)
{
    static UINT8 index = 0;

    if (rxDone && (txCount == 0))
    {
        rxDone    = 0;
        txBuff[0] = 0xff;
        txBuff[1] = 0xa5;
        txBuff[2] = CMD_KEY;
        txBuff[3] = 0x04;
        txBuff[4] = k_count[0];
        txBuff[5] = (k_count[0] >> 8);
        txBuff[6] = k_count[1];
        txBuff[7] = (k_count[1] >> 8);
        txBuff[8] = getCheckSum(txBuff);
        txCount   = 9;
    }

    if (SIF1 & bU1TI)
    {
        SIF1       = bU1TI;  //清除接收完中断
        txComplete = 1;
    }

    if (txComplete && (index < txCount))
    {
        SBUF1 = txBuff[index++];
        if (index == txCount)
        {
            index   = 0;
            txCount = 0;
        }
        txComplete = 0;
    }
}

void serialRxReceive(void)
{
again:
    if (rxStep == 0)
    {
        if (rxCount < 2)
            goto rxContinue;
        if (memcmp(rxBuff, (void*)protocolHeader, 2) == 0)
            rxStep = 1;
        else
        {
            memcpy(rxBuff, rxBuff + 1, rxCount - 1);
            rxCount--;
            goto again;
        }
    }

    if (rxStep == 1)
    {
        if (rxCount < 4)
            goto rxContinue;
        if (rxBuff[3] <= 15)
            rxStep = 2;
        else
        {
            memcpy(rxBuff, rxBuff + 2, rxCount - 2);
            rxCount -= 2;
            rxStep = 0;
            goto again;
        }
    }

    if (rxStep == 2)
    {
        UINT8 checkSum, checkIndex;
        if (rxCount < rxBuff[3] + 5)
            goto rxContinue;
        checkSum   = getCheckSum(rxBuff);
        checkIndex = rxBuff[3] + 4;
        if (checkSum == rxBuff[checkIndex])
        {
            UINT8 len = rxBuff[3] + 5;
            serialRxProcess(rxBuff);
            memcpy(rxBuff, rxBuff + len, rxCount - len);
            rxCount -= len;
            rxStep = 0;
            rxDone = 1;
        }
        else
        {
            memcpy(rxBuff, rxBuff + 4, rxCount - 4);
            rxCount -= 4;
            rxStep = 0;
            goto again;
        }
    }
rxContinue:
    return;
}

void serialRx(UINT8 dat)
{
    rxBuff[rxCount] = dat;
    rxCount++;
}

void serialRxProcess(UINT8* serialDataIn)
{
    switch (*(serialDataIn + 2))
    {
        case CMD_IDEL:
            break;
        case CMD_KEY:
            break;
        case CMD_LED:
            beepState.byte   = *(serialDataIn + 4);
            ledState[0].byte = *(serialDataIn + 5);
            ledState[1].byte = *(serialDataIn + 6);
            ledState[2].byte = *(serialDataIn + 7);
            ledState[3].byte = *(serialDataIn + 8);

            if (BEEPMODE)
            {
                beepCount     = 0;
                beepLongCount = 0;
            }
            else
            {
                beepCount += BEEPSHORT;
                beepLongCount += BEEPLONG;
            }
            break;
        case CMD_REG_UP:
            break;
        case CMD_REG_DOWN:
            break;
    }
}