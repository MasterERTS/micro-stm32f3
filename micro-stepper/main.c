#include "stm32f3xx.h"

void wait()
{
	volatile int i=0;
	for(i=0;i<200000;i++);
}

void setup() {	

	//Declare ports as output for stepper motor
	pinMode(GPIOA, 8, OUTPUT);
	pinMode(GPIOA, 7, OUTPUT);
	pinMode(GPIOA, 6, OUTPUT);
	pinMode(GPIOA, 5, OUTPUT);

	ADCInit();
	

	// LED 
	pinMode(GPIOB, 3, OUTPUT);

	//BUTTON
	pinMode(GPIOB, 1, INPUT_PULLUP);

	// does it count ++ or -- by default ?
	// Prescaler : divide the frequency of intern clock
	// Period : value for which the timer overflows
	/// for psc = 63, cnt = 1MHz, so to have a period of 0.1s, arr = 100000 ( not possible )
	// for psc = 64000, cnt = 1KHz, 0.1/0.001 = 100 = arr
	// 0.02/0.001 = 2000
	setCounter(64000 - 1, 100);
	
}


int pbSwitch() {
	static enum PBState state = OFF; // conserve l'état de state à chaque appel
	switch(state) {
		case(OFF):
			if (digitalRead(GPIOB, 1) == 1) state = PUSH;
			break;
		case(PUSH):
			state = ON;
			break;
		case(ON):
			if (digitalRead(GPIOB, 1) == 0) state = RELEASE;
			break;
		case(RELEASE):
			state = OFF;
			break;
	}

	return state;
}


void controlSpeed() {
	// MAX SPEED : ARR = 2, MIN SPEED : ARR = 100
	int new_arr = (int)(98 * ADCRead() / 4096 + 2);
	TIM6->ARR = new_arr;
}


void stepCCW() {
	static int i = 0;
	//volatile int odr;

	// GPIOA->ODR &= ~(0xF << 5);
	// GPIOA->ODR |= (seq[i] << 5);

	GPIOA->BSRR = (0xF << (16 + 5));
	GPIOA->BSRR |= (seq[i] << 5);

	i++;
	if (i >= sizeof(seq)) i = 0;
}


void stepCW() {
	// ClockWise
	static int i = sizeof(seq) - 1;

	GPIOA->BSRR = 0xF << (16 + 5);
	GPIOA->BSRR |= seq[i] << 5;

	i--;
	if (i < 0) i = sizeof(seq) - 1;


/* main function */
int main(void)
{
	setup();
	/* Infinite loop */
	while (1)
	{
		/* Add application code here */
		GPIOB->BSRR = 1<<3; 		//bit set
		wait();
		GPIOB->BSRR = 1<<(3+16); 	//bit reset
		wait();
	}
}

