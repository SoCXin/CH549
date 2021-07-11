/********************************** (C) COPYRIGHT *******************************
* File Name          : MainMaster.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/23
* Description        : CH549 SPI������CH549 SPI�ӻ�ͨѶ�������������ݣ��ӻ�ȡ������
                       �ӻ��ο�MainSlave.C
                       ע�����DEBUG.C/SPI.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI.H"
#pragma  NOAREGS
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
    mDelaymS(20);                                                              //������Ƶ�������Լ���ʱ�ȴ��ڲ�ʱ���ȶ�
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("SPI Master start ...\n");
    SPIMasterModeSet(3);                                                       //SPI����ģʽ���ã�ģʽ3
    SPI_CK_SET(12);                                                            //12��Ƶ
    while(1)
    {
        SCS = 0;                                                               //SPI������������
        CH549SPIMasterWrite(i);
        mDelaymS(5);
        ret = CH549SPIMasterRead();                                            //����SPI�ӻ����ص����ݣ�ȡ������
        SCS = 1;
        if(ret != (i^0xff))
        {
            printf("Err: %02X  %02X  \n",(UINT16)i,(UINT16)ret);               //��������ڷ������ݵ�ȡ������ӡ������Ϣ
        }
        else
        {
            printf("success %02x\n",(UINT16)i);                                //ÿ�ɹ�40�δ�ӡһ��
        }
        i = i+1;
        mDelaymS(50);
    }
}