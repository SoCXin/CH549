/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 ����1~3 ��ѯ���ж��շ���ʾ��ʵ�����ݻػ�
                      ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\UART\UART.H"
#pragma  NOAREGS
void main( )
{
    UINT8 dat;
    CfgFsys( );                                                                //CH549ʱ��ѡ������
    mDelaymS(20);
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("UART demo start ...\n");
    CH549UART1Init();                                                          //����1��ʼ��
    CH549UART1Alter();                                                         //����1����ӳ��
    CH549UART2Init();                                                          //����2��ʼ��
    CH549UART3Init();                                                          //����3��ʼ��
    while(1)
    {
#ifndef UART1_INTERRUPT
        dat = CH549UART1RcvByte();
        CH549UART1SendByte(dat);
#endif
#ifndef UART2_INTERRUPT
        dat = CH549UART2RcvByte();
        CH549UART2SendByte(dat);
#endif
#ifndef UART3_INTERRUPT
        dat = CH549UART3RcvByte();
        CH549UART3SendByte(dat);
#endif
    }
}