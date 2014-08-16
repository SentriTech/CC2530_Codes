//cd wxl      ����0������
#include <ioCC2530.h>
#include <string.h>
#include "hal_uart.h"

#define uint unsigned int
#define uchar unsigned char

//������ƵƵĶ˿�
#define led1 P1_0
#define led2 P1_1

//��������




/****************************************************************
*�������� ��������								
*��ڲ��� ����							
*�� �� ֵ ����						
*˵    �� ����							
****************************************************************/
/*void uartmain(void)
{	
        //P1 out
	P1DIR = 0x03; 				//P1����LED
	led1 = 0;
	led2 = 1;				//��LED

	halUartInit(HAL_UART_BAUDRATE_28800,HAL_UART_NO_PARITY+HAL_UART_EVEN_ODD_PARITY+HAL_UART_BIT9);  //��ʼ������

	halUartWrite(Txdata,29);	        //wu xian long tong xun
        memset(Txdata,' ',30);
        strcpy(Txdata,"www.c51rf.com\n");
        halUartWrite(Txdata,sizeof("www.c51rf.com "));
        memset(Txdata,' ',30);                  //����Դ�
        strcpy(Txdata,"www.wxlmcu.com\n");   //д�Դ�
        halUartWrite(Txdata,sizeof("www.wxlmcu.com "));
        memset(Txdata,' ',30);                  //����Դ�
        strcpy(Txdata,"www.wxlsoc.com\n");   //д�Դ�
        halUartWrite(Txdata,sizeof("www.wxlsoc.com "));

        memset(Txdata,' ',30);
        strcpy(Txdata,"UART0 TX test ");       //��UART0 TX test����Txdata;

	while(1)
	{
           halUartWrite(Txdata,sizeof("UART0 TX Test")); //���ڷ�������
            Delay(50000);                     //��ʱ
            Delay(50000);
            Delay(50000);
	}
}
*/


/****************************************************************
*�������� ����ʱ						
*��ڲ��� ��������ʱ									
*�� �� ֵ ����												
*˵    �� ��													
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
*�������� ����ʼ������1										
*��ڲ��� ����												
*�� �� ֵ ����							
*˵    �� ��57600-8-n-1						
****************************************************************/
void  halUartInit(uint16 baudrate, uint8 options)
{
    CLKCONCMD &= ~0x40;              //����
   // while(!(SLEEPCMD & 0x40));      //�ȴ������ȶ�
    CLKCONCMD &= ~0x47;             //TICHSPD128��Ƶ��CLKSPD����Ƶ
    SLEEPCMD |= 0x04; 		 //�رղ��õ�RC����
    
    PERCFG = 0x00;				//λ��1 P0��
    P0SEL = 0x3c;				//P0��������
    P2DIR &= ~0XC0;                             //P0������Ϊ����0
 
    U0CSR |= 0x80;	
    U0GCR |= baudrate%256;				//baud_e
    U0BAUD |= baudrate/256;				//��������Ϊ57600
 //   U0UCR = options;
    UTX0IF = 0; 
  
  /*CLKCON &= ~0x40;              //����
    while(!(SLEEP  & 0x40));      //�ȴ������ȶ�
    CLKCON &= ~0x47;             //TICHSPD128��Ƶ��CLKSPD����Ƶ
    SLEEP |= 0x04; 		 //�رղ��õ�RC����

    PERCFG = 0x00;				//λ��1 P0��
    P0SEL = 0x3c;				//P0��������
    P2DIR &= ~0XC0;                             //P0������Ϊ����0

    U0CSR |= 0x80;				//UART��ʽ
    U0GCR |= 10;				//baud_e
    U0BAUD |= 216;				//��������Ϊ57600
    UTX0IF = 0;  */
}

/****************************************************************
*�������� �����ڷ������ݺ���					
*��ڲ��� : buf:����									
*			length :���ݳ���							
*�� �� ֵ ��										
*˵    �� ��				
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
*�������� �����ڽ������ݺ���					
*��ڲ��� : buf:����									
*			length :���ݳ���							
*�� �� ֵ ��										
*˵    �� ��				
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
*�������� ��				
*��ڲ��� : ��									
*�� �� ֵ ��										
*˵    �� ��				
****************************************************************/
/*uint16 halUartGetNumRxBytes(void)
{
  


}*/
/****************************************************************
*�������� �����㲨����				
*��ڲ��� : Originrate:����									
*									
*�� �� ֵ �������16λ������ֵ											
*˵    �� ��				
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