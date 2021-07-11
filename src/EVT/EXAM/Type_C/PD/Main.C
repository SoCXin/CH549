/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/27
* Description        : CH549 Type-C PDʹ��
*                      ������DFPģʽ���Զ����UFP�豸�����壬���ƶ�Ӧ��CC��PDͨѶ
*                      ��ΪPDͨѶ�Ĺ����
*                      ע�⣺�ó������������32Mhz,+5V VDD,38400bps
*******************************************************************************/
#include "CH549.H"
#include "DEBUG.H"
#include "PD.H"
#pragma  NOAREGS
/* UFP���ӡ��Ͽ�״̬
 * 0:�Ͽ�
 * 1:����
 * 2:PDͨѶ��� */
UINT8X Connect_Status = 0;                                                      //UFP����״̬
#if (POWER_ROLE == SOURCE)
/*******************************************************************************
* Function Name  : main
* Description    : ���������
* Input          : None
* Return         : None
*******************************************************************************/
void main( )
{
    UINT8 status;
    UINT8 rev_flag = 0;
    UINT16 j = 0;
    CfgFsys( );                                                                //CH549ʱ��ѡ������
    mDelaymS(20);                                                              //�޸���Ƶ�����Լ���ʱ�ȴ�оƬ�����ȶ�
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("PD Source start ...\n");
    Timer0Init();                                                              //Timer0��ʼ�� 3.33us��ʱ���ڲ�ѯ����
    PD_Source_Init();                                                          //PD�ӿڳ�ʼ��
    while(1)
    {
        status = DFP_Insert();
        if( status )                                                           //UFP����
        {
            if(Connect_Status == 0)
            {
                printf("connected\n");
                Connect_Status = 1;
                CCSel = status;                                                //CC��ѡ��
            }
        }
        else
        {
            if(Connect_Status)                                                 //���״̬
            {
                printf("disconnected\n");
                Connect_Status = 0;
                CCSel = 0;
                VBUSG = 1;                                                     //�ر�VBUS��Դ
            }
        }
        if(Connect_Status == 1)                                                //������
        {
            VBUSG = 0;                                                         //��VBUS��Դ
            mDelaymS(300);
            printf("CC%d Start Communication\n",(UINT16)CCSel);
            /* 1�����Header,׼������ Source Send Capbilityָ�� */
            ResetSndHeader();
//          Union_Header->HeaderStruct.NDO = 2;                                //3 Byte���ݣ����7��
//          Union_Header->HeaderStruct.MsgType = SourceSendCap;
//          /* 2���������Sink��ѡ��ĵ�ѹ����ֵ */
//          memcpy(&SndDataBuf[2],SrcCap_Table,8);
            /* 3�����õײ㷢�ͺ���(����PRE��SOF��CRC32��EOP��4B5B���롢BMC����) */
            Config_SourceCap();
            if(SendHandle(CCSel) == 0)                                         //���ͳɹ�,׼������
            {
                while(1)
                {
                    rev_flag = ReceiveHandle ( CCSel );
                    if(rev_flag)                                               //�������ݵ���
                    {
                        if (GetMsgType() == SINK_SEND_REQUEST)
                        {
                            ResetSndHeader();                                  //�ظ� Accept
                            SetMsgType(SOURCE_SEND_ACCEPT);
                            if(SendHandle(CCSel)==0)
                            {
                                mDelaymS(150);
                                /* ׼����ѹ���� */
                                ResetSndHeader();                               //�ظ� PS_RDY
                                SetMsgType(PS_RDY);                             //�ظ� Accept
                                if(SendHandle(CCSel)==0)
                                {
                                    printf("PD PS_RDY Success.\n");
                                    Connect_Status = 2;                         //ͨѶ����
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
#else
void main( )
{
    UINT8 index;
    UINT8 status;
    UINT8 msgType;
    UINT16 Voltage;
    CfgFsys();                                                                  /* CH549ʱ��ѡ������ */
    mDelaymS(20);                                                               /* �޸���Ƶ�����Լ���ʱ�ȴ�оƬ�����ȶ� */
    mInitSTDIO();                                                               /* ����0��ʼ�� */
    printf("PD Sink start ...\n");
    Timer0Init();                                                                           /* Timer0��ʼ�� 3.33us��ʱ���ڲ�ѯ���� */
    PD_Sink_Init();                                                             /* PD�ӿڳ�ʼ�� */
    Voltage = 14800;                                                                                          /* �����ѹ����λmV */
    while(1)
    {
        /* ���DFP�����ӣ�ȷ��CCͨѶ�ܽ� */
        status = Connect_Check();
        if( status != DFP_DISCONNECT )                                            /* DFP���� */
        {
            if(Connect_Status == 0)
            {
                printf("cc:%d,connected:%d\n",(UINT16)CCSel,(UINT16)status);
                Connect_Status = DFP_STAT_CONNECT;
            }
        }
        else                                                                     /* DFPδ���� */
        {
            if(Connect_Status)                                                     /* ���״̬ */
            {
                printf("disconnected:%d\n",(UINT16)status);
                Connect_Status = DFP_STAT_DISCONNECT;
                VBUSG = 0;                                                           /* �ر�VBUS��Դ */
            }
        }
        while(Connect_Status)                                                    /* �����ӣ��˴����ݽ��մ��� */
        {
            status = ReceiveHandle ( CCSel );
            if(status==0)                                                          /* �������ݵ��� */
            {
                msgType = GetMsgType();
                switch (msgType)
                {
                case SOURCE_SEND_CAP:
                    index = SearchVoltage ( Voltage );                                 /* ��ѹѡ��,��λ mV */
                    if(index != 0xFF)
                    {
                        printf("Matched Volt: %dmV\n", Voltage);
                        PrepareReqData(index);
                        SendHandle(CCSel);                                                             /* ���������Դ��Ϣ */
                    }
                    else
                    {
                        printf("No Matched Volt.\n");
                    }
                    break;
                case SOURCE_SEND_ACCEPT:
                    printf("Accept\n");
                    break;
                case PS_RDY:
                    printf("Ready\n");
                    VBUSG = 0;                                                       /* PDͨѶ��ɣ� ״̬��־��λ */
                    break;
                case REJECT:
                    printf("Reject\n");
                    break;
                default :
                    printf("MsgType: %02x\n",(UINT16)msgType);
                    break;
                }
            }
//          status = Connect_Check();
        }
    }
}
#endif