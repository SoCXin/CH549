#ifndef __SPI_H__
#define __SPI_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
//#define  SPI_INTERRUPT                                                        //�ж�ʹ�ܣ���Slaveģʽ��
#define  SPI_CK_SET( n ) (SPI0_CK_SE = n)                                     //SPIʱ�����ú��� 
//Master��غ���
extern void  SPIMasterModeSet(UINT8 mode);                                    //SPI����ģʽ��ʼ��
extern void  CH549SPIMasterWrite(UINT8 dat);                                  //CH549Ӳ��SPIд���ݣ�����ģʽ
extern UINT8 CH549SPIMasterRead();                                            //CH549Ӳ��SPI0�����ݣ�����ģʽ
//Slave��غ���
extern void  SPISlvModeSet( );                                                //SPI�ӻ�ģʽ��ʼ��
extern void  CH549SPISlvWrite(UINT8 dat);                                     //CH549Ӳ��SPIд���ݣ��ӻ�ģʽ
extern UINT8 CH549SPISlvRead();                                               //CH549Ӳ��SPI0�����ݣ��ӻ�ģʽ
#endif