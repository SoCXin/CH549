/********************************** (C) COPYRIGHT *******************************
* File Name          : CompatibilityHID.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/15
* Description        : CH549模拟HID兼容设备，支持中断上下传，支持控制端点上下传，支持全速传输
                       注意包含DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

#define THIS_ENDP0_SIZE         64
#define ENDP2_IN_SIZE           64
#define ENDP2_OUT_SIZE          64

UINT8X  Ep0Buffer[MIN(64,THIS_ENDP0_SIZE+2)] _at_ 0x0000;   //端点0 OUT&IN缓冲区，必须是偶地址
UINT8X  Ep2Buffer[MIN(64,ENDP2_IN_SIZE+2)+MIN(64,ENDP2_OUT_SIZE+2)] _at_ MIN(64,THIS_ENDP0_SIZE+2);                   							   //端点2 IN&OUT缓冲区,必须是偶地址
UINT8   SetupReq,Ready,UsbConfig;
UINT16  SetupLen;
PUINT8  pDescr;                                                                    //USB配置标志
USB_SETUP_REQ   SetupReqBuf;                                                       //暂存Setup包
#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)
sbit Ep2InKey = P1^5;                                                              //K1按键
#pragma  NOAREGS
/*设备描述符*/
UINT8C DevDesc[] = {0x12,0x01,0x10,0x01,0x00,0x00,0x00,THIS_ENDP0_SIZE,
                    0x86,0x1A,0xE0,0xE6,0x00,0x00,0x01,0x02,
                    0x00,0x01
                   };
UINT8C CfgDesc[] =
{
    0x09,0x02,0x29,0x00,0x01,0x01,0x04,0xA0,0x23,               //配置描述符
    0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x05,               //接口描述符
    0x09,0x21,0x00,0x01,0x00,0x01,0x22,0x22,0x00,               //HID类描述符
    0x07,0x05,0x82,0x03,ENDP2_IN_SIZE,0x00,0x01,              //端点描述符
    0x07,0x05,0x02,0x03,ENDP2_OUT_SIZE,0x00,0x01,              //端点描述符
};
/*字符串描述符 略*/
/*HID类报表描述符*/
UINT8C HIDRepDesc[ ] =
{
    0x06, 0x00,0xff,
    0x09, 0x01,
    0xa1, 0x01,                                                   //集合开始
    0x09, 0x02,                                                   //Usage Page  用法
    0x15, 0x00,                                                   //Logical  Minimun
    0x26, 0x00,0xff,                                              //Logical  Maximun
    0x75, 0x08,                                                   //Report Size
    0x95, THIS_ENDP0_SIZE,                                        //Report Counet
    0x81, 0x06,                                                   //Input
    0x09, 0x02,                                                   //Usage Page  用法
    0x15, 0x00,                                                   //Logical  Minimun
    0x26, 0x00,0xff,                                              //Logical  Maximun
    0x75, 0x08,                                                   //Report Size
    0x95, THIS_ENDP0_SIZE,                                        //Report Counet
    0x91, 0x06,                                                   //Output
    0xC0
};
// 语言描述符
UINT8C  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// 厂家信息
UINT8C  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };
// 产品信息
UINT8C  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '4', 0, '9', 0 };                                //字符串描述符
UINT8X UserEp2Buf[64];                                            //用户数据定义
UINT8 Endp2Busy = 0;
/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceInit()
{
    IE_USB = 0;
    USB_CTRL = 0x00;                                                           // 先设定USB设备模式
    UDEV_CTRL = bUD_PD_DIS;                                                    // 禁止DP/DM下拉电阻
    UDEV_CTRL &= ~bUD_LOW_SPEED;                                               //选择全速12M模式，默认方式
    USB_CTRL &= ~bUC_LOW_SPEED;
    UEP2_T_LEN = 0;                                                            //预使用发送长度一定要清空
    UEP2_DMA = Ep2Buffer;                                                      //端点2数据传输地址
    UEP2_3_MOD |= bUEP2_TX_EN | bUEP2_RX_EN;                                   //端点2发送接收使能
    UEP2_3_MOD &= ~bUEP2_BUF_MOD;                                              //端点2收发各64字节缓冲区
    UEP0_DMA = Ep0Buffer;                                                      //端点0数据传输地址
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //端点0单64字节收发缓冲区
    USB_DEV_AD = 0x00;
    USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                     // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN;                                                  // 允许USB端口
    USB_INT_FG = 0xFF;                                                         // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}
/*******************************************************************************
* Function Name  : Enp2BlukIn()
* Description    : USB设备模式端点2的批量上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp2BlukIn( UINT8 *buf,UINT8 len)
{
    memcpy( Ep2Buffer+MAX_PACKET_SIZE, buf, len);                              //加载上传数据
    UEP2_T_LEN = len;                                                          //上传最大包长度
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                  //有数据时上传数据并应答ACK
}
/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH559USB中断处理函数
*******************************************************************************/
void  DeviceInterrupt( void ) interrupt INT_NO_USB using 1                     //USB中断服务程序,使用寄存器组1
{
    UINT8 i;
    UINT16 len;
    if(UIF_TRANSFER)                                                            //USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:                                                  //endpoint 2# 端点批量上传
            UEP2_T_LEN = 0;                                                     //预使用发送长度一定要清空
            UEP2_CTRL ^= bUEP_T_TOG;                                            //手动翻转同步标志位
            Endp2Busy = 0 ;
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            break;
        case UIS_TOKEN_OUT | 2:                                                 //endpoint 2# 端点批量下传
            if ( U_TOG_OK )                                                     // 不同步的数据包将丢弃
            {
                UEP2_CTRL ^= bUEP_R_TOG;									    //手动翻转同步标志位
				len = USB_RX_LEN;                                               //接收数据长度，数据从Ep2Buffer首地址开始存放
                for ( i = 0; i < len; i ++ )
                {
                    Ep2Buffer[MAX_PACKET_SIZE+i] = Ep2Buffer[i] ^ 0xFF;         // OUT数据取反到IN由计算机验证
                }
                UEP2_T_LEN = len;
                UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;       // 允许上传
            }
            break;
        case UIS_TOKEN_SETUP | 0:                                               //SETUP事务
            UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;          //预置NAK,防止stall之后不及时清除响应方式
            len = USB_RX_LEN;
            if(len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = ((UINT16)UsbSetupBuf->wLengthH<<8) + UsbSetupBuf->wLengthL;
                len = 0;                                                         // 默认为成功并且上传0长度
                SetupReq = UsbSetupBuf->bRequest;
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/*HID类命令*/
                {
                    switch( SetupReq )
                    {
                        Ready = 1;                                              //HID类命令一般代表usb枚举完成的标志
                    case 0x01:                                                //GetReport
                        pDescr = UserEp2Buf;                                    //控制端点上传输据
                        if(SetupLen >= THIS_ENDP0_SIZE)                         //大于端点0大小，需要特殊处理
                        {
                            len = THIS_ENDP0_SIZE;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    case 0x02:                                                   //GetIdle
                        break;
                    case 0x03:                                                   //GetProtocol
                        break;
                    case 0x09:                                                   //SetReport
                        break;
                    case 0x0A:                                                   //SetIdle
                        break;
                    case 0x0B:                                                   //SetProtocol
                        break;
                    default:
                        len = 0xFFFF;                                                    /*命令不支持*/
                        break;
                    }
                    if( SetupLen > len )
                    {
                        SetupLen = len;    //限制总长度
                    }
                    len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;   //本次传输长度
                    memcpy(Ep0Buffer,pDescr,len);                                     //加载上传数据
                    SetupLen -= len;
                    pDescr += len;
                }
                else                                                             //标准请求
                {
                    switch(SetupReq)                                             //请求码
                    {
                    case USB_GET_DESCRIPTOR:
                        switch(UsbSetupBuf->wValueH)
                        {
                        case 1:                                                  //设备描述符
                            pDescr = DevDesc;                                    //把设备描述符送到要发送的缓冲区
                            len = sizeof(DevDesc);
                            break;
                        case 2:                                                  //配置描述符
                            pDescr = CfgDesc;                                    //把设备描述符送到要发送的缓冲区
                            len = sizeof(CfgDesc);
                            break;
                        case 3:
                            switch( UsbSetupBuf->wValueL )
                            {
                            case 1:
                                pDescr = (PUINT8)( &MyManuInfo[0] );
                                len = sizeof( MyManuInfo );
                                break;
                            case 2:
                                pDescr = (PUINT8)( &MyProdInfo[0] );
                                len = sizeof( MyProdInfo );
                                break;
                            case 0:
                                pDescr = (PUINT8)( &MyLangDescr[0] );
                                len = sizeof( MyLangDescr );
                                break;
                            default:
                                len = 0xFFFF;                                // 不支持的字符串描述符
                                break;
                            }
                            break;
                        case 0x22:                                               //报表描述符
                            pDescr = HIDRepDesc;                                 //数据准备上传
                            len = sizeof(HIDRepDesc);
                            break;
                        default:
                            len = 0xFFFF;                                        //不支持的命令或者出错
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;    //限制总长度
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;//本次传输长度
                        memcpy(Ep0Buffer,pDescr,len);                            //加载上传数据
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL;                         //暂存USB设备地址
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
                        if(UsbConfig)
                        {
                            Ready = 1;                                            //set config命令一般代表usb枚举完成的标志
                        }
                        break;
                    case 0x0A:
                        break;
                    case USB_CLEAR_FEATURE:                                      //Clear Feature
                        if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// 端点
                        {
                            switch( UsbSetupBuf->wIndexL )
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x81:
                                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFFFF;                                     // 不支持的端点
                                break;
                            }
                        }
                        else
                        {
                            len = 0xFFFF;                                         // 不是端点不支持
                        }
                        break;
                    case USB_SET_FEATURE:                                         /* Set Feature */
                        if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )        /* 设置设备 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                            {
                                if( CfgDesc[ 7 ] & 0x20 )
                                {
                                    /* 设置唤醒使能标志 */
                                }
                                else
                                {
                                    len = 0xFFFF;                                  /* 操作失败 */
                                }
                            }
                            else
                            {
                                len = 0xFFFF;                                      /* 操作失败 */
                            }
                        }
                        else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )    /* 设置端点 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
                            {
                                switch( ( ( UINT16 )UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT Stall */
                                    break;
                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点1 IN STALL */
                                    break;
                                default:
                                    len = 0xFFFF;                                   /* 操作失败 */
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFFFF;                                       /* 操作失败 */
                            }
                        }
                        else
                        {
                            len = 0xFFFF;                                           /* 操作失败 */
                        }
                        break;
                    case USB_GET_STATUS:
                        Ep0Buffer[0] = 0x00;
                        Ep0Buffer[1] = 0x00;
                        if ( SetupLen >= 2 )
                        {
                            len = 2;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    default:
                        len = 0xFFFF;                                                //操作失败
                        break;
                    }
                }
            }
            else
            {
                len = 0xFFFF;                                                        //包长度错误
            }
            if(len == 0xFFFF)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
            }
            else if(len <= THIS_ENDP0_SIZE)                                         //上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
            }
            else
            {
                UEP0_T_LEN = 0;  //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1,返回应答ACK
            }
            break;
        case UIS_TOKEN_IN | 0:                                                      //endpoint0 IN
            switch(SetupReq)
            {
            case USB_GET_DESCRIPTOR:
            case HID_GET_REPORT:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;     //本次传输长度
                memcpy( Ep0Buffer, pDescr, len );                                   //加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                                            //同步标志位翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                                      //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:  // endpoint0 OUT
            len = USB_RX_LEN;
            UEP0_CTRL ^= bUEP_R_TOG;                                                //同步标志位翻转
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                                           //写0清空中断
    }
    else if(UIF_BUS_RST)                                                            //设备模式USB总线复位中断
    {
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 
        UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                                				   
		USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        Endp2Busy = 0;
        Ready = 0;
        UIF_BUS_RST = 0;                                                            //清中断标志
    }
    else if (UIF_SUSPEND)                                                           //USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if ( USB_MIS_ST & bUMS_SUSPEND )                                            //挂起
        {
#ifdef DE_PRINTF
            printf( "z" );                                                          //睡眠状态
#endif
//             while ( XBUS_AUX & bUART0_TX )
//             {
//                 ;    //等待发送完成
//             }
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = bWA  K_BY_USB | bWAK_RXD0_LO;                              //USB或者RXD0有信号时可被唤醒
//             PCON |= PD;                                                          //睡眠
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = 0x00;
        }
        else                                                                        // 唤醒
        {
#ifdef DE_PRINTF
            printf( "w" );
#endif
        }
    }
    else {                                                                          //意外的中断,不可能发生的情况
        USB_INT_FG = 0xFF;                                                          //清中断标志
#ifdef DE_PRINTF
        printf("UnknownInt  \n");
#endif
    }
}
void main()
{
    UINT8 i;
    CfgFsys( );                                                            //CH549时钟选择配置
    mDelaymS(20);                                                          //修改主频等待内部晶振稳定,必加
    mInitSTDIO( );                                                         //串口0初始化
#ifdef DE_PRINTF
    printf("start ...\n");
#endif
    for(i=0; i<64; i++)                                                    //准备演示数据
    {
        UserEp2Buf[i] = i;
    }
    USBDeviceInit();                                                       //USB设备模式初始化
    EA = 1;                                                                //允许单片机中断
    while(1)
    {
        if(Ready && (Ep2InKey==0))
        {
            while( Endp2Busy )
            {
                ;    //如果忙（上一包数据没有传上去），则等待。
            }
            Endp2Busy = 1;                                                 //设置为忙状态
            Enp2BlukIn( UserEp2Buf,THIS_ENDP0_SIZE );
        }
        mDelaymS( 100 );                                                   //模拟单片机做其它事
    }
}
