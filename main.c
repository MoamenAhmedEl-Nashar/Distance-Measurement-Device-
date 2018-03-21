#include"stdint.h"
#include "C:/Keil_v5/Labware/inc/tm4c123gh6pm.h" // for definitions of addresses
#define SPEEDOFSOUND 340
void SystemInit(){}//function that atartup_TM4C123.s needs to import
	// global variables declaration
	volatile unsigned long counter_value;
	volatile unsigned long counts ;
	volatile unsigned long time_in_ns ;
	float Distance;
	////-------------------------------------------------------------------//////////
void PORTF_Init()
{
		SYSCTL_RCGCGPIO_R=0x20;//enable clock for port F
	  while((SYSCTL_PRGPIO_R &0x20)==0x00){};//waiting untill ensuring enableing clock 
		// you can use dummy instuction also instead of this while loop
		GPIO_PORTF_LOCK_R=0x4C4F434B;
		GPIO_PORTF_CR_R=0x1F;
		GPIO_PORTF_AFSEL_R=0;
		GPIO_PORTF_PCTL_R=0x00;//sw0 is time pin
		GPIO_PORTF_AMSEL_R=0;
		GPIO_PORTF_DIR_R=0x0E;// 0 for input  1 for output
		GPIO_PORTF_DEN_R=0x1F;
		GPIO_PORTF_PUR_R=0x11;//sw0 ,sw1
}
void sysTick_init()
{
	NVIC_ST_CTRL_R=0; // disable the counter
	NVIC_ST_RELOAD_R=(0x0FFFFFF);
	NVIC_ST_CURRENT_R=0;// clear the current register and the count flag
	NVIC_ST_CTRL_R=5;//enable the counter and configure control register
}

void measure_distance()
{
	/*----initialize counter but don't enable it-------*/
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R |= 0;// not enabled
	NVIC_ST_RELOAD_R = 0x00FFFFFF;//max reload value
	
	/*-----wait untill echo goes to 1 ---------------*/
	while((GPIO_PORTF_DATA_R&0x01)==0){} //sw on 
	while((GPIO_PORTF_DATA_R&0x01)==1){} // sw off
		//sw on
	/*-----enable counter untill pin goes to 1 ----*/
	NVIC_ST_CTRL_R |= 5;//enabled
	while((GPIO_PORTF_DATA_R&0x01)==0){}//wait
		//sw off
	counter_value = NVIC_ST_CURRENT_R;
	NVIC_ST_CTRL_R |= 0;//disabled
		
	/*---------------get real time---------------------*/
	 counts = 16777215 - counter_value;
	 //time_in_ns = counts*62.5*0.000000001    /*12.5*0.000000001*/;
	 Distance = (counts*62.5*0.000000001    /*12.5*0.000000001*/ * SPEEDOFSOUND*100) / 2.0;
	
}
void delay_seconds(uint32_t time)//the function assumes that clock is 80 MHz
{
	
	uint32_t i;
	for(i=0;i<(time);i++) //200 ms*10=1 sec
	{
	sysTick_init();
	while((NVIC_ST_CTRL_R&0x10000)==0){}
	}
}

int main() // main function that contains the while loop
{
	
		PORTF_Init();
		//delay_seconds(1);
	  
	while(1)
	{
		/*if((NVIC_ST_CTRL_R&0x10000))//if count flag is high
		{
			GPIO_PORTF_DATA_R|=0x02;//red led
					
		}*/
		measure_distance();
	}
	
}
