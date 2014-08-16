#include <hal_led.h>
#include <hal_assert.h>
#include <hal_board.h>
#include <hal_int.h>
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"


#define NODE_ID               1
#define RF_CHANNEL            24      // 2.4 GHz RF channel

#define PAN_ID                0x2008
#define NODE_NUMBER           24
#define LOCAL_ADDR            0x2520 + NODE_ID
#define ALL_ADDR              0xFFFF
#define PAYLOAD_LENGTH        NODE_NUMBER+2

#define END_ID                24
//#define RESET                 850

/****************************************************************/
#define RESET_FLAG  END_ID
uint8 Begin=0;

uint8 Return_Flag=0;
uint8 Count_Text_T1=0;
uint8 Count_Restart_T1=0;
//uint16 resetTime;
uint8 TxData[PAYLOAD_LENGTH];
uint8 RxData[PAYLOAD_LENGTH];
uint8 Order[2]={0,0};

uint8 Last_R_ID=0;
uint8 Now_R_ID=0;

void TxpacketClear(void)
{
    int i;
    
    TxData[0] = NODE_ID;
    for(i = 1; i<PAYLOAD_LENGTH-1; i++)
    {
        TxData[i] = 0;
    }
    TxData[PAYLOAD_LENGTH-1] =0xFF;
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
    Last_R_ID=Now_R_ID;
    Now_R_ID=*pRxData;
//    basicRfReceive(pRxData, len, pRssi);   
    if(*pRxData>0&&(KK==PAYLOAD_LENGTH)&&Last_R_ID!=Now_R_ID)
    {
      rssi = basicRfGetRssi();
      halLedSet(2); 
      n=*pRxData;
      TxData[n] = rssi;
/*****************蛇，程序***********************/            
        if(RESET_FLAG==n)    //当接到END号节点的信号时
        {
          Begin=1;
        }
        
        if(NODE_ID==(*pRxData-1)) //如果接受到返回的信号
        {
          halLedSet(2);                //灯闪烁,如果复位了，能看到
          Return_Flag=1;   
        }


/************************************************/        
      halLedClear(2);
    }
    //当长度为2时  为控制指令 控制1号节点进行工作
    else if(2==KK&&0==pRxData[0]&&0==pRxData[1])
    {
    Begin=1;
    }
    RxpacketClear();
}

//定时器1初始化函数    
void Time1_intial(void) 
{
  T1CTL = 0x0C;     //128分频、停止运行
  T1CCTL0 |= 0x04;  //设定timer1通道0输出比较模式
  T1CC0L = 0xFF;    //计数1ms
  T1CC0H = 0xFF;    //设置为32000 进行计数时间为1ms
  TIMIF |= 0X40;    //设定timer1中断开启
  IEN1 |= 0x02;     //设定timer1中断开启
  //清除中断标志
  IRCON &= ~0x02;
  T1STAT &= ~0x01;  //第5位计数溢出标志呢
  EA = 1;           //开启总中断 
}



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
    Time1_intial(); 
    
//分隔.......................................     
    if(basicRfInit(&basicRfConfig)==FAILED)
    {
        HAL_ASSERT(FALSE);
    }
    
    basicRfReceiveOn();

//分隔....................................... 
    Begin=1;        //令Begin=1，能进行第一次发送
//分隔............................................    
    while (TRUE)
    {
        if(basicRfPacketIsReady())
        {   
            packetReceive(RxData, PAYLOAD_LENGTH, rssi);
        }
        if(Begin!=0)        //收到最后一个节点的数据的时候
        {
            Begin=0;     //Begin清零    
            halLedSet(1);
            halMcuWaitMs(4);
            halLedClear(1);
            halMcuWaitMs(4);
            packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
            packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
            packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
            T1CTL |= 0x02;     //发送数据后运行定时器 
        }
        if(1==Return_Flag)  //说明接受到下一个节点的数据了
        {
            T1CTL &= ~0x02; //定时器停止运行 等下一次发数据再运行
            T1CNTL=0;       //计算器清零
            Return_Flag=0;  //标志位清零
            Count_Text_T1=0;
        }
    }   

}

#pragma vector = T1_VECTOR //进入定时器中断 说明没有接收到下一个节点的返回值
__interrupt void TIMER1_ISR(void)  
{ 

    Count_Text_T1++;
    if(Count_Text_T1%3==0)
    {
    Begin=1;        //每次进入中断  令Begin=1 重新发送
    T1CTL &= ~0x02; //定时器停止运行        
    IRCON &= ~0x02; //清除中断标志
    T1STAT &= ~0x01;//清除中断标志
    }
    if(Count_Text_T1>10)   //认为进入死循环
    {
    Count_Text_T1=0;
    T1CTL &= ~0x02; //定时器停止运行        
    IRCON &= ~0x02; //清除中断标志
    T1STAT &= ~0x01;//清除中断标志 
    }
}