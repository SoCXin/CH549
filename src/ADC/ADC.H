
#ifndef __ADC_H__
#define __ADC_H__
#include "CH549.H"
#include "DEBUG.H"
#define  EN_ADC_INT                     0                                                 //ADC中断使能开关
//通道号选择,分别对应P1.0~P1.7, P0.0~P0.7
#define  CH0                            0
#define  CH1                            1
#define  CH2                            2
#define  CH3                            3
#define  CH4                            4
#define  CH5                            5
#define  CH6                            6
#define  CH7                            7
#define  CH8                            8
#define  CH9                            9
#define  CH10                           0x0A
#define  CH11                           0x0B
#define  CH12                           0x0C
#define  CH13                           0x0D
#define  CH14                           0x0E
#define  CH15                           0x0F
#define  CHALL                          0xFFFF
//快速调用宏定义
#define  ADC_ExChannelSelect( ch )      ADC_CHAN = ADC_CHAN & ~MASK_ADC_CHAN | ch         //外部通道取值 0~15
#define  ADC_StartSample( )             ADC_CTRL = bADC_START                             //启动一次ADC采样
//ADC相关子函数
extern void ADC_ExInit( UINT8 AdcClk );
extern void ADC_ChSelect( UINT8 ch );
extern void ADC_InTSInit(void);
#endif
