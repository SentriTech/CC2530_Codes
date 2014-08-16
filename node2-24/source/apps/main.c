#include <hal_led.h>
#include <hal_assert.h>
#include <hal_board.h>
#include <hal_int.h>
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"

//#include "hal_uart.h"

#define NODE_ID               24
#define RF_CHANNEL            24    


#define PAN_ID                0x2008
#define NODE_NUMBER           24
#define LOCAL_ADDR            0x2520 + NODE_ID
#define ALL_ADDR              0xFFFF
#define PAYLOAD_LENGTH        NODE_NUMBER+2
#define END_ID                24
/*********************只发送给25号节点*********************/
#define TARGET_ADDR            0x2520 + 25
/**********************************************************/

uint8 Return_Flag=0;
uint8 Count_Text_T1=0;

uint8 Begin;
uint8 TxData[PAYLOAD_LENGTH];
uint8 RxData[PAYLOAD_LENGTH];

uint8 Last_R_ID=0;
uint8 Now_R_ID=0;

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
    TxData[PAYLOAD_LENGTH-1] =0xFF;  //作为标记位 配合C语言程序使用
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
    //TxpacketClear();                //清零
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
    Last_R_ID=Now_R_ID;
    Now_R_ID=*pRxData;
    if(*pRxData>0&&(KK==PAYLOAD_LENGTH)&&Last_R_ID!=Now_R_ID)  //在两个节点相差3的情况下接收
    {
      halLedSet(2);
      rssi = basicRfGetRssi();
      n=*pRxData;
//TxData[n] = rssi;
/*****************************************************************/         
      for(uint8 i=0;i!=PAYLOAD_LENGTH;i++)  //复制其他行数据
      {
        Data[n-1][i]=pRxData[i];
      }
      Data[NODE_ID-1][n]=rssi;           //复制本行,为本行添加数据
/*****************************************************************/         

      if(NODE_ID-1==*pRxData)  //对Begin进行置一
      {
        Begin = TRUE;
      }
      if(NODE_ID==(*pRxData-1)||NODE_ID==*pRxData+(END_ID-1))//分两种情况 目前使用3
      {
          Return_Flag=1;     //如果接受到返回的信号
          TxpacketClear();   //当接受的返回的信号时才进行清零
      }
      TxData[n] = rssi;     //写入RSSI值
        
    }
    halLedClear(2);
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

  IRCON &= ~0x02;   //清除中断标志
  T1STAT &= ~0x01;  //第5位计数溢出标志呢
  EA = 1;           //开启总中断 
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
    
    Time1_intial();             //定时器初始化完毕 暂时不运行
    
//分隔....................................... 
    
    for(uint8 i=0;i!=NODE_NUMBER;i++)     //将24行每行开头 标记节点
    {
      Data[i][PAYLOAD_LENGTH]=NODE_ID;    //将最后一个标记为NODE_ID
    }
    
    for(uint8 i=0;i!=NODE_NUMBER;i++)     //将24行每行开头 标记节点
    {
      Data[i][0]=i+1;    //将每行第一个标记为节点ID
    }     
    for(uint8 i=0;i!=NODE_NUMBER;i++)  //将24行最后一个标记FF
    {
      Data[i][25]=0XFF;    //将24行最后一个标记FF
    }  
    
//分隔.......................................   
    
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
          halMcuWaitMs(2);
          halLedClear(1);
          halMcuWaitMs(2);              
//分隔............... 只在最后一个节点发送所有行数据   
            if(NODE_ID==END_ID)
            {
              for(uint8 i=0;i!=NODE_NUMBER;i++)   //只发送组里的
              {
              packetSend_24_27(TARGET_ADDR, Data[i], PAYLOAD_LENGTH+1);
              }  
            }
//分隔............... 修改为发送所有行数据            
          packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
          packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
          packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
          T1CTL |= 0x02;     //发送数据后运行定时器 
          Begin=FALSE;
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



//更改为查询中断方式可能会好一些

#pragma vector = T1_VECTOR //进入定时器中断 说明没有接收到下一个节点的返回值
__interrupt void TIMER1_ISR(void)  
{ 
    Count_Text_T1++;
    if(Count_Text_T1%3==0)
    {
    Begin=1;
    T1CTL &= ~0x02; //定时器停止运行           
    IRCON &= ~0x02; //清除中断标志
    T1STAT &= ~0x01; 
    }
    if(Count_Text_T1>9)//认为进入死循环
    {
    Count_Text_T1=0;
    T1CTL &= ~0x02; //定时器停止运行           
    IRCON &= ~0x02; //清除中断标志
    T1STAT &= ~0x01; 
    }
}
