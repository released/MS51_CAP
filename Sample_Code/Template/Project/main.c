/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2019 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************/
/* Website: http://www.nuvoton.com                                                                         */
/*  E-Mail : MicroC-8bit@nuvoton.com                                                                       */
/*  Date   : Jan/21/2019                                                                                   */
/***********************************************************************************************************/

/************************************************************************************************************/
/*  File Function: MS51 DEMO project                                                                        */
/************************************************************************************************************/

#include "MS51_16K.h"


/*_____ M A C R O S ________________________________________________________*/

#define ABS(X)  									((X) > 0 ? (X) : -(X)) 

#define MAKE12BIT(v1,v2)         				((((uint16_t)(v1))<<4)+(uint16_t)(v2))      //v1,v2 is uint8_t

#define _debug_log_CAPTURE_						(1)

/*_____ D E F I N I T I O N S ______________________________________________*/
#define SYS_CLOCK 								(24000000ul)

#define PWM_FREQ 								(16000ul)

#define DUTY_RESOLUTION							(10000ul)
#define DUTY_MAX								(DUTY_RESOLUTION)
#define DUTY_MIN								(1ul)

/*_____ D E C L A R A T I O N S ____________________________________________*/
volatile uint8_t u8TH0_Tmp = 0;
volatile uint8_t u8TL0_Tmp = 0;

//UART 0
bit BIT_TMP;
bit BIT_UART;
bit uart0_receive_flag=0;
unsigned char uart0_receive_data;


typedef enum{
	flag_START = 0 ,


	flag_200us ,
	flag_1ms ,
	flag_5ms ,
	flag_10ms ,	
	flag_DEFAULT	
}Flag_Index;

volatile uint32_t BitFlag = 0;
#define BitFlag_ON(flag)							(BitFlag|=flag)
#define BitFlag_OFF(flag)							(BitFlag&=~flag)
#define BitFlag_READ(flag)							((BitFlag&flag)?1:0)
#define ReadBit(bit)								(uint32_t)(1<<bit)

#define is_flag_set(idx)							(BitFlag_READ(ReadBit(idx)))
#define set_flag(idx,en)							( (en == 1) ? (BitFlag_ON(ReadBit(idx))) : (BitFlag_OFF(ReadBit(idx))))



/*_____ F U N C T I O N S __________________________________________________*/

void send_UARTString(uint8_t* Data)
{
	#if 1
	uint16_t i = 0;

	while (Data[i] != '\0')
	{
		#if 1
		SBUF = Data[i++];
		#else
		UART_Send_Data(UART0,Data[i++]);		
		#endif
	}

	#endif

	#if 0
	uint16_t i = 0;
	
	for(i = 0;i< (strlen(Data)) ;i++ )
	{
		UART_Send_Data(UART0,Data[i]);
	}
	#endif

	#if 0
    while(*Data)  
    {  
        UART_Send_Data(UART0, (unsigned char) *Data++);  
    } 
	#endif
}

void send_UARTASCII(uint16_t Temp)
{
    uint8_t print_buf[16];
    uint16_t i = 15, j;

    *(print_buf + i) = '\0';
    j = (uint16_t)Temp >> 31;
    if(j)
        (uint16_t) Temp = ~(uint16_t)Temp + 1;
    do
    {
        i--;
        *(print_buf + i) = '0' + (uint16_t)Temp % 10;
        (uint16_t)Temp = (uint16_t)Temp / 10;
    }
    while((uint16_t)Temp != 0);
    if(j)
    {
        i--;
        *(print_buf + i) = '-';
    }
    send_UARTString(print_buf + i);
}


void capture_application (void)
{
	uint32_t duty = 0;
	uint16_t freq = 0;	
	uint16_t width0 = 0;
	uint16_t width1 = 0;
	uint16_t widthTotal = 0;	
	
	if((CAPCON0 & SET_BIT0) != 0)
	{
		width0 = C0H;
		width0 <<= 8;
		width0 |= C0L;
	}

	if((CAPCON0 & SET_BIT1) != 0)
	{
		width1 = C1H;
		width1 <<= 8;
		width1 |= C1L;	
	}

    clr_CAPCON0_CAPF0; 	
    clr_CAPCON0_CAPF1; 	
    clr_T2CON_TF2;

	widthTotal = width0 + width1;
	freq = (SYS_CLOCK)/(widthTotal);
	
//	duty = ((unsigned int)DUTY_RESOLUTION*(((float)width1) / widthTotal));  
	duty = ((unsigned int)DUTY_RESOLUTION*((float)width1 / widthTotal));
	
	#if (_debug_log_CAPTURE_ == 1)	// when width0 = 0 , calculate will fail
	if ((ABS((SYS_CLOCK/freq)-widthTotal)<5) && ((width0 != 0) && (width1 != 0) ) )
	{
		send_UARTString("capture_application :");	
		send_UARTString(",freq:");
		send_UARTASCII(freq);
		send_UARTString(",duty:");
		send_UARTASCII(duty);	
		send_UARTString(",H:");
		send_UARTASCII(width0);
		send_UARTString(",L:");
		send_UARTASCII(width1);	
		
		send_UARTString(",widthTotal:");
		send_UARTASCII(widthTotal);		
		send_UARTString(",temp:");
		send_UARTASCII(SYS_CLOCK/freq);	
		
		send_UARTString("\r\n");

	}


	#endif		
}


//P1.2 , PWM0_CH0 , IC0 
void CAP_Init(void)
{
    P12_QUASI_MODE;
    P12 = 1;

    TIMER2_CAP0_Capture_Mode;
	TIMER2_CAP1_Capture_Mode;

	IC0_P12_CAP0_RISINGEDGE_CAPTRUE;
	IC0_P12_CAP1_FALLINGEDGE_CAPTURE;

//    set_EIE_ECAP;	
    set_T2CON_TR2;

//    ENABLE_GLOBAL_INTERRUPT;	
}

//(P1.5 , PWM0_CH5) : SHAFT_DRIVER ,  motor PWM control output , 16k freq , duty 50 %
void PWM0_CHx_Init(uint16_t uFrequency,uint16_t d)	// ex : duty 83.5% , d = 8350 , duty 26% , d = 2600
{
	unsigned long res = 0;

	P15_PUSHPULL_MODE;			//Add this to enhance MOS output capability
    PWM5_P15_OUTPUT_ENABLE;	
 
    PWM_IMDEPENDENT_MODE;
    PWM_CLOCK_DIV_2;

/*
	PWM frequency   = Fpwm/((PWMPH,PWMPL)+1) = (24MHz/2)/(PWMPH,PWMPL)+1) = 20KHz
*/	
	res = (SYS_CLOCK>>1);
	res = res/uFrequency;		//value 375 for 16K
	res = res - 1;
	
    PWMPH = HIBYTE(res);
    PWMPL = LOBYTE(res);
	
//	res = d*(MAKEWORD(PWMPH,PWMPL)+1)/DUTY_RESOLUTION;	
	res = d*(res+1)/DUTY_RESOLUTION;

	set_SFRS_SFRPAGE;
    PWM5H = HIBYTE(res);
    PWM5L = LOBYTE(res);
    clr_SFRS_SFRPAGE;
	
    set_PWMCON0_LOAD;
    set_PWMCON0_PWMRUN;		

}

void GPIO_Init(void)
{
	P17_QUASI_MODE;		
	P30_PUSHPULL_MODE;	
}

void task_10ms(void)
{	

	if (is_flag_set(flag_10ms))
	{
		set_flag(flag_10ms,Disable);
		
		//application
		P30 = ~P30;

//		PWM0_CHx_Init(PWM_FREQ , 850);		//8.5 %
		PWM0_CHx_Init(PWM_FREQ , 7510);		//75.1 %		
		
		capture_application();
		
	
	}
}

void task_5ms(void)
{
	if (is_flag_set(flag_5ms))
	{
		set_flag(flag_5ms,Disable);

		//application
//		P17 = ~P17;


	
	}
}

void task_1ms(void)
{
	if (is_flag_set(flag_1ms))
	{
		set_flag(flag_1ms,Disable);

		//application
//		P17 = ~P17;

		
	
	}
}

void task_200us(void)
{
	if (is_flag_set(flag_200us))
	{
		set_flag(flag_200us,Disable);

		//application
//		P17 = ~P17;


		
	}
}

void task_loop(void)
{

	task_200us();
	task_1ms();
	task_5ms();
	task_10ms();


}

void Timer0_IRQHandler(void)
{
	/*
	  	200us base	

	  	1ms = 1000 / 200 = 5
	  	5ms = 5000 / 200 = 25
	  	10ms = 10000 / 200 = 50
	  	
	*/

	const uint16_t div_1ms = 5;	
	static uint16_t cnt_1ms = 0;

	const uint16_t div_5ms = 25;	
	static uint16_t cnt_5ms = 0;	
	
	const uint16_t div_10ms = 50;		//accurate 42 , with ADC conv.
	static uint16_t cnt_10ms = 0;

	set_flag(flag_200us,Enable);

	if (++cnt_1ms >= div_1ms)	
	{
		set_flag(flag_1ms,Enable);	
		cnt_1ms = 0;	
	}
	
	if (++cnt_5ms >= div_5ms)	
	{
		set_flag(flag_5ms,Enable);
		cnt_5ms = 0;	
	}

	if (++cnt_10ms >= div_10ms)
	{
		set_flag(flag_10ms,Enable);
		cnt_10ms = 0;		
	}
	
}

void Timer0_ISR(void) interrupt 1        // Vector @  0x0B
{
    _push_(SFRS);	
	
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;
    clr_TCON_TF0;
	
	Timer0_IRQHandler();

    _pop_(SFRS);	
}

void TIMER0_Init(void)
{
	uint16_t res = 0;

	/*
		formula : 16bit 
		(0xFFFF+1 - target)  / (24MHz/psc) = time base 

	*/	
	const uint16_t TIMER_DIVx_VALUE_200us_FOSC_240000 = 65536-400;

	ENABLE_TIMER0_MODE1;	// mode 0 : 13 bit , mode 1 : 16 bit
    TIMER0_FSYS_DIV12;
	
	u8TH0_Tmp = HIBYTE(TIMER_DIVx_VALUE_200us_FOSC_240000);
	u8TL0_Tmp = LOBYTE(TIMER_DIVx_VALUE_200us_FOSC_240000); 

    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;

    ENABLE_TIMER0_INTERRUPT;                       //enable Timer0 interrupt
    ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts
  
    set_TCON_TR0;                                  //Timer0 run
}


void Serial_ISR (void) interrupt 4 
{
    _push_(SFRS);

    if (RI)
    {   
      uart0_receive_flag = 1;
      uart0_receive_data = SBUF;
      clr_SCON_RI;                                         // Clear RI (Receive Interrupt).
    }
    if  (TI)
    {
      if(!BIT_UART)
      {
          TI = 0;
      }
    }

    _pop_(SFRS);	
}

void UART0_Init(void)
{
	#if 1
	const unsigned long u32Baudrate = 115200;
	P06_QUASI_MODE;    //Setting UART pin as Quasi mode for transmit
	
	SCON = 0x50;          //UART0 Mode1,REN=1,TI=1
	set_PCON_SMOD;        //UART0 Double Rate Enable
	T3CON &= 0xF8;        //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
	set_T3CON_BRCK;        //UART0 baud rate clock source = Timer3

	RH3    = HIBYTE(65536 - (SYS_CLOCK/16/u32Baudrate));  
	RL3    = LOBYTE(65536 - (SYS_CLOCK/16/u32Baudrate));  
	
	set_T3CON_TR3;         //Trigger Timer3
	set_IE_ES;

	ENABLE_GLOBAL_INTERRUPT;

	set_SCON_TI;
	BIT_UART=1;
	#else	
    UART_Open(SYS_CLOCK,UART0_Timer3,115200);
    ENABLE_UART0_PRINTF; 
	#endif
}


void MODIFY_HIRC_24(void)
{
	unsigned char u8HIRCSEL = HIRC_24;
    unsigned char data hircmap0,hircmap1;
//    unsigned int trimvalue16bit;
    /* Check if power on reset, modify HIRC */
    SFRS = 0 ;
	#if 1
    IAPAL = 0x38;
	#else
    switch (u8HIRCSEL)
    {
      case HIRC_24:
        IAPAL = 0x38;
      break;
      case HIRC_16:
        IAPAL = 0x30;
      break;
      case HIRC_166:
        IAPAL = 0x30;
      break;
    }
	#endif
	
    set_CHPCON_IAPEN;
    IAPAH = 0x00;
    IAPCN = READ_UID;
    set_IAPTRG_IAPGO;
    hircmap0 = IAPFD;
    IAPAL++;
    set_IAPTRG_IAPGO;
    hircmap1 = IAPFD;
    clr_CHPCON_IAPEN;

	#if 0
    switch (u8HIRCSEL)
    {
		case HIRC_166:
		trimvalue16bit = ((hircmap0 << 1) + (hircmap1 & 0x01));
		trimvalue16bit = trimvalue16bit - 15;
		hircmap1 = trimvalue16bit & 0x01;
		hircmap0 = trimvalue16bit >> 1;

		break;
		default: break;
    }
	#endif
	
    TA = 0xAA;
    TA = 0x55;
    RCTRIM0 = hircmap0;
    TA = 0xAA;
    TA = 0x55;
    RCTRIM1 = hircmap1;
    clr_CHPCON_IAPEN;
    PCON &= CLR_BIT4;
}


void SYS_Init(void)
{
    MODIFY_HIRC_24();

    ALL_GPIO_QUASI_MODE;
    ENABLE_GLOBAL_INTERRUPT;                // global enable bit	
}

void main (void) 
{
    SYS_Init();

    UART0_Init();
	GPIO_Init();					

	//TimerService  : 200us , 1ms , 5ms , 10ms
	TIMER0_Init();	// us base

	//(P1.2 , PWM0_CH0 , IC0) , input capture 
	CAP_Init();
		
    while(1)
    {
		task_loop();
		
    }
}



