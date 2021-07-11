
/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.1
* Date               : 2018/11/12
* Description        : CH549 Type-C DPʹ��
*                      ������UFPģʽ��CC1 �ⲿ����5.1K��ע����ҪBOOT����P14���գ�
*                      ��ΪPDͨѶ���ܵ�ˣ�Ȼ��DP��
*                      ע�⣺�ó������������32Mhz,+5V VDD,57600bps
*******************************************************************************/

#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\Type_C\PD.H"

#pragma  NOAREGS

/* �������� */
UINT8X Connect_Status = 0;                                                     //����״̬ 0:�Ͽ�  1:����

/*******************************************************************************
* Function Name  : PD_Init()
* Description    : Type-C UPFģʽ����,����ADC����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PD_Init( )
{
	P1_MOD_OC &= ~(bUCC2|bUCC1);                                                   
	P1_DIR_PU &= ~(bUCC2|bUCC1);                                                     //UCC1 UCC2 ���ø�������

	P2_MOD_OC &= ~(5<<2);                                                            //LED1 LED2�������
	P2_DIR_PU |= (5<<2);
	
	USB_C_CTRL |= bUCC1_PD_EN;                                                       //CC1��������5.1K��ʹ���ⲿ�������裩
	CCSel = 1;                                                                       //ѡ��CC1��
	USB_C_CTRL |= bUCC_PD_MOD;                                                       //BMC���ʹ��
	
	ADC_CFG |= (bADC_EN | bADC_AIN_EN|bVDD_REF_EN|bCMP_EN);                          //����ADCģ���Դ,�����ⲿͨ��,�����Ƚ�����ο���Դ���Ƚ��������ѡ��1/8VDD��
	ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);                                    //ѡ��ADC�ο�ʱ�� 750K
	ADC_CTRL = bADC_IF;                                                              //���ADCת����ɱ�־��д1����
	mDelayuS(2);                                                                     //�ȴ�ADC��Դ�ȶ�	
}
/*******************************************************************************
* Function Name  : Connect_Check(void)
* Description    : UPF���DPF��������
* Input          : None
* Output         : None
                   1 Ĭ�ϵ���
                   2 1.5A
                   3 3.0A
                   0xff ����
*******************************************************************************/
UINT8 Connect_Check( void )
{
	UINT16 UCC1_Value;
	
	ADC_CHAN = 4;                                                                   //CC1����������AIN4(P14)
	ADC_CTRL = bADC_START;                                                          //��������
	while((ADC_CTRL&bADC_IF) == 0);                                                 //��ѯ�ȴ���־��λ
	ADC_CTRL = bADC_IF;                                                             //���־
	UCC1_Value = ADC_DAT&0xFFF;
//	printf("UCC1=%d\n",(UINT16)UCC1_Value);
	
	if(UCC1_Value > DufaultPowerMin)
	{
		return DFP_PD_CONNECT;
	}
	else
	{
		return DFP_PD_DISCONNECT;
	}
}
/*******************************************************************************
* Function Name  : SearchVoltage
* Description    : ����Volt��ѹ������ӽ���
* Input          : Volt,��ѹֵ (mV)
* Output         : None
* Return         : ���ֵ
*                  �������0xFF,��ʾ��С��Ҳ������Ĵ�
*******************************************************************************/
UINT8 SearchVoltage ( UINT16 Volt ) 
{
	UINT8 NDORcv;
	UINT8 i;
	UINT8 number = 0xff;
	UINT16 Temp;
	UINT16 Cur_Temp = 0;                                                          //�ҵ�һ������ҽӽ���Volt*20����ֵ       

	NDORcv=Union_Header->HeaderStruct.NDO;

	for (i=0;i!=NDORcv;i++) 
	{
		Union_SrcCap = (_Union_SrcCap*)&RcvDataBuf[(i<<2)+2];
		Temp=((Union_SrcCap->SrcCapStruct.VoltH4<<6)+Union_SrcCap->SrcCapStruct.VoltL6);
		Temp*=50;
		if (Temp <= Volt ) 
		{
			if(Temp > Cur_Temp) 
			{
				Cur_Temp = Temp;
				number = i;
			}
		}
	}
	return number;
}
/*******************************************************************************
* Function Name  : main()
* Description    : Type CתDP
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main( ) 
{
	UINT8 index;
	UINT8 status;
	UINT16 ERR = 0;
	
	PD_Init();                                                                 //PD�ӿڳ�ʼ��
	CfgFsys( );                                                                //CH549ʱ��ѡ������ 
    mDelaymS(20);                                                              //�޸���Ƶ�����Լ���ʱ�ȴ�оƬ�����ȶ�
	mInitSTDIO( );                                                             //����0��ʼ��
    printf("Type-C DP start ...\n"); 
	Timer0Init();						                                       //Timer0��ʼ�� 3.33us��ʱ���ڲ�ѯ����
	
	while(1)
	{
		/* ���DFP�����ӣ�CC1ͨѶ */
		status = Connect_Check();
		if( status != DFP_PD_DISCONNECT )                                      //DFP����
		{
			ERR = 0;
			if(Connect_Status == 0)
			{
				printf("Con\n");
				Connect_Status = 1;
			}
		}
		else                                                                   //DFPδ����
		{
			if(Connect_Status)                                                 //���״̬
			{
				ERR++;
				if(ERR == 1000)
				{
					ERR = 0;
					printf("DCon\n");
					Connect_Status = 0;
				}
			}
		}
		
		if(Connect_Status)                                                     //�����ӣ��˴����ݽ��մ���
		{
			status = ReceiveHandle( CCSel );
			if(status==0)                                                      //�������ݵ���
			{
				switch (Union_Header->HeaderStruct.MsgType)
				{
					case SourceSendCap:
						index = SearchVoltage ( 5000 );                          //��ѹѡ��
						if(index != 0xFF)
						{
							Union_SrcCap = (_Union_SrcCap*)&RcvDataBuf[(index<<2)+2];
							printf("Volt:%d mV\n",(UINT16)(((Union_SrcCap->SrcCapStruct.VoltH4<<6)+(Union_SrcCap->SrcCapStruct.VoltL6))*50));
							printf("Current:%d mA\n",(UINT16)(Union_SrcCap->SrcCapStruct.Current*10) );
							
							ResetSndHeader();
							Union_Header->HeaderStruct.PortPwrRole = PwrRoleSink;
							Union_Header->HeaderStruct.PortDataRole = DataRoleUFP;
							Union_Header->HeaderStruct.NDO = 1;                //1 Byte����
							Union_Header->HeaderStruct.MsgType = SinkSendRequest;
							RcvDataBuf[(index<<2)+2] = ((index+1)<<4);
							RcvDataBuf[(index<<2)+3] &= 0x0f;
							
							RcvDataBuf[(index<<2)+3] &= 0xf0;
							RcvDataBuf[(index<<2)+4] &= 0x03;							
							RcvDataBuf[(index<<2)+4] |= ((RcvDataBuf[(index<<2)+5] &0x3f)<<2);
							RcvDataBuf[(index<<2)+3] |= ((RcvDataBuf[(index<<2)+5]>>6)+ ((RcvDataBuf[(index<<2)+4]&0x03)<<2));
							memcpy(&SndDataBuf[2],&RcvDataBuf[(index<<2)+2],4);
							SendHandle(CCSel,SOP);
						}
						else
						{
							printf("No Matched Volt.\n");
						}
						break;
					case Accept:
						printf("Accept\n");
						break;
					case PS_RDY:
						printf("Ready\n");
						break;
					case GetSinkCap:
						ResetSndHeader();
						Union_Header->HeaderStruct.PortPwrRole = PwrRoleSink;
						Union_Header->HeaderStruct.PortDataRole = DataRoleUFP;
						Union_Header->HeaderStruct.NDO = 1;
						Union_Header->HeaderStruct.MsgType = SinkCap;
						SndDataBuf[2]=0x20;
						SndDataBuf[3]=0x01;
						SndDataBuf[4]=0x90;
						SndDataBuf[5]=0x64;
						SendHandle(CCSel,SOP);						
						break;
					case REJECT:
						printf("Reject\n");
						break;
					case SoftRst:
						printf("Soft Reset\n");
						break;
					default :
						printf("MsgType: %02x\n",(UINT16)Union_Header->HeaderStruct.MsgType);
					break;
				}
			}	
		}
	}
}