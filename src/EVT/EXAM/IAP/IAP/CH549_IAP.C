/********************************** (C) COPYRIGHT ******************************
* File Name          : CH549_IAP.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/24
* Description        : IAP ������ʾ���ӳ���
*                      1��֧��USB���أ�USBΪȫ���Զ����豸
*                      2��֧��CodeFlash���
                       ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\IAP\IAP\CH549_IAP.H"
/******************************************************************************
*  ע��CH549_IAP.C��Ҫ���CH549_USER.Cʹ��
*  ע�⣺�����Ӵ�0xE000��ʼ���IAP����,Ҳ���Ǵ�56K����ʼ���IAP,56K-60Kλ�ÿ������ڴ��IAP����
*  ����ǰ��ָ�����ӵ�ַ�� Option->Target Code Memeryѡ��� Eprom��ʼ��ַ����:0xE004
*
*  ���ӵ�ַƫ�����ֽڵ�˵����IAP����CodeFlash���棬���ɵ�BIN�ļ���ͷ���ĸ��ֽ�����ת��
*  IAPmain��ָ�������USER��IAP֮��ǰ�������Ƿ��û����ļ��ģ��������ĸ��ֽڵ���ת
*  ��IAPmain��ָ�����ᵽ����ĳ�̶���ַ��������IAP�����ļ�ʱ���ƫ��4�ֽھ���Ϊ����ת
*  ָ���ڵط���
*******************************************************************************/
#define THIS_ENDP0_SIZE         DEFAULT_ENDP0_SIZE               //Ĭ�϶˵�0�Ĵ�С
// �豸������
UINT8C MyDevDescr[] = { 0x12, 0x01, 0x10, 0x01,
                        0xFF, 0x80, 0x55, THIS_ENDP0_SIZE,
                        0x48, 0x43, 0xe0, 0x55,
                        0x00, 0x01, 0x00, 0x00,0x00, 0x01
                      };
// ����������
UINT8C MyCfgDescr[] = { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                        0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x80, 0x55, 0x00,
                        0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,
                        0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00
                      };
UINT8X UsbConfig = 0;                                            // USB���ñ�־
UINT8X Ep0Buffer[ THIS_ENDP0_SIZE+2 ] _at_ 0x0000 ;              // OUT&IN, must even address
UINT8X Ep2Buffer[ 2*MAX_PACKET_SIZE+4 ] _at_ THIS_ENDP0_SIZE+2 ; // OUT+IN, must even address
#define   UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)
UINT8X SetupReqCode,SetupLen;
PUINT8 pDescr;

iap_cmd1 xdata iap_cmd _at_ 0x00C6;                              //IAP����
UINT8X uart_bit;                                                 //���ط�ʽȫ�ֱ�־λ��1��ʾΪ���ڣ�2��ʾΪUSB��
UINT16X chip_id,eeprom_len;
PUINT8C pCode;
#pragma NOAREGS
sbit DisableIAP = P1^0;                                         //����λ,�����û����򴥷�����
sbit D2 = P2^2;
UINT16X cur_block,last_block;                                   //64�ֽ�һ�飬��ʾ��ǰ��ǰһ�εĿ�ţ����ƿ�Ĳ�������

/*******************************************************************************
* Function Name  : ErasePage( UINT16 Addr )
* Description    : ����ҳ������ÿ64�ֽ�Ϊһҳ����ҳ���������ݱ�Ϊ0x00
* Input          : Addr:������ַ����ҳ
* Output         : None
* Return         : ���ز���״̬��0x00:�ɹ�  0x01:��ַ��Ч  0x02:δ֪�����ʱ
*******************************************************************************/
UINT8 FlashErasePage( UINT16 Addr )
{
    bit e_all;
    UINT8 status;                                    /* ���ز���״̬ */
    UINT8 FlashType;                                 /* Flash ���ͱ�־ */
    e_all = EA;
    EA = 0;                                          /* �ر�ȫ���ж�,��ֹFlash��������� */
    Addr &= 0xFFC0;                                  /* 64�ֽڶ��� */
    if((Addr>0xEFFF) && (Addr<0xF400))               /* DataFlash���� */
    {
        FlashType = bDATA_WE;
    }
    else                                             /* CodeFlash���� */
    {
        FlashType = bCODE_WE;
    }
    SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= FlashType;
    ROM_ADDR = Addr;                                 /* д��Ŀ���ַ */
    ROM_BUF_MOD = bROM_BUF_BYTE;                     /* ѡ������ģʽ���ֽڱ��ģʽ */
    ROM_DAT_BUF = 0;                                 /* ��д���ݻ������Ĵ���Ϊ0 */
    if ( ROM_STATUS & bROM_ADDR_OK )                 /* ������ַ��Ч */
    {
        ROM_CTRL = ROM_CMD_ERASE;                    /* �������� */
        if(ROM_STATUS & bROM_CMD_ERR)
        {
            status = 0x02;    /* δ֪�����ʱ */
        }
        else
        {
            status = 0x00;    /* �����ɹ� */
        }
    }
    else
    {
        status = 0x01;    /* ��ַ��Ч */
    }
    SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~FlashType;                        /* ����д���� */
    EA = e_all;                                      /* �ָ�ȫ���ж�״̬ */
    return status;
}
/*******************************************************************************
* Function Name  : FlashProgByte(UINT16 Addr,UINT8 Data)
* Description    : Flash �ֽڱ��
* Input          : Addr��д���ַ
*                  Data���ֽڱ�̵�����
* Output         : None
* Return         : ���״̬���� 0x00:�ɹ�  0x01:��ַ��Ч  0x02:δ֪�����ʱ
*******************************************************************************/
UINT8 FlashProgByte( UINT16 Addr,UINT8 Data )
{
    bit e_all;
    UINT8 status;                                    /* ���ز���״̬ */
    UINT8 FlashType;                                 /* Flash ���ͱ�־ */
    e_all = EA;
    EA = 0;                                          /* �ر�ȫ���ж�,��ֹFlash��������� */
    if((Addr>0xEFFF) && (Addr<0xF400))               /* DataFlash���� */
    {
        FlashType = bDATA_WE;
    }
    else                                             /* CodeFlash���� */
    {
        FlashType = bCODE_WE;
    }
    SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= FlashType;
    ROM_ADDR = Addr;                                 /* д��Ŀ���ַ */
    ROM_BUF_MOD = bROM_BUF_BYTE;                     /* ѡ������ģʽ���ֽڱ��ģʽ */
    ROM_DAT_BUF = Data;                              /* ���ݻ������Ĵ��� */
    if ( ROM_STATUS & bROM_ADDR_OK )                 /* ������ַ��Ч */
    {
        ROM_CTRL = ROM_CMD_PROG ;                    /* ������� */
        if(ROM_STATUS & bROM_CMD_ERR)
        {
            status = 0x02;    /* δ֪�����ʱ */
        }
        else
        {
            status = 0x00;    /* �����ɹ� */
        }
    }
    else
    {
        status = 0x01;    /* ��ַ��Ч */
    }
    SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~FlashType;                        /* ����д���� */
    EA = e_all;                                      /* �ָ�ȫ���ж�״̬ */
    return status;
}
/*******************************************************************************
* Function Name  : FlashVerify
* Description    : FlashУ��
* Input          : Addr    оƬ��̵�ַ��ַ����ַΪż����ַ
                   pData   ������ݣ���WORDΪ��׼
                   len     У�鳤��
* Output         : None
* Return         :         ����У��״̬
                    0x00   У��ɹ�
                    0xff   У��ʧ��
*******************************************************************************/
UINT8 FlashVerify( UINT16 Addr, UINT8 *pData, UINT16 len )
{
    UINT16 i;
    pCode = (PUINT8C)( Addr );
    for( i=0; i!=len; i++ )
    {
        if( *pData != *pCode )
        {
            return 0xff;
        }
        pCode++;
        pData++;
    }
    return 0;
}
/*******************************************************************************
* Function Name  : CH549_Respond
* Description    : IAP����ʱ��оƬӦ����
* Input          : s ��ЧӦ���ֽ�
* Output         : None
* Return         : SBUF ���ڽ����ֽ�
*******************************************************************************/
void CH549_Respond( UINT8 s )
{
    Ep2Buffer[ MAX_PACKET_SIZE ] = s;
    Ep2Buffer[ MAX_PACKET_SIZE+1 ] = 0x00;
    UEP2_T_LEN = 2;
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;  // �����ϴ�
}
/*******************************************************************************
* Function Name  : CH549_USB_ISPDownload
* Description    : CH549���غ���
*                ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549_IAPDownload( void )
{
    UINT8  s;
    UINT16 i;
    UINT16 len;
    UINT32 addr;
    switch( iap_cmd.other.buf[0] )                                  //����������
    {
    case CMD_IAP_PROM:                                              //ISP �������
        len = iap_cmd.program.len;                                  //��д�ֽ���
        addr = (iap_cmd.program.addr[0] | (UINT16)iap_cmd.program.addr[1]<<8);
        cur_block = (addr+len-1)/64;                                //�´����ݵ����һ���ֽڵ�ַ���ڿ�
        if(cur_block!=last_block)
        {
            last_block = cur_block;
            s = FlashErasePage(cur_block<<6);                       //����
        }
        if(s == 0)
        {
            for(i=0; i!=len; i++)
            {
                s = FlashProgByte( addr+i,iap_cmd.program.buf[i] ); //�ֽڱ��,��������٣����Ըĳɿ���
                if(s)
                {
                    break;
                }
            }
        }
        CH549_Respond( s );                                         //����У��
        break;
    case CMD_IAP_ERASE:                                             //ISP �������ʵ�ʲ������ڱ��ʱ������
        cur_block = 0;                                              //CMD_IAP_ERASE�ǵ�һ���������Ҳ�������,�ʴ˴���ʼ������
        last_block = 0xFFFF;
        CH549_Respond( 0 );
        break;
    case CMD_IAP_VERIFY:                                             // ISP У������
        addr = (iap_cmd.verify.addr[0] | (UINT16)iap_cmd.verify.addr[1]<<8);
        len = iap_cmd.verify.len;
        s = FlashVerify( addr,&(iap_cmd.verify.buf[0]),iap_cmd.verify.len );
        CH549_Respond( s );
        break;
    case CMD_IAP_END:                                                // ISP ��������
        CH549SoftReset();                                            // ִ�и�λ
        break;
    default:
        CH549_Respond( 0xfe );                                       // δ֪������
        break;
    }
}
/*******************************************************************************
* Function Name  : USB_DeviceInterrupt
* Description    : USB�жϲ�ѯ������IAP�����޷�ʹ���ж�
*                ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_DeviceInterrupt( void )
{
    UINT8 len;
    if( UIF_TRANSFER )                                                // USB�������
    {
        switch ( USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) )// �����������ƺͶ˵��
        {
        case UIS_TOKEN_OUT | 2:                                   // endpoint 2# �����˵��´�
            if ( U_TOG_OK )                                       // ��ͬ�������ݰ�������
            {
                len = USB_RX_LEN;
                memcpy( iap_cmd.other.buf,Ep2Buffer,len );
                uart_bit = 2;
                CH549_IAPDownload( );
            }
            break;
        case UIS_TOKEN_IN | 2:                                    // endpoint 2# �����˵��ϴ�
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;// ��ͣ�ϴ�
            break;
        case UIS_TOKEN_SETUP | 0:                                 // endpoint 0# SETUP
            len = USB_RX_LEN;
            if ( len == sizeof( USB_SETUP_REQ ) )                 // SETUP������
            {
                SetupLen = UsbSetupBuf->wLengthL;
                if ( UsbSetupBuf->wLengthH || SetupLen > 0x7F )
                {
                    SetupLen = 0x7F;                              // �����ܳ���
                }
                len = 0;                                          // Ĭ��Ϊ�ɹ������ϴ�0����
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )//ֻ֧�ֱ�׼����
                {
                    len = 0xFF;                                   // ����ʧ��
                }
                else                                              // ��׼����
                {
                    SetupReqCode = UsbSetupBuf->bRequest;
                    switch( SetupReqCode )                        // ������
                    {
                    case USB_GET_DESCRIPTOR:
                        switch( UsbSetupBuf->wValueH )
                        {
                        case 1:                                   // �豸������
                            pDescr = (PUINT8)( &MyDevDescr[0] );
                            len = sizeof( MyDevDescr );
                            break;
                        case 2:                                   // ����������
                            pDescr = (PUINT8)( &MyCfgDescr[0] );
                            len = sizeof( MyCfgDescr );
                            break;
                        default:
                            len = 0xFF;                           // ��֧�ֵ�����������
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;                       // �����ܳ���
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;// ���δ��䳤��
                        memcpy( Ep0Buffer, pDescr, len );         // �����ϴ�����
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL;          // �ݴ�USB�豸��ַ
                        break;
                    case USB_GET_CONFIGURATION:
                        Ep0Buffer[0] = UsbConfig;
                        if ( SetupLen >= 1 )
                        {
                            len = 1;
                        }
                        break;
                    case USB_SET_CONFIGURATION:
                        UsbConfig = UsbSetupBuf->wValueL;
                        break;
                    default:
                        len = 0xFF;                               // ����ʧ��
                        break;
                    }
                }
            }
            else
            {
                len = 0xFF;                                       // SETUP�����ȴ���
            }
            if ( len == 0xFF )                                    // ����ʧ��
            {
                SetupReqCode = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;// STALL
            }
            else if ( len <= THIS_ENDP0_SIZE )                    // �ϴ����ݻ���״̬�׶η���0���Ȱ�
            {
                UEP0_T_LEN = len;
                UEP0_CTRL  = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// Ĭ�����ݰ���DATA1
            }
            else                                                  // �´����ݻ�����
            {
                UEP0_T_LEN = 0;                                   // ��Ȼ��δ��״̬�׶Σ�������ǰԤ���ϴ�0�������ݰ��Է�������ǰ����״̬�׶�
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// Ĭ�����ݰ���DATA1
            }
            break;
        case UIS_TOKEN_IN | 0:                                    // endpoint 0# IN
            switch( SetupReqCode )
            {
            case USB_GET_DESCRIPTOR:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;// ���δ��䳤��
                memcpy( Ep0Buffer, pDescr, len );                 // �����ϴ�����
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                          // ��ת
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                   // ״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ���
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:                                   // endpoint 0# OUT
            UEP0_CTRL ^= bUEP_R_TOG;                              //ͬ����־λ��ת
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                             // ���жϱ�־
    }
    else if ( UIF_BUS_RST )                                           // USB���߸�λ
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;                                              // ���жϱ�־
    }
    else if ( UIF_SUSPEND )                                           // USB���߹���/�������
    {
        UIF_SUSPEND = 0;
    }
}
/*******************************************************************************
* Function Name  : InitUSB_Device()
* Description    : USB�豸ģʽ����,�豸ģʽ�������շ��˵����ã��жϿ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitUSB_Device( void )                                         // ��ʼ��USB�豸
{
    IE_USB = 0;
    USB_CTRL = 0x00;                                                // ���趨ģʽ
    UEP2_3_MOD = bUEP2_RX_EN | bUEP2_TX_EN;                         // �˵�2�´�OUT���ϴ�IN
    UEP0_DMA = Ep0Buffer;
    UEP2_DMA = Ep2Buffer;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
    USB_DEV_AD = 0x00;
    UDEV_CTRL = bUD_PD_DIS;                                         // ��ֹDP/DM��������
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;           // ����USB�豸��DMA�����ж��ڼ��жϱ�־δ���ǰ�Զ�����NAK
    UDEV_CTRL |= bUD_PORT_EN;                                       // ����USB�˿�
    USB_INT_FG = 0xFF;                                              // ���жϱ�־
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
}
/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main( void )
{
    UINT32 j=0;
    CfgFsys();
    mDelaymS(5);
    mInitSTDIO( );
    printf("IAP Demo start...\n");
    EA = 0;                                                              //�ر��жϣ�IAP��ʽ�޷�ʹ���ж�
    P1_MOD_OC |= (1<<0);
    P1_DIR_PU |= (1<<0);
    uart_bit = 0;
    InitUSB_Device( );                                                   //USB�豸ģʽ��ʼ����������ѯ��ʽ
    while(1)
    {
        USB_DeviceInterrupt( );                                          // ��ѯusb�ж�,���鲻ҪƵ����ѯ
        /* �˳�iap���� */
        if( DisableIAP == 0 )                                            // ��ѯP10�͵�ƽʱִ������λ������ִ���û�����
        {
            printf("soft reset\n");
            CH549SoftReset();                                            //����λ
        }
        j++;
        if(j == 50000)
        {
            D2 = ~D2;                                                      //D2����
            j = 0;
        }
    }
}