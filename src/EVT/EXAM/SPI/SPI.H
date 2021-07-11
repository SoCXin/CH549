#ifndef __SPI_H__
#define __SPI_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
//#define  SPI_INTERRUPT                                                        //中断使能（仅Slave模式）
#define  SPI_CK_SET( n ) (SPI0_CK_SE = n)                                     //SPI时钟设置函数 
//Master相关函数
extern void  SPIMasterModeSet(UINT8 mode);                                    //SPI主机模式初始化
extern void  CH549SPIMasterWrite(UINT8 dat);                                  //CH549硬件SPI写数据，主机模式
extern UINT8 CH549SPIMasterRead();                                            //CH549硬件SPI0读数据，主机模式
//Slave相关函数
extern void  SPISlvModeSet( );                                                //SPI从机模式初始化
extern void  CH549SPISlvWrite(UINT8 dat);                                     //CH549硬件SPI写数据，从机模式
extern UINT8 CH549SPISlvRead();                                               //CH549硬件SPI0读数据，从机模式
#endif
