
/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0Mater.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559�ṩSPI0����ģʽ�����ӿں���             				   
*******************************************************************************/
#include "..\DEBUG.C"                                                          //������Ϣ��ӡ
#include "..\DEBUG.H"

#define SPI0Interrupt   0                                                      //�趨SPI0�����շ��жϷ�ʽ���߲�ѯ��ʽ
UINT8 flag;
UINT8 TmpBuf;

#pragma  NOAREGS
#define SET_SPI0_CK( d )   { SPI0_CK_SE = d; }                                 //d>=2

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
    SPI0_SETUP |= bS0_IE_FIFO_OV | bS0_IE_BYTE;                                //ʹ�ܽ���1�ֽ��жϣ�ʹ��FIFO����ж�
    SPI0_CTRL |= bS0_AUTO_IF;                                                  //�Զ���S0_IF_BYTE�жϱ�־
    SPI0_STAT |= 0xff;                                                         //���SPI0�жϱ�־
#if SPI0Interrupt
    IE_SPI0 = 1;                                                               //ʹ��SPI0�ж�
#endif
}


/*******************************************************************************
* Function Name  : CH559SPI0HostInit()
* Description    : CH559SPI0��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0HostInit(void)
{
    PORT_CFG &= ~bP1_OC;
    P1_DIR |= (bSCK | bMOSI | bSCS);
    P1_IE |= bMISO;                                                            //��������
	
    SPI0_SETUP &= ~(bS0_MODE_SLV | bS0_BIT_ORDER);                             //����Ϊ����ģʽ���ֽ�˳��Ϊ���ģʽ		
    SPI0_CTRL |=  bS0_MOSI_OE  | bS0_SCK_OE ;                                  //MISO���ʹ�ܣ�SCK���ʹ��
    SPI0_CTRL &= ~(bS0_MST_CLK | bS0_2_WIRE);
    SPI0_CTRL &=  ~(bS0_DATA_DIR);                                             //����д��Ĭ�ϲ�����д���䣬���ʹ��bS0_DATA_DIR��
	                                                                             //��ô�������ݺ��Զ�����һ���ֽڵ�ʱ�ӣ����ڿ��������շ�	
    SPI0_CK_SE = 0x40;
    SPI0_CTRL &= ~bS0_CLR_ALL;                                                 //���SPI0��FIFO,Ĭ����1������������ܷ�������
}

/*******************************************************************************
* Function Name  : CH559SPI0Write(UINT8 dat)
* Description    : CH559Ӳ��SPIд����
* Input          : UINT8 dat   ����
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0Write(UINT8 dat)
{
    SPI0_DATA = dat;                                                           
    while(S0_FREE == 0);													   //�ȴ��������		
//���bS0_DATA_DIRΪ1���˴�����ֱ�Ӷ�ȡһ���ֽڵ��������ڿ��ٶ�д	
}

/*******************************************************************************
* Function Name  : CH559SPI0Read( )
* Description    : CH559Ӳ��SPI0������
* Input          : None
* Output         : None
* Return         : UINT8 ret   
*******************************************************************************/
UINT8 CH559SPI0Read()
{
    SPI0_DATA = 0xff;
    while(S0_FREE == 0);
    return SPI0_DATA;
}

#if SPI0Interrupt
/*******************************************************************************
* Function Name  : SPI0HostInterrupt(void)
* Description    : SPI0 ����ģʽ�жϷ������
* Input          : None
* Output         : None
* Return         : UINT8 ret   
*******************************************************************************/
void	SPI0HostInterrupt( void ) interrupt INT_NO_SPI0 using 1                //SPI0�жϷ������,ʹ�üĴ�����1
{ 
    if(flag == 0)
    {
        while(S0_FREE == 0);
    }
    if(flag == 1)
    {
        while(S0_FREE == 0);
        TmpBuf = SPI0_DATA;
    }
    //printf("sta %02X",(UINT16)SPI0_STAT);
    //printf("fl %02X",(UINT16)flag);
    SPI0_STAT = 0xff; 
}
#endif

void main()
{
    UINT8 ret,i=0;
    mDelaymS(30);                                                              //�ϵ���ʱ,�ȴ��ڲ������ȶ�,�ؼ� 
//  CfgFsys( );     
    
    P4_DIR |= bLED2;
    P3_DIR |= bTXD;
    mInitSTDIO( );                                                             //����0,�������ڵ���
    printf("start ...\n");  
	
    CH559SPI0HostInit();                                                       //SPI0����ģʽ��ʼ��
#if SPI0Interrupt
    CH559SPI0InterruptInit();                                                  //SPI0�жϳ�ʼ��
    EA  = 1;                                                                   //ʹ��ȫ���ж�
#endif
    while(1)
    {   
	      SCS = 0; 
        CH559SPI0Write(0x06);                                                  //CH559��CH376ͨѶ
//		  flag = 0;
        mDelayuS(2);
        CH559SPI0Write(i);
        mDelayuS(2);
//		  flag = 1;
        TmpBuf = CH559SPI0Read();
        SCS = 1;
        if(TmpBuf != (i^0xff))
        {
            LED2 = !LED2;
		        printf("%02X  %02X  \n",(UINT16)i,(UINT16)TmpBuf);					
        }
		    mDelaymS(10);
				i = i+2;
    }
}