#include <hal_led.h>
#include <hal_assert.h>
#include <hal_board.h>
#include <hal_int.h>
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"

#define RF_CHANNEL                24      // 2.4 GHz RF channel

#define PAN_ID                0x2008
#define NODE_ID               51
#define NODE_NUMBER           24
#define LOCAL_ADDR            0x2520 + NODE_ID
#define ALL_ADDR              0xFFFF
#define PAYLOAD_LENGTH        NODE_NUMBER+2

#define END_ID                74
//#define RESET                 850

/****************************************************************/
#define RESET_FLAG  END_ID-50
uint8 Begin=0;



//uint16 resetTime;
uint8 TxData[PAYLOAD_LENGTH];
uint8 RxData[PAYLOAD_LENGTH];

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

void packetReceive(uint8* pRxData, uint8 len, int16* pRssi)
{
    int8 rssi,n;
    int8 KK;
    KK=basicRfReceive(pRxData, len, pRssi);     //计算长度
//    basicRfReceive(pRxData, len, pRssi);   
    if(*pRxData>50&&(KK==PAYLOAD_LENGTH))
    {
      rssi = basicRfGetRssi();
      halLedSet(2); 
      n=*pRxData-50;
      TxData[n] = rssi;
/*****************蛇，程序***********************/            
        if(n==RESET_FLAG)    //当接到END号节点的信号时
        {
          Begin=1;
        }

/************************************************/        
      halLedClear(2);
    }
    RxpacketClear();
}
/************************看门狗初始化***************************/
void init_watchdag(void)
{
  WDCTL=0X00;//时间间隔为1秒
  WDCTL |=0X08;//看门狗模式
}

//清除看门狗定时器
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
    
//    resetTime = RESET;
    TxpacketClear();
    RxpacketClear();
    
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
    init_watchdag();             //进行看门狗初始化  
/**************************************************************/    
    if(basicRfInit(&basicRfConfig)==FAILED)
    {
        HAL_ASSERT(FALSE);
    }
    
    basicRfReceiveOn();

/***********************/
    Begin=5;        //令Begin=1，能进行第一次发送
/***********************/    
    
    while (TRUE)
    {
        if(basicRfPacketIsReady())
        {   
            packetReceive(RxData, PAYLOAD_LENGTH, rssi);
        }
        if(Begin!=0)        //收到最后一个节点的数据的时候
        {
            Begin=0;     //Begin清零    
            
            /***************************
            为什么Begin--会出现一号节点接受不到2号节点的数据的问题，而令Begin=0
            则可以解决这个问题？
            
            ***************************/
            
            
            halLedSet(1);
            halMcuWaitMs(30);
            halLedClear(1);
            halMcuWaitMs(30);
//            resetTime = 0;
            packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
        }
       // halMcuWaitUs(100);
//        resetTime++;

/**************************************************************/ 
        feetdog();                   //每次循环都喂狗   
/**************************************************************/ 
    }   
  
 
  
  
  
}

