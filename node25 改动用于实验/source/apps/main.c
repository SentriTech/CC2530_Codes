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

/******************************�޸�Ϊ��3******************************/
#define APP_PAYLOAD_LENGTH    NODE_NUMBER+3   


/************************���һ��24*27������*******************************/
uint8 Data[NODE_NUMBER][APP_PAYLOAD_LENGTH];
uint8 count=0;
/**************************************************************************/


const unsigned char AA[2]={0XFF,0XFF};
uint8 pRxData[APP_PAYLOAD_LENGTH];

void clearBuff(void)
{
    int i;
    for(i = 0; i<APP_PAYLOAD_LENGTH; i++)
    {
        pRxData[i] = 0;
    }
}
/************************���Ź���ʼ��***************************/
void init_watchdag(void)
{
  WDCTL=0X00;//ʱ����Ϊ1��
  WDCTL |=0X08;//���Ź�ģʽ
}

//������Ź���ʱ��
void feetdog(void)
{
  WDCTL=0XA0;
  WDCTL=0X50;
}
/***************************************************/
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
   // init_watchdag();             //���п��Ź���ʼ��   
    
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
    
    if(basicRfInit(&basicRfConfig)==FAILED) {
        HAL_ASSERT(FALSE);
    }
    
    basicRfReceiveOn();
    
    while (TRUE)
    {
        
       if(basicRfPacketIsReady())
       {
         
         length=basicRfReceive(pRxData, APP_PAYLOAD_LENGTH, rssi); 
         
       /*  if(length==APP_PAYLOAD_LENGTH-1)
         {                     
            if(*pRxData>50)
         {
           rss=basicRfGetRssi();
           halLedSet(1);
           halRfDisableRxInterrupt();
           halUartWrite(pRxData, length);
           halRfEnableRxInterrupt();
           halLedClear(1);
         }
         }
  */
          
         if(length==APP_PAYLOAD_LENGTH)
         {          
        
           
            if(pRxData[APP_PAYLOAD_LENGTH-1]==52)
            {
                count++;
                for(uint8 i=0;i!=APP_PAYLOAD_LENGTH;i++)
                {
                  Data[*pRxData-51][i]=pRxData[i];
                }
            }
            

           
         }
           if(count==24)
           {
              count=0;
              for(uint8 i=0;i!=NODE_NUMBER;i++)
              {
                halUartWrite(Data[i],27);
              }
           }



         
         clearBuff();
       }

/**************************************************************/ 
       // feetdog();                   //ÿ��ѭ����ι��   
/**************************************************************/ 
     
    }    
}