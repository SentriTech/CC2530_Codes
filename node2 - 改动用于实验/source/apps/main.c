#include <hal_led.h>
#include <hal_assert.h>
#include <hal_board.h>
#include <hal_int.h>
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"
//#include "hal_uart.h"

#define RF_CHANNEL                24      // 2.4 GHz RF channel

#define PAN_ID                0x2008
#define NODE_ID               52
#define NODE_NUMBER           24
#define LOCAL_ADDR            0x2520 + NODE_ID
#define ALL_ADDR              0xFFFF
#define PAYLOAD_LENGTH        NODE_NUMBER+2
#define RESET_FLAG            NODE_ID-51
/*********************ֻ���͸�25�Žڵ�*********************/
#define TARGET_ADDR            0x2520 + 75
#define Count_Number          10000
/**********************************************************/



uint8 Begin;
uint8 TxData[PAYLOAD_LENGTH];
uint8 RxData[PAYLOAD_LENGTH];

/************************���һ��24*27������*******************************/
uint8 Data[NODE_NUMBER][PAYLOAD_LENGTH+1];


uint16 Count_Receive=0;       //����
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

    if(*pRxData>50&&(KK==PAYLOAD_LENGTH))
    {
      rssi = basicRfGetRssi();
      halLedSet(2);
      n=*pRxData-50;
      TxData[n] = rssi;
/*****************************************************************/         
      for(uint8 i=0;i!=PAYLOAD_LENGTH;i++)  //��������������
      {
        Data[n-1][i]=pRxData[i];
      }
      Data[NODE_ID-50-1][n]=rssi;           //���Ʊ���,Ϊ�����������
/*****************************************************************/         

      if(n == RESET_FLAG)
      {
        Begin = TRUE;
      }
      halLedClear(2);  
      Count_Receive++;
    }
    RxpacketClear();
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
    halLedSet(2);
    halMcuWaitMs(100);
    halLedClear(2);
    halMcuWaitMs(100);
    init_watchdag();             //���п��Ź���ʼ��   
/**************************************************************/  
    
/********************************************************************/
    for(uint8 i=0;i!=NODE_NUMBER;i++)     //��24��ÿ�п�ͷ ��ǽڵ�
    {
      Data[i][PAYLOAD_LENGTH]=NODE_ID;    //�����һ�����ΪNODE_ID
    }
    
    for(uint8 i=0;i!=NODE_NUMBER;i++)     //��24��ÿ�п�ͷ ��ǽڵ�
    {
      Data[i][0]=i+51;    //��ÿ�е�һ�����Ϊ�ڵ�ID
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
          
/***************************�޸�Ϊ��������������**************************/        
            for(uint8 i=0;i!=NODE_NUMBER;i++)
            {
            packetSend_24_27(TARGET_ADDR, Data[i], PAYLOAD_LENGTH+1);
            }   
/***************************�޸�Ϊ��������������**************************/            
          packetSend(ALL_ADDR, TxData, PAYLOAD_LENGTH);
          
                    
          Begin=FALSE;
        }
/**************************************************************/ 
        feetdog();                   //ÿ��ѭ����ι��   
/**************************************************************/ 
    }    




}

