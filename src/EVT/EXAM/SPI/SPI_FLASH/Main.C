/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/29
* Description        : CH549 SPI Flash��д������ʾ
                       ע�����DEBUG.C/SPI.C/SPIFlash.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI_FLASH\SPIFlash.H"
#pragma  NOAREGS
UINT8X buf[512];
void main( )
{
    UINT16X i;
    CfgFsys( );                                                                //CH549ʱ��ѡ������
    mDelaymS(20);
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("SPI Flash demo start ...\n");
    for(i=0; i!=512; i++)
    {
        buf[i] = i;
    }
    SPIFlash_Init();
    EraseExternal4KFlash_SPI(0);
    BlukWriteExternalFlash_SPI(3,512,buf);
    BlukReadExternalFlash_SPI( 0,512,buf );
    for(i=0; i!=512; i++)
    {
        printf("%02x ",(UINT16)buf[i]);
    }
    printf("done\n");
    while(1)
    {
    }
}
