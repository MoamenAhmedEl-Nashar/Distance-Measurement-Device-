# Distance-Measurement-Device-
This is an embedded system used to measure distance made with Tiva c 
#include "stdint.h"
#include "C:/Keil/EE319Kware/inc/tm4c123gh6pm.h"
void  SystemInit(){}

void init(void){
 SYSCTL_RCGCGPIO_R |= 0x1B;     // 1) Activate clock for Port A&B
	//////////A////////////////////
 GPIO_PORTA_LOCK_R   = 0x4C4F434B;   // 2) Unlock GPIO Port A
 GPIO_PORTA_CR_R = 0xFF;           // Allow changes to PA7-0==
	GPIO_PORTA_AMSEL_R   = 0;        // 3) disable analog on PortB
 GPIO_PORTA_PCTL_R = 0;   
 GPIO_PORTA_DIR_R = 0x0F;          // Connect Decoder of 100s to pins 0-3
 GPIO_PORTA_AFSEL_R = 0;        
 GPIO_PORTA_DEN_R = 0x0F;          
	///////////////B////////////////////
	GPIO_PORTB_LOCK_R   = 0x4C4F434B;   // 2) Unlock GPIO Port B
 GPIO_PORTB_CR_R = 0xFF;           // Allow changes to PB3-0, 5,5=6==
 GPIO_PORTB_AMSEL_R   = 0;        // 3) disable analog on PortB
 GPIO_PORTB_PCTL_R = 0;   // 4) PCTL GPIO on PB4-0(clearing this and AFSEL ensures that PB is working GPIO)
 GPIO_PORTB_DIR_R = 0x01;          // Connect trigger to pin 0, echo to pin 1
 GPIO_PORTB_AFSEL_R = 0;        
 GPIO_PORTB_DEN_R = 0x03;          // 7) enable digital I/O on PF4-0
	//////////////D//////////////
	GPIO_PORTD_LOCK_R   = 0x4C4F434B;   // 2) Unlock GPIO Port D
 GPIO_PORTD_CR_R = 0xFF;           // Allow changes to PD7-0==
	GPIO_PORTD_AMSEL_R   = 0;        // 3) disable analog on PortD
 GPIO_PORTD_PCTL_R = 0;   
 GPIO_PORTD_DIR_R = 0x0F;          // Connect Decoder of 10s to pins 0-3
 GPIO_PORTD_AFSEL_R = 0;        
 GPIO_PORTD_DEN_R = 0x0F;          
	//////////////E//////////////
	GPIO_PORTE_LOCK_R   = 0x4C4F434B;   // 2) Unlock GPIO Port D
 GPIO_PORTE_CR_R = 0xFF;           // Allow changes to PD7-0==
	GPIO_PORTE_AMSEL_R   = 0;        // 3) disable analog on PortD
 GPIO_PORTE_PCTL_R = 0;   
 GPIO_PORTE_DIR_R = 0x0F;          // Connect Decoder of units to pins 0-3
 GPIO_PORTE_AFSEL_R = 0;        
 GPIO_PORTE_DEN_R = 0x0F;          
	
}
void setDisplay(uint32_t distance)
{
	GPIO_PORTA_DATA_R = distance/100; //100s
	GPIO_PORTD_DATA_R = (distance%100)/100; //10s
	GPIO_PORTE_DATA_R = (distance%100)%10; //1s
}
int main(void)
	{
		init();
  }
