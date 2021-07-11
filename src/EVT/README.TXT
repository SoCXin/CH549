
南京沁恒电子有限公司 2018.10
最新资料请到我们的网站上查询
http://wch.cn


文件目录
  |
  |-- README.TXT：目录文件说明
  |-- DS
  |      |--CH549DS1.PDF：CH549应用手册
  |-- EVT
  |      |-- EXAM: C例程
  |      |      |--CH549.uvproj：CH549 keil4工程文件
  |      |      |--obj：CH549 keil4 EXAM目标文件目录    
  |      |      |--Public
  |      |      |      |--CH549.H：CH549 C语言头文件，SFR定义
  |      |      |      |--Debug.C：UART0&1初始化、常用延时函数、UART0&1数据收发子函数,看门狗初始化
  |      |      |      |--Debug.H：UART0&1初始化、常用延时函数、UART0&1数据收发子函数、看门狗初始化声明
  |      |      |--USB
  |      |      |      |-- Device
  |      |      |      |      |-- CompositeKM.C:模拟键鼠例程，支持部分类命令
  |      |      |      |      |-- CompositeKM.hex 
  |      |      |      |      |-- VendorDefinedDev.C:模拟厂商自定义设备，需要安装CH372驱动，可以通过372test.exe调试(批量数据收发)
  |      |      |      |      |-- VendorDefinedDev.hex
  |      |      |      |      |-- CompatibilityHID.C:模拟HID兼容设备，支持中断上下传
  |      |      |      |      |-- CompatibilityHID.hex  
  |      |      |      |-- Host
  |      |      |      |      |-- USBHostHUB_KM.C: USB主机应用例子,初始化和枚举USB端口连接的设备,支持一级外部HUB,可以操作USB键鼠和HUB，打印机,包含HID类命令处理
  |      |      |      |      |-- USBHostHUB_KM.hex:   
  |      |      |      |      |-- TOOL AOA安卓通讯手机APK    
  |      |      |      |-- U_DISK
  |      |      |      |      |      |-- EXAM1.C:C示例源程序,以字节为单位读写文件，包括文件创建、删除、修改文件属性,修改文件名
  |      |      |      |      |      |-- EXAM1.hex
  |      |      |      |      |      |-- EXAM11.C:C示例源程序,枚举根目录或者指定目下的文件
  |      |      |      |      |      |-- EXAM11.hex:
  |      |      |      |-- USB_LIB
  |      |      |      |      |-- CH549UFI.C&H:CH549 U盘读写文件系统库常用接口函数
  |      |      |      |      |-- CH549UFI.LIB:CH549 U盘文件系统库文件(编译器内存模式设置成:small)
  |      |      |      |      |-- USBHOST.C&H:CH549 USB主机控制传输函数定义  
  |      |      |--Type-C
  |      |      |      |-- DFP_UFP  
  |      |      |      |-- Main.C:Type-C 主从模式使用示例
  |      |      |      |-- Type_C.C&H&Hex:Type-C主从模式配置函数定义 
  |      |      |      |-- PD  
  |      |      |      |-- Main.C:Type-C PD请求电源通讯使用示例
  |      |      |      |-- PD.H&Hex:Type-C PD变量/函数声明和目标文件   
  |      |      |      |-- PD_LIB.LIB:Type-C PD底层通讯LIB  
  |      |      |--TouchKey
  |      |      |      |-- Main.C:CH549 触摸按键中断和查询方式进行采集并报告当前采样通道按键状态，包含初始化和按键采样等演示函数  
  |      |      |      |-- TouchKey.C&H:TouchKey开启，通道切换和中断采集函数
  |      |      |      |-- TouchKey.hex
  |      |      |--Compare
  |      |      |      |-- Main.C:CH549 比较器示例，比较器使能，通道设置，中断/查询方式查询结果  
  |      |      |      |-- Compare.C&H:比较器使能，通道选择，中断函数
  |      |      |      |-- Compare.hex    
  |      |      |--ADC：
  |      |      |      |-- Main.C:ADC使用示例
  |      |      |      |-- ADC.C&H:ADC初始化，采样时钟设定，通道变化和电压比较功能函数定义
  |      |      |      |-- ADC.hex
  |      |      |--FlashRom
  |      |      |      |-- Main.C:DataFlash使用示例
  |      |      |      |-- FlashRom.C&H:DataFlash读写函数定义
  |      |      |      |-- FlashRom.hex
  |      |      |--IAP
  |      |      |      |--IAP
  |      |      |      |     |-- CH549_IAP.C&H:IAP功能C程序，支持串口和USB方式升级用户程序
  |      |      |      |     |-- CH549_IAP.hex:IAP功能hex文件
  |      |      |      |--User
  |      |      |      |     |-- CH549_USER.C&H: CH549 配合IAP的应用程序，通过按键进入IAP
  |      |      |      |     |-- CH549_USER.hex:配合IAP功能用户程序hex文件
  |      |      |      |--WIN 
  |      |      |      |     |-- WCHMCUIAP_WINAPP.exe:CH549 配合例程IAP的上位机工具，支持串口和USB下载  
  |      |      |      |     |-- WCHMCUIAP_WINAPP.exe源码    
  |      |      |--GPIO
  |      |      |      |-- Main.C:GPIO使用示例
  |      |      |      |-- GPIO.C&H:GPIO设置函数定义
  |      |      |      |-- GPIO.hex
  |      |      |--PWM
  |      |      |      |-- Main.C:PWM使用示例
  |      |      |      |-- PWM.C&H:PWM初始化、设置函数定义
  |      |      |      |-- PWM.hex
  |      |      |--SPI
  |      |      |      |-- SPI.C&H:SPI主从设置和收发函数定义 
  |      |      |      |      |-- SPI_SLAVE
  |      |      |      |      |      |-- MainSlave.C&hex 从机收发演示程序     
  |      |      |      |      |-- SPI_MASTER  
  |      |      |      |      |      |-- MainMaster.C&Hex SPI主机收发演示程序    
  |      |      |      |      |-- SPI_FLASH 
  |      |      |      |      |      |-- Main.C 
  |      |      |      |      |      |-- SPIFlash.C&H&Hex SPI操作外部Flash接口函数      
  |      |      |      |      |-- SPI_SDCARD     
  |      |      |      |      |      |-- Main.C
  |      |      |      |      |      |-- SD.C&H&Hex SPI控制读写SD卡    
  |      |      |--Timer
  |      |      |      |-- Main.C:Timer0&1&2定时，计数设置和捕捉功能使用示例
  |      |      |      |-- Timer.C&H:Timer0&1&2定时，计数设置和捕捉函数定义
  |      |      |      |-- Timer.hex  
  |      |-- PUB: 评估板说明
  |      |      |-- CH549EVT.PDF: 评估板和EVT资料包说明
  |      |      |-- CH549SCH.PDF: 评估板原理图
  
2018-10-8 14:29
1、初版发行;

2019-09-18 20:15
1、ADC
修改ADC.C、ADC.H和Main.C文件：
1、ADC的main.C通道选择错误，ADC_ChSelect(1<<ch)改成ADC_ChSelect(ch);
2、ADC的ADC.H通道定义错误，#define  CH0  0x0001改成#define  CH0   0，以此类推；
3、ADC.C的void ADC_ChSelect( UINT8 ch ) 函数，通道选择配置引脚模式错误；
4、ADC.C中断开启错误，30行     SCON2 &= bU2IE;应改成    SCON2 &= ~bU2IE;

2、Touchkey
修改TouchKey.C和Main.C文件：
1、增加滤波函数，解决评估板按下触摸按键偶尔会有检测到抬起的异常情况；
2、TouchKey.C中断开启错误，31行     SCON2 &= bU2IE;应改成    SCON2 &= ~bU2IE;
                 


