/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI1Master.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559 SPI1�����ӿڲ���CH376
*******************************************************************************/

/*Ӳ���ӿڶ���*/
/******************************************************************************
ʹ��CH559 Ӳ��SPI1����CH376
Ӳ���ӿ� 
         CH559        DIR       CH376
         P2.1        <==>       MOSI
         P2.2        <==>       MISO
         P2.3        <==>       SCK
         P1.4        <==>       SCS
*******************************************************************************/
#include "..\DEBUG.C"                                                          //������Ϣ��ӡ
#include "..\DEBUG.H"

#pragma  NOAREGS

UINT8  buffer,flag;   
#define SET_SPI1_CK( d )   { SPI1_CK_SE = d; }                                //d>=2

/*******************************************************************************
* Function Name  : CH559SPI1Init()
* Description    : CH559SPI1��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI1Init( )
{
	P1_DIR |= bSCS;
	P2_DIR |= bMOSI1 | bSCK1;                                                  //����SPI�ӿڵ����ŷ���
	SPI1_CTRL |= bS1_SCK_OE | bS1_AUTO_IF ;                                    //MISO���ʹ�ܣ�SCK���ʹ��
	SPI1_CTRL &= ~(bS1_DATA_DIR | bS1_2_WIRE);                                 //ʹ��3��SPI�������ݲ���������
	                                                                           //���ʹ��bS1_DATA_DIR����ô�������ݺ��Զ�����һ���ֽڵ�ʱ�ӣ����ڿ��������շ�
	SPI1_CK_SE = 0x20;                                                         //����SPI����ʱ�ӣ������Լ�����
	SPI1_CTRL &= ~bS1_CLR_ALL;                                                 //���SPI1��FIFO,Ĭ����1������������ܷ�������
}

/***************************************************************************** **
* Function Name  : CH559SPI1Write(UINT8 dat)
* Description    : SPI1�������
* Input          : UINT8 dat ����
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI1Write(UINT8 dat)
{
//  SCS = 0;	
    SPI1_DATA = dat;                                                           //�������
    while((SPI1_STAT & 0x08) == 0);	                                           //�ȴ��������	
//���bS1_DATA_DIRΪ1���˴�����ֱ�Ӷ�ȡһ���ֽڵ��������ڿ��ٶ�д
//   SCS = 1;																	
}

/*******************************************************************************
* Function Name  : CH559SPI1Read()
* Description    : SPI1������
* Input          : None
* Output         : None
* Return         : UINT8 ret   
*******************************************************************************/
UINT8 CH559SPI1Read( )
{
//  SCS = 0;
    SPI1_DATA = 0xff;                                                          //����ʱ��                                               
    while((SPI1_STAT & 0x08) == 0);                                            //�ȴ��������
//  SCS = 1;	
    return SPI1_DATA;
}

main( ) 
{
    UINT8 ret,i = 0;
//  CfgFsys( );                                                               //ʱ������    
    mDelaymS(5);                                                              //�ȴ��ڲ������ȶ�,�ؼ� 
    
    P4_DIR |= bLED2;
    P3_DIR |= bTXD;
    mInitSTDIO( );                                                            //����0,�������ڵ���
    printf("start ...\n");  
	
    CH559SPI1Init();                                                          //SPI1�ĳ�ʼ�� 
    while(1)
    { 
	     SCS = 0;
	     CH559SPI1Write(0x06);
	     mDelayuS(2);
	     CH559SPI1Write(i);
	     mDelayuS(2);
	     ret = CH559SPI1Read();
	     if(ret != (i^0xff))
	     {
	         SCK = ~SCK;
	         printf("%02X  %02X",(UINT16)i,(UINT16)ret);				
	     }
	     SCS = 1;
			 i++;
		   mDelaymS(20);   		
    }
}