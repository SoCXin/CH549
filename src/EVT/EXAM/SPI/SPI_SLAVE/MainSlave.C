/********************************** (C) COPYRIGHT *******************************
* File Name          : MainSlave.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/23
* Description        : CH549 SPI�豸������ʾ������SPI�������������շ����ӻ���ȡ����������ȡ��
                       Ȼ���͸�����
                       ע�����DEBUG.C/SPI.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI.H"
/*Ӳ���ӿڶ���*/
/******************************************************************************
ʹ��CH549 Ӳ��SPI�ӿ�
         CH549
         P1.4        =       SCS
         P1.5        =       MOSI
         P1.6        =       MISO
         P1.7        =       SCK
*******************************************************************************/
void main()
{
    UINT8 ret,i=0;
    CfgFsys( );
    mDelaymS(5);                                                               //�޸�ϵͳ��Ƶ�������Լ���ʱ�ȴ���Ƶ�ȶ�
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("SPI Slave start ...\n");
    SPISlvModeSet( );                                                          //SPI�ӻ�ģʽ����
    while(1)
    {
#ifndef SPI_INTERRUPT
        ret = CH549SPISlvRead();                                               //��������CS=0
        CH549SPISlvWrite(ret^0xFF);                                            //SPI�ȴ�����������ȡ��,SPI ����ÿ�ζ�֮ǰ�Ƚ�CS=0�������CS=1
        printf("Read#%02x\n",(UINT16)ret);
#endif
    }
}