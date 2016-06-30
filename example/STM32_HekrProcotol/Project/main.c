#include "stm32f10x.h"                  // Device header
#include "uart.h"
#include "hekr_protocol.h"

/*********************HEKR USER API*********************/

/*******************************************************/
//#define USER_MAX_LEN 0x64u
//使用前要确定用户所需要的最大数组 默认为100 大于100需要自行修改
//数组大小可以自行修改为最长长度
//如果有多条不等长命令，取最长长度 此处长度指用户数据长度 非整帧长度
/*******************************************************/


/*******************************************************/
//void HekrInit(void (*fun)(unsigned char));
//eg:  HekrInit(UART_SendChar);   
//HekrInit函数:
//传入参数为用户串口发送一个byte函数的函数名
//Hekr 协议初始化
//使用Hekr协议前需要完成初始化
//初始化需要用户有串口发送一个byte的程序
/*******************************************************/


/*******************************************************/
//eg:  void UART_SendChar(u8 ch); 传输参数必须只是一个8bit的数
//     该函数需要用户自行在程序中定义
/*******************************************************/


/*******************************************************/
//unsigned char HekrRecvDataHandle(unsigned char* data);
//串口数据接收处理
//返回值见头文件 RecvDataHandleCode
//数据保存在对应数组中 valid_data 和 ModuleStatus 指针
//模块状态值保存在module_status数组中
/*******************************************************/


/*******************************************************/
//void HekrValidDataUpload(unsigned char len);
//上传用户有效数据
//数据存放在valid_data数组中,len 为用户数据长度,非整帧长度
/*******************************************************/


//协议修改日期 2016.06.23 
//协议网址  http://docs.hekr.me/v4/resourceDownload/protocol/uart/
//BUG 反馈  zejun.zhao@hekr.me
//					387606503@qq.com
/*******************************************************/

u8 DateHandleFlag = 0;
unsigned char ProdKey[16] = {0x01,0x36,0xA6,0x6C,0x12,0x75,0x4E,0xE8,0x2F,0xFF,0x88,0x04,0xB7,0xFA,0xA5,0x3C};         //产品秘钥设置,共16个字节,当前示例ProdKey值对应LED多彩灯体验页面。ProdKey的获取和设置方法可参考:http://docs.hekr.me/v4/resourceDownload/protocol/uart/#44prodkey


void System_Init(void);

int main()
{
	u8 temp;
	u8 UserValidLen = 9;
	System_Init();
	HekrInit(UART3_SendChar);	
	HekrValidDataUpload(UserValidLen);
	
	/*************模块操作函数************/

	Module_State_Function();               //模块状态查询
  /*
	Module_Soft_Reboot_Function();         //模块软重启
	Module_Factory_Reset_Function();       //模块恢复出厂设置
	Hekr_Config_Function();                //模块进入一键配置模式
	Module_Set_Sleep_Function();           //模块进入休眠
	Module_Weakup_Function();              //模块休眠唤醒
	Module_Firmware_Versions_Function();   //模块版本查询
	Module_ProdKey_Get_Function();         //模块产品秘钥ProdKey查询
	Module_Factory_Test_Function();        //模块进入厂测模式
	Set_ProdKey(ProdKey);                  //模块ProdKey设置,用户根据从console平台获取的ProdKey值设置产品秘钥数组ProdKey[16]
  */
	
	while (1)
	{
		if(Uart3_Recv_STA && !DateHandleFlag)
		{
			DateHandleFlag = 1;
			Uart3_Recv_STA = 0;
		}
		if(DateHandleFlag)
		{ 
			temp = HekrRecvDataHandle(Uart3_Recv_Buffer);
			if(ValidDataUpdate == temp)
			{
				//接收的产品业务数据保存在 valid_data 数组里
				/************产品业务数据操作用户代码********/
				UART3_SendChar(valid_data[0]);
				/********************************************/
			}
			if(HekrModuleStateUpdate == temp)
			{
				//接收的模块状态取值保存在 ModuleStatus 指针里
				/*************模块状态操作用户代码************/
				UART3_SendChar(ModuleStatus->Mode);           //打印模块工作模式指示字节
				UART3_SendChar(ModuleStatus->WIFI_Status);    //打印模块WIFI状态指示字节
				UART3_SendChar(ModuleStatus->CloudStatus);    //打印模块云连接状态指示字节
				UART3_SendChar(ModuleStatus->SignalStrength); //打印模块状态查询应答帧保留字节
				/*************模块状态取值参考:http://docs.hekr.me/v4/resourceDownload/protocol/uart/#42  **********/
			}
			DateHandleFlag = 0;			
		}			
	} 
	return 0;
}



void System_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Uart3_init();
}

