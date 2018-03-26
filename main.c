#include "tm4c123gh6pm.h" //for the addresses
#include "stdint.h" //for int data types (like uint32_t)

#define ECHO (*((volatile uint32_t *)(0x400053FC + 0x0100)))
#define TRIGGER (*((volatile uint32_t *)(0x400053FC + 0x080)))
#define DECA (*((volatile uint32_t *)(0x400043FC + 0x04 + 0x08 + 0x010 + 0x020)))
#define DECD (*((volatile uint32_t *)(0x400073FC + 0x04 + 0x08 + 0x010 + 0x020)))
#define DECE (*((volatile uint32_t *)(0x400243FC + 0x04 + 0x08 + 0x010 + 0x020)))

#define SPEEDOFSOUND 340

void SystemInit(){} //so when startup code finishes it jumps here

void portBInit()
{
	SYSCTL_RCGCGPIO_R |= 0x02;
	while(!(SYSCTL_PRGPIO_R & 0x02));
	
	GPIO_PORTB_LOCK_R = 0x4C4F434B;
	GPIO_PORTB_CR_R |= 0x6F;
	
	GPIO_PORTB_DEN_R = 0x6F;
	GPIO_PORTB_AMSEL_R = 0;
	GPIO_PORTB_AFSEL_R = 0;
	GPIO_PORTB_PCTL_R = 0;
	GPIO_PORTB_DIR_R = 0x2F; //echo = 6 , trigger = 5 , 0->3 outputs if needed
}

void portAInit()
{
	SYSCTL_RCGCGPIO_R |= 0x01;
	while(!(SYSCTL_PRGPIO_R & 0x01));
	
	GPIO_PORTA_LOCK_R = 0x4C4F434B;
	GPIO_PORTA_CR_R |= 0x0F;
	
	GPIO_PORTA_DEN_R = 0x0F;
	GPIO_PORTA_AMSEL_R = 0;
	GPIO_PORTA_AFSEL_R = 0;
	GPIO_PORTA_PCTL_R = 0;
	GPIO_PORTA_DIR_R = 0x0F; //0->3 outputs (for decoder)
}

void portDInit()
{
	//PD5 is not on the board
	SYSCTL_RCGCGPIO_R |= 0x08;
	while(!(SYSCTL_PRGPIO_R & 0x08));
	
	GPIO_PORTD_LOCK_R = 0x4C4F434B;
	GPIO_PORTD_CR_R |= 0x0F;
	
	GPIO_PORTD_DEN_R = 0x0F;
	GPIO_PORTD_AMSEL_R = 0;
	GPIO_PORTD_AFSEL_R = 0;
	GPIO_PORTD_PCTL_R = 0;
	GPIO_PORTD_DIR_R = 0x0F; //0->3 outputs (for decoder)
}

void portEInit()
{
	//PE0 is P30 on the board
	SYSCTL_RCGCGPIO_R |= 0x10;
	while(!(SYSCTL_PRGPIO_R & 0x10));
	
	GPIO_PORTE_LOCK_R = 0x4C4F434B;
	GPIO_PORTE_CR_R |= 0x0F;
	
	GPIO_PORTE_DEN_R = 0x0F;
	GPIO_PORTE_AMSEL_R = 0;
	GPIO_PORTE_AFSEL_R = 0;
	GPIO_PORTE_PCTL_R = 0;
	GPIO_PORTE_DIR_R = 0x0F; //0->3 outputs (for decoder)
}

void systickInit()
{
	NVIC_ST_CTRL_R = 0; // Disable the systick timer
	NVIC_ST_CURRENT_R = 0; // Makes sure that current value is empty
	NVIC_ST_RELOAD_R = 8000; // value to be reloaded
	NVIC_ST_CTRL_R = 0x05; // Enable the systick timer
}

void init()
{
	portAInit();
	portBInit();
	portDInit();
	portEInit();
	//systickInit();
}

//the function assumes that clock is 80 MHz
//delay by multiples of 1 ms for minimum distance of 3.43 cm
void delay(uint32_t time)
{
	
	uint32_t i;
	for(i=0;i<(time);i++) //
	{
	systickInit();
	while((NVIC_ST_CTRL_R&0x10000)==0){}
	}
}

//returns distance in cm
double measure_distance() 
{
	uint32_t counterValue;
	uint32_t counts;
	/*----initialize counter but don't enable it-------*/
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R |= 0;// not enabled
	NVIC_ST_RELOAD_R = 0x00FFFFFF;//max reload value
	/*-----trigger the ultra sonic ---------------*/
	TRIGGER = 1;
	/*-----wait untill echo goes to 1 ---------------*/
	while(!ECHO);
	/*-----enable counter untill echo goes to 0 ----*/
	NVIC_ST_CTRL_R |= 5;//enabled
	while(ECHO);//wait
	counterValue = NVIC_ST_CURRENT_R - 3; // for accuracy for the execution of 3 instructions
	NVIC_ST_CTRL_R |= 0;//disabled
		
	/*---------------get real time---------------------*/
	 counts = 16777215 - counterValue;
	 //time = counts*62.5*0.000000001    /*12.5*0.000000001*/;
	 return (counts*12.5*0.000000001    /*12.5*0.000000001*/ * SPEEDOFSOUND * 100) / 2.0;
}

void setDisplay(uint32_t distance)
{
	DECA = distance/100; //100s
	DECD = (distance%100)/100; //10s
	DECE = (distance%100)%10; //1s
}
int main()
{
	uint32_t dist;
	init();
	while(1)
	{
		setDisplay(measure_distance());
		delay(10);
	}
}
