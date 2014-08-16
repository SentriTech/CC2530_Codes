#include <hal_led.h>
#include <hal_assert.h>
#include <hal_board.h>
#include <hal_int.h>
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"
//#include "hal_uart.h"



#define NODE_ID               56
#define PAN_ID                0x2008+(NODE_ID %3)*3
#define RF_CHANNEL            24-(NODE_ID-51)%3     // 2.4 GHz RF channel

#define NODE_NUMBER           24
#define LOCAL_ADDR            0x2520 + NODE_ID
#define ALL_ADDR              0xFFFF
#define PAYLOAD_LENGTH        NODE_NUMBER+2
//#define RESET_FLAG            NODE_ID-51
/*********************只发送给25号节点*********************/
#define TARGET_ADDR            0x2520 + 75

/**********************************************************/



uint8 Begin;
uint8 TxData[PAYLOAD_LENGTH];
uint8 RxData[PAYLOAD_LENGTH];

/************************添加一个24*27的数组*******************************/
uint8 Data[NODE_NUMBER][PAYLOAD_LENGTH+1];



/**************************************************************************/




void TxpacketClear(void)
{
    int i;
    
    TxData[0] = NODE_ID;
    for(i = 1; i<PAYLOAD_LENGTH-1; i++)
    {
        TxData[i] = 0;
    }
    TxData[PAYLOAD_LENGTH-1] =0x65;
}

void RxpacketClear(void)
{
    int j;
    for(j = 0; j<PAYLOAD_LENGTH; j++)
    {
        RxData[j] = 0;
    }
}

void packetSend(uint16 destAddr, uint8* pTxData, uint8 length)
{
    //halLedSet(1);
    basicRfReceiveOff();
    basicRfSendPacket(destAddr, pTxData, length);
    basicRfReceiveOn();
    //halLedClear(1);
    TxpacketClear();
}

void packetSend_24_27(uint16 destAddr, uint8* pTxData, uint8 length)
{
    //halLedSet(1);
    basicRfReceiveOff();
    basicRfSendPacket(destAddr, pTxData, length);
    basicRfReceiveOn();
    //halLedClear(1);

}


void packetReceive(uint8* pRxData, uint8 len, int16* pRssi)
{
    int8 rssi,n;
    int8 KK;
    KK=basicRfReceive(pRxData, len, pRssi);

    if((NODE_ID-*pRxData)%3==0&&(KK==PAYLOAD_LENGTH))  //在两个节点相差3的情况下接收
    {
      rssi = basicRfGetRssi();
      halLedSet(2);
      n=*pRxData-50;
      TxData[n] = rssi;
/*****************************************************************/         
      for(uint8 i=0;i!=PAYLOAD_LENGTH;i++)  //复制其他行数据
      {
        Data[n-1][i]=pRxData[i];
      }
      Data[NODE_ID-50-1][n]=rssi;           //复制本行,为本行添加数据
/*****************************************************************/         

      if(*pRxData==NODE_ID-3)  //相差3的情况下  对Begin进行置一
      {
        Begin = TRUE;
      }
      halLedClear(2);  
    }
    RxpacketClear();
}

void main(void)
{
    basicRfCfg_t basicRfConfig;
    int16* rssi;
    
    Begin = FALSE;
    TxpacketClear();
    RxpacketClear();
    
    basicRfConfig.panId = PAN_ID;
    basicRfConfig.channel = RF_CHANNEL;
    basicRfConfig.ackRequest = FALSE;
    basicRfConfig.myAddr = LOCAL_ADDR;

    halBoardInit();
    
/**************************************************************/    
    halLedSet(1);                //灯闪烁,如果复位了，能看到
    halMcuWaitMs(100);
    halLedClear(1);
    halMcuWaitMs(100);
    halLedSet(1);
    halMcuWaitMs(100);
    halLedClear(1);
    halMcuWaitMs(100);
 
/**************************************************************/  
    
/********************************************************************/
    for(uint8 i=0;i!=NODE_NUMBER;i++)     //将24行每行开头 标记节点
    {
      Data[i][PAYLOAD_LENGTH]=NODE_ID;    //将最后一个标记为NODE_ID
    }
    
    for(uint8 i=0;i!=NODE_NUMBER;i++)     //将24行每行开头 标记节点
    {
      Data[i][0]=i+51;    //将每行第一个标记为节点ID
    }       
/********************************************************************/  
    
    if(basicRfInit(&basicRfConfig)==FAILED)
    {
        HAL_ASSERT(FALSE);
    }
    
    basicRfReceiveOn();
    
    while (TRUE)
    {

        if(basicRfPacketIsReady())
        {   
            packetReceive(RxData, PAYLOAD_LENGTH, rssi);
        }
        
        
        if(Begin)
        { 
          halLedSet(1);
          halMcuWaitMs(8);
          halLedClear(1);
          halMcuWaitMs(8);    
          
/***************************修改为发送所有行数据**************************/        
            for(uint8 i=0;i!=NODE_NUMBER;i++)   //只发送组里的
            {
            packetSend_24_27(TARGET_ADDR, Data[i], PAYLOAD_LENGTH+1);
            }   
/***************************修改为发送所有行数据**************************/            
          packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
          
                    
          Begin=FALSE;
        }
    }    




}

