#include <hal_led.h>
#include <hal_assert.h>
#include <hal_board.h>
#include <hal_int.h>
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"




#define NODE_ID               53
//#define RF_CHANNEL            22      // 2.4 GHz RF channel
//#define RF_CHANNEL            20      // 2.4 GHz RF channel
#define RF_CHANNEL            16      // 2.4 GHz RF channel




#define PAN_ID                0x2008+(NODE_ID%3)*3
#define NODE_NUMBER           24
#define LOCAL_ADDR            0x2520 + NODE_ID
#define ALL_ADDR              0xFFFF
#define PAYLOAD_LENGTH        NODE_NUMBER+2

#define END_ID                72+(NODE_ID-51)
//#define RESET                 850

/****************************************************************/
#define RESET_FLAG  END_ID-50
uint8 Begin=0;

uint8 Return_Flag=0;
uint8 Count_Text_T1=0;
uint8 Count_Restart_T1=0;
//uint16 resetTime;
uint8 TxData[PAYLOAD_LENGTH];
uint8 RxData[PAYLOAD_LENGTH];
uint8 Order[2]={0,0};

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
    KK=basicRfReceive(pRxData, len, pRssi);     //���㳤��
//    basicRfReceive(pRxData, len, pRssi);   
    if(*pRxData>50&&(KK==PAYLOAD_LENGTH))
    {
      rssi = basicRfGetRssi();
      halLedSet(2); 
      n=*pRxData-50;
      TxData[n] = rssi;
/*****************�ߣ�����***********************/            
        if(RESET_FLAG==n)    //���ӵ�END�Žڵ���ź�ʱ
        {
          Begin=1;
        }
        
        if((NODE_ID+3)==*pRxData) //������ܵ����ص��ź�
        {
          Return_Flag=1;     
        }


/************************************************/        
      halLedClear(2);
    }
    //������Ϊ2ʱ  Ϊ����ָ�� ����1�Žڵ���й���
    else if(2==KK&&0==pRxData[0]&&0==pRxData[1])
    {
    Begin=1;
    }
    RxpacketClear();
}

//��ʱ��1��ʼ������   
void Time1_intial(void) 
{

  T1CTL = 0x0C;     //128��Ƶ��ֹͣ����
  T1CCTL0 |= 0x04;  //�趨timer1ͨ��0����Ƚ�ģʽ
  T1CC0L = 0xFF;    //����1ms
  T1CC0H = 0xFF;    //����Ϊ32000 ���м���ʱ��Ϊ1ms
  TIMIF |= 0X40;    //�趨timer1�жϿ���
  IEN1 |= 0x02;     //�趨timer1�жϿ���
  //����жϱ�־
  IRCON &= ~0x02;
  T1STAT &= ~0x01;  //��5λ���������־��
  EA = 1;           //�������ж� 

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
    
    halLedSet(2);                //����˸,�����λ�ˣ��ܿ���
    halMcuWaitMs(100);
    halLedClear(2);
    halMcuWaitMs(100);
    halLedSet(2);
    halMcuWaitMs(100);
    halLedClear(2);
    halMcuWaitMs(100);
    Time1_intial(); 
    
//�ָ�.......................................     
    if(basicRfInit(&basicRfConfig)==FAILED)
    {
        HAL_ASSERT(FALSE);
    }
    
    basicRfReceiveOn();

//�ָ�....................................... 
    Begin=1;        //��Begin=1���ܽ��е�һ�η���
//�ָ�.......................................    
    
    while (TRUE)
    {
        if(basicRfPacketIsReady())
        {   
            packetReceive(RxData, PAYLOAD_LENGTH, rssi);
        }
        if(Begin!=0)        //�յ����һ���ڵ�����ݵ�ʱ��
        {
            Begin=0;     //Begin����    
            halLedSet(1);
            halMcuWaitMs(7);
            halLedClear(1);
            halMcuWaitMs(7);
            packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
            T1CTL |= 0x02;     //�������ݺ����ж�ʱ�� 
        }
        if(1==Return_Flag)  //˵�����ܵ���һ���ڵ��������
        {
            T1CTL &= ~0x02; //��ʱ��ֹͣ���� ����һ�η�����������
            T1CNTL=0;       //����������
            Return_Flag=0;  //��־λ����
            Count_Text_T1=0;
        }
    }   

}

#pragma vector = T1_VECTOR //���붨ʱ���ж� ˵��û�н��յ���һ���ڵ�ķ���ֵ
__interrupt void TIMER1_ISR(void)  
{ 

    Count_Text_T1++;
    if(Count_Text_T1%8==0)
    {
    Begin=1;        //ÿ�ν����ж�  ��Begin=1 ���·���
    T1CTL &= ~0x02; //��ʱ��ֹͣ����        
    IRCON &= ~0x02; //����жϱ�־
    T1STAT &= ~0x01; 
    }
    if(Count_Text_T1>33)   //��Ϊ������ѭ��
    {
    Count_Text_T1=0;
    T1CTL &= ~0x02; //��ʱ��ֹͣ����        
    IRCON &= ~0x02; //����жϱ�־
    T1STAT &= ~0x01; 
    }
}