#include "stdint.h"
#include "C:/Keil/EE319Kware/inc/tm4c123gh6pm.h"
#define FREQ 80000000
#define SPEEDOFSOUND 343
void  SystemInit(){}
volatile unsigned long counterValue;
volatile unsigned long counts ;
volatile unsigned long Distance_cm ;
volatile unsigned long Distance ;
float timeEcho;


void init(void){
	
	//////////A/////////////
 SYSCTL_RCGCGPIO_R |= 0x01;              // Enable Clk for Port A
 while((SYSCTL_PRGPIO_R &0x01)==0x00){}; // Wait until clk is enabled
 GPIO_PORTA_LOCK_R  =  0x4C4F434B;       // Unlock GPIO Port A
 GPIO_PORTA_CR_R = 0xFF;                 // Allow changes to (PA0->PA7)
 GPIO_PORTA_AMSEL_R  =  0;               // Disable analog on Port A
 GPIO_PORTA_AFSEL_R = 0;                 // Disable alternating function    
 GPIO_PORTA_PCTL_R = 0;                  // PCTL GPIO on PA (clearing this and AFSEL ensures that PortA is working GPIO)
 GPIO_PORTA_DIR_R = 0x0F;                // Connect Decoder of 100s to pins 0-3 (Output)       
 GPIO_PORTA_DEN_R = 0x0F;          			 // Enable Digital on (PA0->PA3)
	 
	/////////B//////////////
 SYSCTL_RCGCGPIO_R |= 0x02;              // Enable Clk for Port B
 while((SYSCTL_PRGPIO_R &0x02)==0x00){}; // Wait until clk is enabled
 GPIO_PORTB_LOCK_R   = 0x4C4F434B;       // Unlock GPIO Port B
 GPIO_PORTB_CR_R = 0xFF;                 // Allow changes to (PB0->PB7)
 GPIO_PORTB_AMSEL_R   = 0;               // Disable analog on Port B
 GPIO_PORTB_AFSEL_R = 0;                 // Disable alternating function
 GPIO_PORTB_PCTL_R = 0;                  // PCTL GPIO on PB (clearing this and AFSEL ensures that PortB is working GPIO)
 GPIO_PORTB_DIR_R = 0x01;                // Connect trigger to pin 0 (Output), echo to pin 1 (Input)       
 GPIO_PORTB_DEN_R = 0x03;                // Enable digital I/O on (PB0->PB3)
	 
	//////////////D//////////////
 SYSCTL_RCGCGPIO_R |= 0x08;              // Enable Clk for Port D
 while((SYSCTL_PRGPIO_R &0x08)==0x00){}; // Wait until clk is enabled
 GPIO_PORTD_LOCK_R   = 0x4C4F434B;       // Unlock GPIO Port D
 GPIO_PORTD_CR_R = 0xFF;                 // Allow changes to (PD0->PD7)
 GPIO_PORTD_AMSEL_R   = 0;               // Disable analog on Port D
 GPIO_PORTD_AFSEL_R = 0;                 // Disable alternating function
 GPIO_PORTD_PCTL_R = 0;                  // PCTL GPIO on PD (clearing this and AFSEL ensures that PortD is working GPIO)
 GPIO_PORTD_DIR_R = 0x0F;                // Connect Decoder of 10s to pins 0-3 (Output)
 GPIO_PORTD_DEN_R = 0x0F;                // Enable digital on (PD0->PD3)
	 
	//////////////E//////////////
 SYSCTL_RCGCGPIO_R |= 0x10;              // Enable Clk for Port E
 while((SYSCTL_PRGPIO_R &0x10)==0x00){}; // Wait until clk is enabled
 GPIO_PORTE_LOCK_R   = 0x4C4F434B;       // Unlock GPIO Port E
 GPIO_PORTE_CR_R = 0xFF;                 // Allow changes to (PE0->PE7)
 GPIO_PORTE_AMSEL_R   = 0;               // Disable analog on Port E
 GPIO_PORTE_AFSEL_R = 0;                 // Disable alternating function
 GPIO_PORTE_PCTL_R = 0;                  // PCTL GPIO on PE (clearing this and AFSEL ensures that PortE is working GPIO)
 GPIO_PORTE_DIR_R = 0x0F;                // Connect Decoder of units to pins 0-3 (Output)      
 GPIO_PORTE_DEN_R = 0x0F;                // Enable digital on (PE0->PE3)
	///////////F///////////////////

 SYSCTL_RCGCGPIO_R=0x20;                  // enable clock for port F
 while((SYSCTL_PRGPIO_R &0x20)==0x00){};  // waiting untill ensuring enableing clock 
 GPIO_PORTF_LOCK_R=0x4C4F434B;
 GPIO_PORTF_CR_R=0x1F;
 GPIO_PORTF_AFSEL_R=0;
 GPIO_PORTF_PCTL_R=0x00;                  // sw0 is trig pin
 GPIO_PORTF_AMSEL_R=0;
 GPIO_PORTF_DIR_R=0x0E;                   // 0 for input  1 for output
 GPIO_PORTF_DEN_R=0x1F;
 GPIO_PORTF_PUR_R=0x11;                   // sw0 ,sw1	 
}
void setDisplay(uint32_t distance)
{
	GPIO_PORTA_DATA_R = distance/100;      // 100s
	GPIO_PORTD_DATA_R = (distance%100)/10; // 10s
	GPIO_PORTE_DATA_R = (distance%100)%10; // 1s
}

void sysTick_init(void)
{
	NVIC_ST_CTRL_R=0;                      // Disable the counter
	NVIC_ST_RELOAD_R=(0x00FFFFFF);         // Putting max. value in reload register
	NVIC_ST_CURRENT_R=0;                   // Clear the current register and the count flag
	NVIC_ST_CTRL_R=5;                      // Enable the counter and configure control register
}
void sysTick_delay(uint32_t delay)
								
{
	NVIC_ST_RELOAD_R = delay-1;	       // Putting delay value in reload register (800 for 10 us)  (10ms/(1/freq(80Mhz)))
	NVIC_ST_CURRENT_R=0;                   // Clear the current register and the count flag
	while((NVIC_ST_CTRL_R&0x10000)==0){}   // wait until count flag = 1
}
void SendTrigger(void)
{
	GPIO_PORTB_DATA_R = 0x00;              // TrigPin = 0
	sysTick_delay((10/1000000)*FREQ);      // Wait 10us 
	GPIO_PORTB_DATA_R = 0x01;              // TrigPin = 1
	sysTick_delay((10/1000000)*FREQ);      // wait 10us
	GPIO_PORTB_DATA_R = 0x00;              // TrigPin = 0
	
}
uint32_t MeasureDistance(void)
{
	if(!(GPIO_PORTF_DATA_R & 0x01))
	{
		SendTrigger();
		//while((GPIO_PORTB_DATA_R&0x02)==1){} // Wait while Echo = 1 
	  //while((GPIO_PORTB_DATA_R&0x02)==0){} // Wait while Echo = 0
																				   // Capture rising edge
			///////// For Test /////////////
		while((GPIO_PORTF_DATA_R&0x02)==0){} 	 //sw on 
	  while((GPIO_PORTF_DATA_R&0x02)==1){} 	 // sw off
																					 //sw on
	  /*-----Enable counter until pin goes to 1 ----*/
	  sysTick_init();                    		 // Enable Counter   
	  while((GPIO_PORTF_DATA_R&0x02)==0){}   // Wait
		                                       //sw off
		//while((GPIO_PORTB_DATA_R&0x02)==1){} // Wait while Echo = 1
	  counterValue = NVIC_ST_CURRENT_R;      // Store counter value
	  NVIC_ST_CTRL_R = 0;                    // Disable counter
		
	  /*---------------get real time---------------------*/
	  counts = 16777215 - counterValue;      
	  timeEcho = counts*(1/FREQ);             // FREQ is a Macro
	  Distance_cm = (timeEcho/2)*SPEEDOFSOUND*100;   // SPEEDOFSOUND is a Macro
		
	}
	return Distance_cm;	
}
int main(void)
	{
		init();
		sysTick_init();
		Distance = MeasureDistance();
		setDisplay(Distance);
		
  }
