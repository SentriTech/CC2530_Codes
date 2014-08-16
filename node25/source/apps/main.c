#include <hal_led.h>
#include <hal_assert.h>
#include <hal_board.h>
#include <hal_int.h>
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h"

#define RF_CHANNEL            24      // 2.4 GHz RF channel


#define PAN_ID                0x2008
#define NODE_NUMBER           24
#define NODE_ID               75
#define LOCAL_ADDR            0x2520 + NODE_ID

/******************************修改为加3******************************/
#define APP_PAYLOAD_LENGTH    NODE_NUMBER+3   
#define Recieve_Choose        74

/************************添加一个24*27的数组*******************************/
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
/************************看门狗初始化***************************/
void init_watchdag(void)
{
  WDCTL=0X00;//时间间隔为1秒
  WDCTL |=0X08;//看门狗模式
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
    
    halLedSet(2);                //灯闪烁,如果复位了，能看到
    halMcuWaitMs(100);
    halLedClear(2);
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
         halLedSet(2);          
         if(length==APP_PAYLOAD_LENGTH)
         {          
            if(pRxData[APP_PAYLOAD_LENGTH-1]==Recieve_Choose)
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
              count=0;
              for(uint8 i=0;i!=NODE_NUMBER;i++)
              {
                halUartWrite(Data[i],26);
              }
           }

         clearBuff();
       }   
       halLedClear(2);
    }    
}


