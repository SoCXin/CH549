
/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.1
* Date               : 2018/11/12
* Description        : CH549 Type-C DP使用
*                      工作在UFP模式，CC1 外部下拉5.1K（注意需要BOOT配置P14浮空）
*                      作为PD通讯的受电端，然后DP。
*                      注意：该程序测试条件：32Mhz,+5V VDD,57600bps
*******************************************************************************/

#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\Type_C\PD.H"

#pragma  NOAREGS

/* 变量定义 */
UINT8X Connect_Status = 0;                                                     //连接状态 0:断开  1:连接

/*******************************************************************************
* Function Name  : PD_Init()
* Description    : Type-C UPF模式配置,引脚ADC配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PD_Init( )
{
	P1_MOD_OC &= ~(bUCC2|bUCC1);                                                   
	P1_DIR_PU &= ~(bUCC2|bUCC1);                                                     //UCC1 UCC2 设置浮空输入

	P2_MOD_OC &= ~(5<<2);                                                            //LED1 LED2推挽输出
	P2_DIR_PU |= (5<<2);
	
	USB_C_CTRL |= bUCC1_PD_EN;                                                       //CC1引脚下拉5.1K（使用外部下拉电阻）
	CCSel = 1;                                                                       //选择CC1脚
	USB_C_CTRL |= bUCC_PD_MOD;                                                       //BMC输出使能
	
	ADC_CFG |= (bADC_EN | bADC_AIN_EN|bVDD_REF_EN|bCMP_EN);                          //开启ADC模块电源,开启外部通道,开启比较器与参考电源（比较器反向端选择1/8VDD）
	ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);                                    //选择ADC参考时钟 750K
	ADC_CTRL = bADC_IF;                                                              //清除ADC转换完成标志，写1清零
	mDelayuS(2);                                                                     //等待ADC电源稳定	
}
/*******************************************************************************
* Function Name  : Connect_Check(void)
* Description    : UPF检测DPF供电能力
* Input          : None
* Output         : None
                   1 默认电流
                   2 1.5A
                   3 3.0A
                   0xff 悬空
*******************************************************************************/
UINT8 Connect_Check( void )
{
	UINT16 UCC1_Value;
	
	ADC_CHAN = 4;                                                                   //CC1引脚连接至AIN4(P14)
	ADC_CTRL = bADC_START;                                                          //启动采样
	while((ADC_CTRL&bADC_IF) == 0);                                                 //查询等待标志置位
	ADC_CTRL = bADC_IF;                                                             //清标志
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
* Description    : 搜索Volt电压以下最接近的
* Input          : Volt,电压值 (mV)
* Output         : None
* Return         : 序号值
*                  如果返回0xFF,表示最小的也比请求的大
*******************************************************************************/
UINT8 SearchVoltage ( UINT16 Volt ) 
{
	UINT8 NDORcv;
	UINT8 i;
	UINT8 number = 0xff;
	UINT16 Temp;
	UINT16 Cur_Temp = 0;                                                          //找到一个最大且接近（Volt*20）的值       

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
* Description    : Type C转DP
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main( ) 
{
	UINT8 index;
	UINT8 status;
	UINT16 ERR = 0;
	
	PD_Init();                                                                 //PD接口初始化
	CfgFsys( );                                                                //CH549时钟选择配置 
    mDelaymS(20);                                                              //修改主频建议稍加延时等待芯片供电稳定
	mInitSTDIO( );                                                             //串口0初始化
    printf("Type-C DP start ...\n"); 
	Timer0Init();						                                       //Timer0初始化 3.33us定时用于查询发送
	
	while(1)
	{
		/* 检测DFP的连接，CC1通讯 */
		status = Connect_Check();
		if( status != DFP_PD_DISCONNECT )                                      //DFP连接
		{
			ERR = 0;
			if(Connect_Status == 0)
			{
				printf("Con\n");
				Connect_Status = 1;
			}
		}
		else                                                                   //DFP未连接
		{
			if(Connect_Status)                                                 //清除状态
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
		
		if(Connect_Status)                                                     //已连接，此处数据接收处理
		{
			status = ReceiveHandle( CCSel );
			if(status==0)                                                      //有新数据到达
			{
				switch (Union_Header->HeaderStruct.MsgType)
				{
					case SourceSendCap:
						index = SearchVoltage ( 5000 );                          //电压选择
						if(index != 0xFF)
						{
							Union_SrcCap = (_Union_SrcCap*)&RcvDataBuf[(index<<2)+2];
							printf("Volt:%d mV\n",(UINT16)(((Union_SrcCap->SrcCapStruct.VoltH4<<6)+(Union_SrcCap->SrcCapStruct.VoltL6))*50));
							printf("Current:%d mA\n",(UINT16)(Union_SrcCap->SrcCapStruct.Current*10) );
							
							ResetSndHeader();
							Union_Header->HeaderStruct.PortPwrRole = PwrRoleSink;
							Union_Header->HeaderStruct.PortDataRole = DataRoleUFP;
							Union_Header->HeaderStruct.NDO = 1;                //1 Byte数据
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