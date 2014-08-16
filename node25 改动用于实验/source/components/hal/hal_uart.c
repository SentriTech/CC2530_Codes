//cd wxl      串口0发数据
#include <ioCC2530.h>
#include <string.h>
#include "hal_uart.h"

#define uint unsigned int
#define uchar unsigned char

//定义控制灯的端口
#define led1 P1_0
#define led2 P1_1

//函数声明




/****************************************************************
*函数功能 ：主函数								
*入口参数 ：无							
*返 回 值 ：无						
*说    明 ：无							
****************************************************************/
/*void uartmain(void)
{	
        //P1 out
	P1DIR = 0x03; 				//P1控制LED
	led1 = 0;
	led2 = 1;				//关LED

	halUartInit(HAL_UART_BAUDRATE_28800,HAL_UART_NO_PARITY+HAL_UART_EVEN_ODD_PARITY+HAL_UART_BIT9);  //初始化串口

	halUartWrite(Txdata,29);	        //wu xian long tong xun
        memset(Txdata,' ',30);
        strcpy(Txdata,"www.c51rf.com\n");
        halUartWrite(Txdata,sizeof("www.c51rf.com "));
        memset(Txdata,' ',30);                  //清除显存
        strcpy(Txdata,"www.wxlmcu.com\n");   //写显存
        halUartWrite(Txdata,sizeof("www.wxlmcu.com "));
        memset(Txdata,' ',30);                  //清除显存
        strcpy(Txdata,"www.wxlsoc.com\n");   //写显存
        halUartWrite(Txdata,sizeof("www.wxlsoc.com "));

        memset(Txdata,' ',30);
        strcpy(Txdata,"UART0 TX test ");       //将UART0 TX test赋给Txdata;

	while(1)
	{
           halUartWrite(Txdata,sizeof("UART0 TX Test")); //串口发送数据
            Delay(50000);                     //延时
            Delay(50000);
            Delay(50000);
	}
}
*/


/****************************************************************
*函数功能 ：延时						
*入口参数 ：定性延时									
*返 回 值 ：无												
*说    明 ：													
****************************************************************/
void Delay(uint16 n)
{
	uint8 i;
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
}

/****************************************************************
*函数功能 ：初始化串口1										
*入口参数 ：无												
*返 回 值 ：无							
*说    明 ：57600-8-n-1						
****************************************************************/
void  halUartInit(uint16 baudrate, uint8 options)
{
    CLKCONCMD &= ~0x40;              //晶振
   // while(!(SLEEPCMD & 0x40));      //等待晶振稳定
    CLKCONCMD &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEPCMD |= 0x04; 		 //关闭不用的RC振荡器
    
    PERCFG = 0x00;				//位置1 P0口
    P0SEL = 0x3c;				//P0用作串口
    P2DIR &= ~0XC0;                             //P0优先作为串口0
 
    U0CSR |= 0x80;	
    U0GCR |= baudrate%256;				//baud_e
    U0BAUD |= baudrate/256;				//波特率设为57600
 //   U0UCR = options;
    UTX0IF = 0; 
  
  /*CLKCON &= ~0x40;              //晶振
    while(!(SLEEP  & 0x40));      //等待晶振稳定
    CLKCON &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEP |= 0x04; 		 //关闭不用的RC振荡器

    PERCFG = 0x00;				//位置1 P0口
    P0SEL = 0x3c;				//P0用作串口
    P2DIR &= ~0XC0;                             //P0优先作为串口0

    U0CSR |= 0x80;				//UART方式
    U0GCR |= 10;				//baud_e
    U0BAUD |= 216;				//波特率设为57600
    UTX0IF = 0;  */
}

/****************************************************************
*函数功能 ：串口发送数据函数					
*入口参数 : buf:数据									
*			length :数据长度							
*返 回 值 ：										
*说    明 ：				
****************************************************************/
void halUartWrite(const char* buf, uint16 length)
{
  int j;
  for(j=0;j<length;j++)
  {
    U0DBUF = *buf++;
    while(UTX0IF == 0);
    UTX0IF = 0;
  }

}
/****************************************************************
*函数功能 ：串口接收数据函数					
*入口参数 : buf:数据									
*			length :数据长度							
*返 回 值 ：										
*说    明 ：				
****************************************************************/
void halUartRead(char* buf, uint16 length)
{
    int j;
    for(j=0;j<length;j++)
    {
        *buf++ = U0DBUF ;
        while(URX0IF == 0);
        URX0IF = 0;
    }

}
/****************************************************************
*函数功能 ：				
*入口参数 : 无									
*返 回 值 ：										
*说    明 ：				
****************************************************************/
/*uint16 halUartGetNumRxBytes(void)
{
  


}*/
/****************************************************************
*函数功能 ：换算波特率				
*入口参数 : Originrate:数据									
*									
*返 回 值 ：换算后16位波特率值											
*说    明 ：				
****************************************************************/
/*uint16 halComputeBaud(uint32 Originrate)
{
  uint16 result;
  uint8 baudm,baude;
  double i,f;
  i=Originrate*(0x10000000);
  if()
   f=16000000;
  else
    f=32000000;
  i/=f;


  result=baudm*256+baude;  

}
*/