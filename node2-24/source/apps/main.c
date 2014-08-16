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
/*********************ֻ���͸�25�Žڵ�*********************/
#define TARGET_ADDR            0x2520 + 25
/**********************************************************/

uint8 Return_Flag=0;
uint8 Count_Text_T1=0;

uint8 Begin;
uint8 TxData[PAYLOAD_LENGTH];
uint8 RxData[PAYLOAD_LENGTH];

uint8 Last_R_ID=0;
uint8 Now_R_ID=0;

/************************���һ��24*27������*******************************/
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
    TxData[PAYLOAD_LENGTH-1] =0xFF;  //��Ϊ���λ ���C���Գ���ʹ��
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
    //TxpacketClear();                //����
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
    if(*pRxData>0&&(KK==PAYLOAD_LENGTH)&&Last_R_ID!=Now_R_ID)  //�������ڵ����3������½���
    {
      halLedSet(2);
      rssi = basicRfGetRssi();
      n=*pRxData;
//TxData[n] = rssi;
/*****************************************************************/         
      for(uint8 i=0;i!=PAYLOAD_LENGTH;i++)  //��������������
      {
        Data[n-1][i]=pRxData[i];
      }
      Data[NODE_ID-1][n]=rssi;           //���Ʊ���,Ϊ�����������
/*****************************************************************/         

      if(NODE_ID-1==*pRxData)  //��Begin������һ
      {
        Begin = TRUE;
      }
      if(NODE_ID==(*pRxData-1)||NODE_ID==*pRxData+(END_ID-1))//��������� Ŀǰʹ��3
      {
          Return_Flag=1;     //������ܵ����ص��ź�
          TxpacketClear();   //�����ܵķ��ص��ź�ʱ�Ž�������
      }
      TxData[n] = rssi;     //д��RSSIֵ
        
    }
    halLedClear(2);
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

  IRCON &= ~0x02;   //����жϱ�־
  T1STAT &= ~0x01;  //��5λ���������־��
  EA = 1;           //�������ж� 
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
    halLedSet(1);                //����˸,�����λ�ˣ��ܿ���
    halMcuWaitMs(100);
    halLedClear(1);
    halMcuWaitMs(100);
    halLedSet(1);
    halMcuWaitMs(100);
    halLedClear(1);
    halMcuWaitMs(100);
    
    Time1_intial();             //��ʱ����ʼ����� ��ʱ������
    
//�ָ�....................................... 
    
    for(uint8 i=0;i!=NODE_NUMBER;i++)     //��24��ÿ�п�ͷ ��ǽڵ�
    {
      Data[i][PAYLOAD_LENGTH]=NODE_ID;    //�����һ�����ΪNODE_ID
    }
    
    for(uint8 i=0;i!=NODE_NUMBER;i++)     //��24��ÿ�п�ͷ ��ǽڵ�
    {
      Data[i][0]=i+1;    //��ÿ�е�һ�����Ϊ�ڵ�ID
    }     
    for(uint8 i=0;i!=NODE_NUMBER;i++)  //��24�����һ�����FF
    {
      Data[i][25]=0XFF;    //��24�����һ�����FF
    }  
    
//�ָ�.......................................   
    
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
//�ָ�............... ֻ�����һ���ڵ㷢������������   
            if(NODE_ID==END_ID)
            {
              for(uint8 i=0;i!=NODE_NUMBER;i++)   //ֻ���������
              {
              packetSend_24_27(TARGET_ADDR, Data[i], PAYLOAD_LENGTH+1);
              }  
            }
//�ָ�............... �޸�Ϊ��������������            
          packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
          packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
          packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
          T1CTL |= 0x02;     //�������ݺ����ж�ʱ�� 
          Begin=FALSE;
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



//����Ϊ��ѯ�жϷ�ʽ���ܻ��һЩ

#pragma vector = T1_VECTOR //���붨ʱ���ж� ˵��û�н��յ���һ���ڵ�ķ���ֵ
__interrupt void TIMER1_ISR(void)  
{ 
    Count_Text_T1++;
    if(Count_Text_T1%3==0)
    {
    Begin=1;
    T1CTL &= ~0x02; //��ʱ��ֹͣ����           
    IRCON &= ~0x02; //����жϱ�־
    T1STAT &= ~0x01; 
    }
    if(Count_Text_T1>9)//��Ϊ������ѭ��
    {
    Count_Text_T1=0;
    T1CTL &= ~0x02; //��ʱ��ֹͣ����           
    IRCON &= ~0x02; //����жϱ�־
    T1STAT &= ~0x01; 
    }
}
