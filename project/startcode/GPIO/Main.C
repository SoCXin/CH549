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
sbit LED2 = P2^2;
sbit LED3 = P2^3;
sbit LED4 = P2^4;
sbit LED5 = P2^5;
void main()
{
    CfgFsys( );                                                               //CH549时钟选择配置
    mDelaymS(20);
    mInitSTDIO( );                                                            //串口0初始化
    printf("GPIO demo start ...\n");
    /* 配置GPIO */
    GPIO_Init( PORT1,PIN0,MODE3);                                              //P1.0上拉输入
    GPIO_Init( PORT1,PIN4,MODE1);                                              //P1.4推挽输出
    /* 配置外部中断 */
    GPIO_Init( PORT0,PIN3,MODE3);                                              //P03上拉输入
    GPIO_Init( PORT1,PIN5,MODE3);                                              //P15上拉输入
    GPIO_Init( PORT3,PIN2,MODE3);                                              //P32(INT0)上拉输入
    GPIO_Init( PORT3,PIN3,MODE3);                                              //P33(INT1)上拉输入
    GPIO_INT_Init( (INT_P03_L|INT_P15_L|INT_INT0_L|INT_INT1_L),INT_EDGE,Enable); //外部中断配置
    while(1)
    {
        LED2 = ~LED2;
        LED3 = ~LED3;
        LED4 = ~LED4;
        LED5 = ~LED5;
        mDelaymS(100);
    }
}
