/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/28
* Description        : CH549 SD���ļ�ϵͳ������ʾ
�����������ļ���DEBUG.C\SD.C\Main.C\SPI.C\CH549UFI.LIB
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI_SDCARD\SD.H"
#pragma  NOAREGS
#ifdef MAX_PACKET_SIZE
#undef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE                 1       /* ����RxBuffer,TxBuffer,ֻ�趨��Сһ�㼴�ɣ����Ǳ��붨��,��ΪU�̲���ʱ�ļ�ϵͳ�������� */
#define DISK_BASE_BUF_LEN               512     /* Ĭ�ϵĴ������ݻ�������СΪ512�ֽ�(����ѡ��Ϊ2048����4096��֧��ĳЩ��������U��),Ϊ0���ֹ�ڱ��ļ��ж��建��������Ӧ�ó�����pDISK_BASE_BUF��ָ�� */
#define NO_DEFAULT_ACCESS_SECTOR        1       /* ��ֹĬ�ϵĴ���������д�ӳ���,���������б�д�ĳ�������� */
#define NO_DEFAULT_DISK_CONNECT         1       /* ��ֹĬ�ϵļ����������ӳ���,���������б�д�ĳ�������� */
//#define NO_DEFAULT_FILE_ENUMER        1       /* ��ֹĬ�ϵ��ļ���ö�ٻص�����,���������б�д�ĳ�������� */
#include ".\USB\USB_LIB\CH549UFI.H"
#include ".\USB\USB_LIB\CH549UFI.C"
/********************** ���¶���Ϊ�������뾯������ **************************/
UINT8X RxBuffer[ MAX_PACKET_SIZE ];
UINT8X TxBuffer[ MAX_PACKET_SIZE ];
UINT8 USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout )
{
    endp_pid = endp_pid;
    tog = tog;
    timeout = timeout;
    return 0;
}
UINT8 HostCtrlTransfer( PUINT8X DataBuf, PUINT8I RetLen )
{
    DataBuf = DataBuf;
    RetLen = RetLen;
    return 0;
}
void CopySetupReqPkg( PUINT8C pReqPkt )
{
    pReqPkt = pReqPkt;
}
UINT8 CtrlGetConfigDescr( void )
{
    return 0;
}
UINT8 CtrlSetUsbConfig( UINT8 cfg )
{
    cfg = cfg;
    return 0;
}
UINT8 CtrlClearEndpStall( UINT8 endp )
{
    endp = endp;
    return 0;
}
/******************���ϱ���,������U�̿�,SD��ʱ������*****************************/
/******************����ΪSD���ײ���⺯���ӿڴ������� ***************************/
#ifdef NO_DEFAULT_DISK_CONNECT
/*******************************************************************************
* Function Name  : CH549DiskConnect
* Description    : �������Ӽ�⣨�ض��壩
* Input          : None
* Output         : None
* Return         : ERR_SUCCESS / ERR_USB_DISCON
*******************************************************************************/
UINT8 CH549DiskConnect( void )
{
    if(SD_Check_Insert() == TRUE)                                              /* ���Ѳ��� */
    {
        if(CH549DiskStatus < DISK_CONNECT)
        {
            CH549DiskStatus = DISK_CONNECT;
        }
        return( ERR_SUCCESS );
    }
    else                                                                       /* ��δ���� */
    {
        CH549DiskStatus = DISK_DISCONNECT;
        return( ERR_USB_DISCON );
    }
}
#endif
#ifdef NO_DEFAULT_ACCESS_SECTOR
/*******************************************************************************
* Function Name  : CH549ReadSector
* Description    : ������ȡ���ض��壩,���ļ�ϵͳ�����
* Input          : SectCount ��ȡ������
*                  DataBuf ָ���ȡ������
*                  CH549vLbaCurrent ������ʼ����
* Output         : None
* Return         : ����״̬
*******************************************************************************/
UINT8 CH549ReadSector( UINT8 SectCount, PUINT8X DataBuf )
{
#if 0
    UINT16X i;
    printf("read sector 0x%08lx\n",CH549vLbaCurrent);
#endif
    CH549IntStatus = SD_READ_MULT_SECTOR(DataBuf,CH549vLbaCurrent,SectCount );
#if 0
    for( i=0; i!=DISK_BASE_BUF_LEN; i++)
    {
        printf("%02x ",(UINT16)DataBuf[i]);
    }
    printf("\n");
#endif
    if ( CH549IntStatus == SD_ERR_CMD_OK )
    {
        CH549IntStatus = ERR_SUCCESS;
    }
    else
    {
        CH549IntStatus = ERR_USB_DISK_ERR;
    }
    return( CH549IntStatus );
}
/*******************************************************************************
* Function Name  : CH549WriteSector
* Description    : ����д���ض��壩,���ļ�ϵͳ�����
* Input          : SectCount д������
*                  DataBuf ָ��д������
*                  CH549vLbaCurrent ������ʼ����
* Output         : None
* Return         : ����״̬
*******************************************************************************/
#ifdef  EN_DISK_WRITE
UINT8 CH549WriteSector( UINT8 SectCount, PUINT8X DataBuf )  /* ���������еĶ�����������ݿ�д����� */
{
#if 0
    UINT16X i;
    printf("write sector 0x%08lx\n",CH549vLbaCurrent);
#endif
    CH549IntStatus = SD_WRITE_MULT_SECTOR( DataBuf,CH549vLbaCurrent,SectCount );
#if 0
    for( i=0; i!=DISK_BASE_BUF_LEN; i++)
    {
        printf("%02x ",(UINT16)DataBuf[i]);
    }
    printf("\n");
#endif
    if ( CH549IntStatus == SD_ERR_CMD_OK )
    {
        CH549IntStatus = ERR_SUCCESS;
    }
    else
    {
        CH549IntStatus = ERR_USB_DISK_ERR;
    }
    return( CH549IntStatus );
}
#endif
#endif
/*******************************************************************************
* Function Name  : CH549DiskInit
* Description    : SD����ʼ�����������ļ�ϵͳ,������U�̲���ʱ����CH549DiskReady
*                  ����ʼ������һ��,�ҵ�DBR��ַ
* Input          : None
* Output         : ���� CH549DiskStatus CH549vLbaCurrent
* Return         : ����״̬
*******************************************************************************/
UINT8 CH549DiskInit( void )
{
    UINT8X s;
    s = SD_Init();                                                             //SD����ʼ��
    if(s!=SD_ERR_CMD_OK)
    {
        return s;
    }
    CH549vLbaCurrent = 0;                                                      //����DBR
    s = CH549ReadSector(1,pDISK_BASE_BUF);                                     //��ȡ0������������Ϣ
    if((pDISK_BASE_BUF[0] == 0xeb)&&(pDISK_BASE_BUF[2] == 0x90))               //DBR��������������0
    {
        CH549vLbaCurrent = 0;
    }
    else
    {
        CH549vLbaCurrent = pDISK_BASE_BUF[454] + (((UINT32)pDISK_BASE_BUF[455])<<8) + (((UINT32)pDISK_BASE_BUF[456])<<16) + (((UINT32)pDISK_BASE_BUF[457])<<24);
        if(CH549vLbaCurrent)                                                  //����ƫ�Ʒ�0���ҵ�DBR
        {
#if DE_PRINTF
            printf("The active part is PART1!\r\n");
#endif
        }
        else
        {
            CH549vLbaCurrent = pDISK_BASE_BUF[470] + (((UINT32)pDISK_BASE_BUF[471])<<8) + (((UINT32)pDISK_BASE_BUF[472])<<16) + (((UINT32)pDISK_BASE_BUF[473])<<24);
            if(CH549vLbaCurrent)                                              //����ƫ�Ʒ�0���ҵ�DBR
            {
#if DE_PRINTF
                printf("The active part is PART2!\r\n");
#endif
            }
            else
            {
                CH549vLbaCurrent = pDISK_BASE_BUF[486] + (((UINT32)pDISK_BASE_BUF[487])<<8) + (((UINT32)pDISK_BASE_BUF[488])<<16) + (((UINT32)pDISK_BASE_BUF[489]<<24));
                if(CH549vLbaCurrent)                                          //����ƫ�Ʒ�0���ҵ�DBR
                {
#if DE_PRINTF
                    printf("The active part is PART3!\r\n");
#endif
                }
                else
                {
                    CH549vLbaCurrent = pDISK_BASE_BUF[502] + (((UINT32)pDISK_BASE_BUF[503])<<8) + (((UINT32)pDISK_BASE_BUF[504])<<16) + (((UINT32)pDISK_BASE_BUF[505])<<24);
                    if(CH549vLbaCurrent)                                      //����ƫ�Ʒ�0���ҵ�DBR
                    {
#if DE_PRINTF
                        printf("The active part is PART4!\r\n");
#endif
                    }
                    else
                    {
                        return ERR_MBR_ERROR;
                    }
                }
            }
        }
    }
    CH549DiskStatus = DISK_MOUNTED;
    return ERR_SUCCESS;
}
/*******************************************************************************
* Function Name  : mStopIfError
* Description    : ������״̬,�����������ʾ������벢ͣ��
* Input          : iError
* Output         : None
* Return         : None
*******************************************************************************/
void mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    //�����ɹ�
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* ��ʾ���� */
    while ( 1 )
    {
//      LED_TMP=0;                                                            //LED��˸
//      mDelaymS( 100 );
//      LED_TMP=1;
//      mDelaymS( 100 );
    }
}
UINT8X buf[100];
/*******************************************************************************
* Function Name  : main
* Description    : �������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main()
{
    UINT8X i,s,c;
    CfgFsys( );                                                                //CH549ʱ��ѡ������
    mDelaymS(20);
#if DE_PRINTF
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("SD Card demo start ...\n");
#endif
    CH549LibInit();                                                            //���ʼ��
#if DE_PRINTF
    printf("Wait SD Insert...\n");
#endif
    while(1)
    {
        CH549DiskConnect();
        if( CH549DiskStatus >= DISK_CONNECT )                                  //��������
        {
            if(CH549DiskStatus == DISK_CONNECT)                                //˵���ո����ӣ����ʼ����
            {
#if DE_PRINTF
                printf("Detect the Card\n");
#endif
                mDelaymS( 50 );                                                //�������ʱ
                for ( i = 0; i != 10; i ++ )
                {
#if DE_PRINTF
                    printf( "Wait DiskReady\n" );
#endif
                    s = CH549DiskInit();                                       //�ȴ�SD׼����
                    if ( s == ERR_SUCCESS )
                    {
                        break;
                    }
                    else
                    {
#if DE_PRINTF
                        printf("%02x\n",(UINT16)s);
#endif
                    }
                    mDelaymS( 50 );
                }
            }
            if( CH549DiskStatus >= DISK_MOUNTED )                              //���Խ����ļ�����
            {
                /* �����ļ���ʾ */
#if DE_PRINTF
                printf( "Create\n" );
#endif
                strcpy( mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          //���ļ���,�ڸ�Ŀ¼��,�����ļ���
                s = CH549FileCreate( );                                        //�½��ļ�����,����ļ��Ѿ���������ɾ�������½�
                mStopIfError( s );
                /* д�ļ���ʾ */
#if DE_PRINTF
                printf( "ByteWrite\n" );
#endif
                //ʵ��Ӧ���ж�д���ݳ��ȺͶ��建���������Ƿ������������ڻ�������������Ҫ���д��
                i = sprintf( buf,"Note: \xd\xa������������ֽ�Ϊ��λ����U���ļ���д,549����ʾ���ܡ�\xd\xa");
                for(c=0; c<10; c++)
                {
                    mCmdParam.ByteWrite.mByteCount = i;                        //ָ������д����ֽ���
                    mCmdParam.ByteWrite.mByteBuffer = buf;                     //ָ�򻺳���
                    s = CH549ByteWrite( );                                     //���ֽ�Ϊ��λ���ļ�д������
                    mStopIfError( s );
#if DE_PRINTF
                    printf("�ɹ�д�� %02X��\n",(UINT16)c);
#endif
                }
                /* �رղ������ļ� */
#if DE_PRINTF
                printf( "Close\n" );
#endif
                mCmdParam.Close.mUpdateLen = 1;                                //�Զ������ļ�����,���ֽ�Ϊ��λд�ļ�,�����ó����ر��ļ��Ա��Զ������ļ�����
                i = CH549FileClose( );
                mStopIfError( i );
                /* �ȴ����Ƴ� */
                while(1)
                {
                    CH549DiskConnect();
                    if( CH549DiskStatus == DISK_DISCONNECT )
                    {
                        break;
                    }
                }
            }
        }
    }
}