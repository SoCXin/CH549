/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 GPIO功能演示,普通输入输出测试；中断触发模式
                        注意包含DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\GPIO\GPIO.H"
#pragma  NOAREGS
sbit LED2 = P1^6;
sbit LED3 = P1^7;

void main()
{
    CfgFsys( );                                                               //CH549时钟选择配置
    mDelaymS(20);
    mInitSTDIO( );                                                            //串口0初始化
    printf("GPIO demo blink start ...\n");
    /* 配置GPIO */
    GPIO_Init( PORT1,PIN7,MODE1);                                              //P1.0上拉输入
    GPIO_Init( PORT1,PIN6,MODE1);                                              //P1.4推挽输出
    while(1)
    {
        LED2 = ~LED2;
        printf("loop ...\n");
        mDelaymS(300);
        LED3 = ~LED3;
    }
}
