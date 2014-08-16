#include <hal_led.h>
#include <hal_assert.h>
#include <hal_board.h>
#include <hal_int.h>
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h"

#define RF_CHANNEL                24      // 2.4 GHz RF channel

#define PAN_ID                0x2008
#define NODE_NUMBER           24
#define NODE_ID               75
#define LOCAL_ADDR            0x2520 + NODE_ID
#define ALL_ADDR              0xFFFF
/******************************�޸�Ϊ��3******************************/
#define APP_PAYLOAD_LENGTH    NODE_NUMBER+3   


/************************���һ��24*27������*******************************/
uint8 Data[NODE_NUMBER][APP_PAYLOAD_LENGTH];
uint8 count=0;
uint8 temp=0;                              //���ڽ��ܴ�������
uint8 Order1[2]={0,0};
uint8 Choose=72;
uint8 Loop=0;

#define First 72
#define Second 73
#define Third 74
/**************************************************************************/


const unsigned char AA[2]={0X00,0XFF};
uint8 pRxData[APP_PAYLOAD_LENGTH];

void clearBuff(void)
{
    int i;
    for(i = 0; i<APP_PAYLOAD_LENGTH; i++)
    {
        pRxData[i] = 0;
    }
}

//��ӷ��ͳ���
void packetSend(uint16 destAddr, uint8* pTxData, uint8 length)
{
    //halLedSet(1);
    basicRfReceiveOff();
    basicRfSendPacket(destAddr, pTxData, length);
    basicRfReceiveOn();
    //halLedClear(1);

}



void main(void)
{
  
  
  
    basicRfCfg_t basicRfConfig;
    int16* rssi;
    uint8 rss;
    uint8 length;

    basicRfConfig.panId = PAN_ID;
    basicRfConfig.channel = RF_CHANNEL;
    basicRfConfig.ackRequest = FALSE;
    basicRfConfig.myAddr = LOCAL_ADDR;

    halBoardInit();
    
/**************************************************************/ 
    halLedSet(1);                //����˸,�����λ�ˣ��ܿ���
    halMcuWaitMs(100);
    halLedClear(1);
    halMcuWaitMs(100);
    halLedSet(2);
    halMcuWaitMs(100);
    halLedClear(2);
    halMcuWaitMs(100);

/**************************************************************/
    
    clearBuff();
    
    halUartInit(HAL_UART_BAUDRATE_115200, 0);
   
    //���ڸĶ���������    
    U0CSR |= 0x40; 
    EA = 1;        //ȫ���ж�
    URX0IE = 1;    //�����ж�ʹ��
    

    if(basicRfInit(&basicRfConfig)==FAILED) {
        HAL_ASSERT(FALSE);
    }
    
    basicRfInit(&basicRfConfig);
    
    while (TRUE)
    {  

 /**       if(0==Loop)   //�յ�1ʱ ���ƽڵ�1���й���  �ı�ڵ�PIN ID
       {          
          basicRfConfig.panId=0x2008;
          basicRfConfig.channel=24;
          halBoardInit();
          basicRfInit(&basicRfConfig);
          Choose=First;
       }
       
       if(1==Loop)   //�յ�2ʱ ���ƽڵ�2���й���  �ı�ڵ�PIN ID
       {          
          basicRfConfig.panId=0x2008+3;
          basicRfConfig.channel=23;
          halBoardInit();
          basicRfInit(&basicRfConfig);
          Choose=Second;
       }
       
       else   //�յ�3ʱ ���ƽڵ�3���й���  �ı�ڵ�PIN ID
       {          
          basicRfConfig.panId=0x2008+6;
          basicRfConfig.channel=22;
          halBoardInit();
          basicRfInit(&basicRfConfig);
          Choose=Third;
       }
****/
      
      if(1==temp)   //�յ�1ʱ ���ƽڵ�1���й���  �ı�ڵ�PIN ID
       {          
          basicRfConfig.panId=0x2008;
          basicRfConfig.channel=24;
          halBoardInit();
          while(basicRfInit(&basicRfConfig)==FAILED);
          packetSend(ALL_ADDR,Order1,2);
          temp=0;
          Loop=2;
          Choose=First;
       }
       
       if(2==temp)   //�յ�2ʱ ���ƽڵ�2���й���  �ı�ڵ�PIN ID
       {          
          basicRfConfig.panId=0x2008+3;
          basicRfConfig.channel=23;
          halBoardInit();
          while(basicRfInit(&basicRfConfig)==FAILED);
          packetSend(ALL_ADDR,Order1,2);
          temp=0;
          Loop=3;
          Choose=Second;
       }
       
       if(3==temp)   //�յ�3ʱ ���ƽڵ�3���й���  �ı�ڵ�PIN ID
       {          
          basicRfConfig.panId=0x2008+6;
          basicRfConfig.channel=22;
          halBoardInit();
          while(basicRfInit(&basicRfConfig)==FAILED);
          packetSend(ALL_ADDR,Order1,2);
          temp=0;
          Loop=2;
          Choose=Third;
       }

        

       if(basicRfPacketIsReady())
       {
         halLedSet(2);                //����˸,�����λ�ˣ��ܿ���
         halMcuWaitMs(1);
         halLedClear(2);
         length=basicRfReceive(pRxData, APP_PAYLOAD_LENGTH, rssi); 
              
         if(length==APP_PAYLOAD_LENGTH)
         {               
            if(pRxData[APP_PAYLOAD_LENGTH-1]==Choose)
            {
                count++;
                for(uint8 i=0;i!=APP_PAYLOAD_LENGTH;i++)
                {
                  Data[*pRxData-51][i]=pRxData[i];
                }
            }
  
         }
           if(count>=24)
           {
              temp=Loop;
              count=0;
              for(uint8 i=0;i!=NODE_NUMBER;i++)
              {
                halUartWrite(Data[i],27);
               
              }
           }
         clearBuff();
       }
 
    }    
}







#pragma vector = URX0_VECTOR
 __interrupt void UART0_ISR(void)
 {
       EA=0;
       temp = U0DBUF;
       halLedSet(1);                //����˸,�����λ�ˣ��ܿ���
       halMcuWaitMs(1);
       halLedClear(1);
       URX0IF = 0;                 //��������
       EA=1;
 }