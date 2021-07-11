/********************************** (C) COPYRIGHT *******************************
* File Name          : SPIFlash.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/29
* Description        : CH549 SPI Flash�����ӿں���(W25QXX)
*******************************************************************************/
#include ".\SPI\SPI_FLASH\SPIFlash.H"
/********************************* ���Ŷ��� ************************************
*    P14  <===========>  SCS
*    P15  <===========>  DI/MOSI
*    P16  <===========>  DO/MISO
*    P17  <===========>  SCK
*******************************************************************************/
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : ReadExternalFlashStatusReg_SPI
* Description    : ������ȡ״̬�Ĵ���,������״̬�Ĵ�����ֵ
* Input          : None
* Output         : None
* Return         : ExFlashRegStatus
*******************************************************************************/
UINT8 ReadExternalFlashStatusReg_SPI( void )
{
    UINT8X ExFlashRegStatus;
    SCS = 0;
    CH549SPIMasterWrite(CMD_STATUS1);                                           //���Ͷ�״̬�Ĵ���������
    ExFlashRegStatus = CH549SPIMasterRead();                                    //��ȡ״̬�Ĵ���
    SCS = 1 ;
    return ExFlashRegStatus;
}

/*******************************************************************************
* Function Name  : WaitExternalFlashIfBusy
* Description    : �ȴ�оƬ����(��ִ��Byte-Program, Sector-Erase, Block-Erase, Chip-Erase������)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WaitExternalFlashIfBusy( void )
{
    while ((ReadExternalFlashStatusReg_SPI())&0x01 == 0x01 )
    {
        ;    //�ȴ�ֱ��Flash����
    }
}
/*******************************************************************************
* Function Name  : WriteExternalFlashEnable_SPI
* Description    : дʹ��,ͬ����������ʹ��д״̬�Ĵ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WriteExternalFlashEnable_SPI( void )
{
    SCS = 0 ;
    CH549SPIMasterWrite(CMD_WR_ENABLE);                                        //����дʹ������
    SCS = 1 ;
}
/*******************************************************************************
* Function Name  : EraseExternal4KFlash_SPI
* Description    : ����4K Flash  ����һ������
* Input          : Dst_Addr 0-1 ffff ffff ,��������ַ���ڵ�������
* Output         : None
* Return         : None
*******************************************************************************/
void EraseExternal4KFlash_SPI( UINT32 Dst_Addr )
{
    WriteExternalFlashEnable_SPI();
    WaitExternalFlashIfBusy();
    SCS = 0 ;
    CH549SPIMasterWrite(CMD_ERASE_4KBYTE);                                    //������������
    CH549SPIMasterWrite(((Dst_Addr & 0xFFFFFF) >> 16));                       //����3�ֽڵ�ַ
    CH549SPIMasterWrite(((Dst_Addr & 0xFFFF) >> 8));
    CH549SPIMasterWrite(Dst_Addr & 0xFF);
    SCS = 1 ;
    WaitExternalFlashIfBusy();
}
/*******************************************************************************
* Function Name  : EraseExternalFlash_SPI
* Description    : ����32K Flash  ����һ������
* Input          : Dst_Addr 0-1 ffff ffff ,��������ַ���ڵ�������
* Output         : None
* Return         : None
*******************************************************************************/
void EraseExternal32KFlash_SPI( UINT32 Dst_Addr )
{
    WriteExternalFlashEnable_SPI();
    WaitExternalFlashIfBusy();
    SCS = 0 ;
    CH549SPIMasterWrite(CMD_ERASE_32KBYTE);                                    //32K��������
    CH549SPIMasterWrite(((Dst_Addr & 0xFFFFFF) >> 16));                        //����3�ֽڵ�ַ
    CH549SPIMasterWrite(((Dst_Addr & 0xFFFF) >> 8));
    CH549SPIMasterWrite(Dst_Addr & 0xFF);
    SCS = 1 ;
    WaitExternalFlashIfBusy();
}
/*******************************************************************************
* Function Name  : PageWriteExternalFlash_SPI
* Description    : ҳд��SPI��һҳ(0~65535)��д������256���ֽڵ�����
* Input          : RcvBuffer:���ݴ洢��
*                  StarAddr:��ʼд��ĵ�ַ
*                  Len:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
* Output         : None
* Return         : None
*******************************************************************************/
void PageWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer)
{
    UINT16X i;
    WriteExternalFlashEnable_SPI();                                            //SET WEL
    SCS=0;                                                                     //ʹ������
    CH549SPIMasterWrite(CMD_PAGE_PROG);                                        //����дҳ����
    CH549SPIMasterWrite(((StarAddr & 0xFFFFFF) >> 16));                        //����24bit��ַ
    CH549SPIMasterWrite(((StarAddr & 0xFFFF) >> 8));
    CH549SPIMasterWrite(StarAddr & 0xFF);
    for(i=0; i!=Len; i++)
    {
        CH549SPIMasterWrite(RcvBuffer[i]);    //ѭ��д��
    }
    SCS=1;                                                                     //ȡ��Ƭѡ
    WaitExternalFlashIfBusy();                                                 //�ȴ�д�����
}
/*******************************************************************************
* Function Name  : BlukWriteExternalFlash_SPI
* Description    : �޼���дSPI FLASH
*                  ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
*                  �����Զ���ҳ����
*                  ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
* Input          : SendBuffer:���ݴ洢��
*                  StarAddr:��ʼд��ĵ�ַ
*                  Len:Ҫд����ֽ���(���65535)
* Output         : None
* Return         : None
*******************************************************************************/
void BlukWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 SendBuffer)
{
    UINT16X pageremain;
    pageremain=256-StarAddr%256;                                               //��ҳʣ����ֽ���
    if(Len<=pageremain)
    {
        pageremain=Len;    //������256���ֽ�
    }
    while(1)
    {
        PageWriteExternalFlash_SPI(StarAddr,pageremain,SendBuffer);
        if(Len==pageremain)
        {
            break;    //д�������
        }
        else
        {
            SendBuffer+=pageremain;
            StarAddr+=pageremain;
            Len-=pageremain;                                                   //��ȥ�Ѿ�д���˵��ֽ���
            if(Len>256)
            {
                pageremain=256;    //һ�ο���д��256���ֽ�
            }
            else
            {
                pageremain=Len;    //����256���ֽ���
            }
        }
    }
}
/*******************************************************************************
* Function Name  : ReadExternalFlash_SPI
* Description    : ��ȡ��ַ������.
*******************************************************************************/
void ReadExternalFlash_SPI( UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer )
{
    UINT16X i = 0;
    SCS = 0 ;                                                           //Ƭѡ�豸
    CH549SPIMasterWrite(CMD_READ_DATA);                                 //������
    CH549SPIMasterWrite(((StarAddr & 0xFFFFFF) >> 16));                 //����3�ֽڵ�ַ
    CH549SPIMasterWrite(((StarAddr & 0xFFFF) >> 8));
    CH549SPIMasterWrite(StarAddr & 0xFF);
    for(i=0; i!=Len; i++)
    {
        RcvBuffer[i] = CH549SPIMasterRead();                            //ѭ������
    }
    SCS = 1;                                                            //ȡ��Ƭѡ
}
/*******************************************************************************
* Function Name  : BlukReadExternalFlash_SPI
* Description    : ��ȡ��ʼ��ַ(StarAddr)�ڶ���ֽ�(Len)������.���뻺����RcvBuffer��
* Input          : StarAddr -Destination Address 000000H - 1FFFFFH
                   Len ��ȡ���ݳ���
                   RcvBuffer ���ջ�������ʼ��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void BlukReadExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer)
{
    UINT16X i;
    UINT16X l;
    PUINT8 p;
    l = Len/16;
    p = RcvBuffer;
    SCS = 0 ;                                                           //Ƭѡʹ��
    CH549SPIMasterWrite(CMD_FAST_READ);                                 //���ٶ�
    CH549SPIMasterWrite(((StarAddr & 0xFFFFFF) >> 16));                 //����3�ֽڵ�ַ
    CH549SPIMasterWrite(((StarAddr & 0xFFFF) >> 8));
    CH549SPIMasterWrite(StarAddr & 0xFF);
    CH549SPIMasterWrite(0x00);
    SPI0_DATA = 0xFF;
    SPI0_CTRL |=  bS0_DATA_DIR;
    for(i=0; i!=l; i++)                                                 //��������
    {
        //��ȡ��һ��ַ
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
        *p = SPI0_DATA;
        p++;
        while(S0_FREE == 0)
        {
            ;
        }
    }
    SPI0_CTRL &=  ~bS0_DATA_DIR;
    SCS = 1 ;                                                           //ȡ��Ƭѡ
}
/*******************************************************************************
* Function Name  : SPIFlash_ReadID
* Description    : SPI Flash��ȡоƬID
* Input          : None
* Output         : None
* Return         : 0XEF13,��ʾоƬ�ͺ�ΪW25Q80
*                  0XEF14,��ʾоƬ�ͺ�ΪW25Q16
*                  0XEF15,��ʾоƬ�ͺ�ΪW25Q32
*                  0XEF16,��ʾоƬ�ͺ�ΪW25Q64
*                  0XEF17,��ʾоƬ�ͺ�ΪW25Q128
*******************************************************************************/
UINT16 SPIFlash_ReadID(void)
{
    UINT16X temp = 0;
    SCS=0;
    CH549SPIMasterWrite(CMD_DEVICE_ID);                    //��ȡID����
    CH549SPIMasterWrite(0x00);
    CH549SPIMasterWrite(0x00);
    CH549SPIMasterWrite(0x00);
    temp = CH549SPIMasterRead();
    temp = temp<<8;
    temp |= CH549SPIMasterRead();
    SCS=1;
    return temp;
}
/*******************************************************************************
* Function Name  : SPIFlash_Init
* Description    : SPI Flash��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPIFlash_Init(void)
{
    SPIMasterModeSet(3);
    SPI_CK_SET(4);
    printf("id:0x%04x\n",SPIFlash_ReadID());
}