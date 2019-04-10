
/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0Slv.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559�ṩSPI0�ӻ�ģʽ�����ӿں��� 
ע��Ƭѡ��Чʱ���ӻ����Զ�����SPI0_S_PRE��Ԥ��ֵ��������λ��������������ÿ�����Ƭѡ
��Чǰ��SPI0_S_PRE�Ĵ���д��Ԥ��ֵ�������������˶����׸������ֽڣ�����ʱע����������
ȡ��SPI0_S_PRE�����ֵ����һ��S0_IF_BYTE�жϡ�
���Ƭѡ����Ч����Ч���ӻ����Ƚ��з��͵Ļ�����ð���������ֽڷŵ�SPI0_S_PRE�Ĵ����У�
����Ѿ�����Ƭѡ��Ч�Ļ�����������ʹ��SPI0_DATA�Ϳ����ˡ�
*******************************************************************************/
#include "..\DEBUG.C"                                                          //������Ϣ��ӡ
#include "..\DEBUG.H"

#pragma  NOAREGS

#define SPI0Interrupt   0                                                      //�趨SPI0�����շ��жϷ�ʽ���߲�ѯ��ʽ
#define CH559SPI0SlvWrite(dat)  {SPI0_DATA = dat;while(S0_IF_BYTE == 0);S0_IF_BYTE = 0;}
#define CH559SPI0SlvRead(dat)    {while(S0_IF_BYTE == 0);dat = SPI0_DATA;S0_IF_BYTE = 0;}
UINT8 Num;
UINT8X buf[200];

/*Ӳ���ӿڶ���*/
/******************************************************************************
ʹ��CH559 Ӳ��SPI�ӿ� 
         CH559        DIR       
         P1.4        <==>       SCS
         P1.5        <==>       MOSI
         P1.6        <==>       MISO
         P1.7        <==>       SCK
*******************************************************************************/

/*******************************************************************************
* Function Name  : CH559SPI0InterruptInit()
* Description    : CH559SPI0�жϳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0InterruptInit()
{
    //IP_EX |= bIP_SPI0;                                                       //SPI0�ж����ȼ�����
    //SPI0_SETUP |= bS0_IE_FIRST;                                              //ʹ�����ֽڽ����ж�	
    //һ���������ֽ���������
    SPI0_SETUP |=  bS0_IE_BYTE;                                                //ʹ�ܽ���1�ֽ��жϣ�ʹ��FIFO����ж�
    SPI0_CTRL |= bS0_AUTO_IF;                                                  //�Զ���S0_IF_BYTE�жϱ�־
    SPI0_STAT |= 0xff;                                                         //���SPI0�жϱ�־
    IE_SPI0 = 1;                                                               //ʹ��SPI0�ж�
}

/*******************************************************************************
* Function Name  : CH559SPI0Init()
* Description    : CH559SPI0��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0Init(void)
{
    SPI0_S_PRE = 0x59;                                                        //Ԥ�����ݣ�����SPI�����������ֽ��������õ��Ľ�������
    PORT_CFG &= ~bP1_OC;                                                 
    P1_DIR &= 0x0F;
    P1_PU &=  0x0F;                                                           //SCS,MISO,MOSI,SCK ������Ϊ��������,Ƭѡ��ЧMISO�Զ�ʹ�����
    SPI0_SETUP |= bS0_MODE_SLV;                                               //����Ϊ�豸ģʽ��
    SPI0_SETUP &= ~	(bS0_BIT_ORDER);                                          //�ֽ�˳��Ϊ���ģʽ	
    SPI0_CTRL &= ~(bS0_MOSI_OE | bS0_SCK_OE);                                  //�ر�SCK��MOSI���
    SPI0_CTRL |=  bS0_MISO_OE;                                              
    SPI0_CTRL &= ~(bS0_MST_CLK | bS0_DATA_DIR | bS0_2_WIRE);                                
    SPI0_CTRL &= ~bS0_CLR_ALL;                                                //���SPI0��FIFO,Ĭ����1������������ܷ�������
}

#if SPI0Interrupt
/*******************************************************************************S
* Function Name  : SPI0HostInterrupt(void)
* Description    : SPI0 �ӻ�ģʽ�жϷ������
* Input          : None
* Output         : None
* Return         : UINT8 ret   
*******************************************************************************/
void	SPI0HostInterrupt( void ) interrupt INT_NO_SPI0 using 1                //* SPI0�жϷ������,ʹ�üĴ�����1
{
	  //if(S0_IF_FIRST)                                                        //������ֽ����������������Ӧ����
		//{}
    buf[Num] = SPI0_DATA;	
    S0_IF_BYTE = 0;                                                          // ���жϱ�־	
    Num++;
//  printf(" %02X\n",(UINT16)Num);
}
#endif

main( ) 
{
    UINT8 j;
    mDelaymS(30);                                                              //�ϵ���ʱ,�ȴ��ڲ������ȶ�
//  CfgFsys( );  
    
    P4_DIR |= bLED2;
    P3_DIR |= bTXD;
    mInitSTDIO( );                                                      //����0,�������ڵ���
    printf("start ...\n");  
    CH559SPI0Init();
    CH559SPI0InterruptInit(); 
#if SPI0Interrupt 
    EA  = 1;                                                            //ʹ��ȫ���ж�
    j = 0;
    while(1)                                                            //�жϷ�ʽ����
    {
	      if(Num == 10)
	      {
		        for(j = 0;j < 10;j++)
			      {
		            printf("  %02X",(UINT16)buf[j]);
			      }
	          Num = 0;
	      }
    } 
#endif 
    j = 0;
//     SPI0_S_PRE = 0;                                                    //�ӻ����ݷ���
//     while(1)                                                             
//     {
//         CH559SPI0SlvWrite(j);
//         j++;
//     }	
    while(1)                                                              //�ӻ���������
    {                                                                     //��Ϊ�д�ӡ������ע������ʱ������
        CH559SPI0SlvRead(j);
        j++;
        printf("%02X  ",(UINT16)j);
    }			
    while(1);
}