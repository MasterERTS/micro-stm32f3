#include "stm32f3xx.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "adc.h"

//font definition.
#define PROGMEM
#define FONT_WIDTH 11
#define FONT_HEIGHT 21
#include "tft/Adafruit-GFX-Library/fontconvert/veraMono9.h"
#define FONT_NAME VeraMono9pt7b

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#include "pinAccess.h"

Adafruit_ST7735 tft = Adafruit_ST7735();
volatile int measure;

void configTIM6_IT() {
	//local validation
	TIM6->DIER |= TIM_DIER_UIE;
	//NVIC validation*
	NVIC_EnableIRQ(TIM6_DAC1_IRQn);
}

void configTIM6(uint16_t period) {
	// TIM6 Config
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
	TIM6->CNT = 0;
	TIM6->PSC = 64000-1; //tick@1ms
	TIM6->ARR = period; //counts 100 ticks
	TIM6->CR1 |= TIM_CR1_CEN;
	configTIM6_IT();
}

void configTIM7(uint16_t period) {
	// TIM7 Config
	period++;
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	asm("nop");
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM7RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM7RST;
	asm("nop");
	TIM7->CNT = 0;
	TIM7->PSC = 64-1; //tick@1µs
	// TIM7->ARR = period; //counts n ticks
	TIM7->CR1 |= TIM_CR1_CEN;
}

void configEXT() {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	asm("nop");
	EXTI->IMR |= EXTI_IMR_MR10;
	EXTI->FTSR |= EXTI_FTSR_TR10;
	EXTI->RTSR |= EXTI_FTSR_TR10;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PA;
	//NVIC_SetPriority(EXTI15_10_IRQn, 3); // priority 3 = ?
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void setup()
{
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	tft.setFont(&FONT_NAME);
	tft.fillScreen(ST7735_BLACK);
	tft.setTextColor(ST7735_RED);
	tft.setCursor(10, FONT_HEIGHT);
	tft.print("Lab 2 (Sensor)");

	// ADCInit();
	pinMode(GPIOB, 0, OUTPUT);
	pinMode(GPIOA, 10, OUTPUT);
	configTIM6(100-1);
	configTIM7(1);
	configEXT();
}

void TIM6_DAC1_IRQHandler() {
	TIM6->SR &= ~TIM_SR_UIF;

	EXTI->IMR &= ~EXTI_IMR_MR10;

	pinMode(GPIOA,10,OUTPUT);
	digitalWrite(GPIOA, 10, 1);
	for(volatile int i = 0 ; i < 20 ; i++);
	digitalWrite(GPIOA, 10, 0);
	pinMode(GPIOA,10,INPUT);

	//digitalToggle(GPIOB, 0);

	EXTI->IMR |= EXTI_IMR_MR10;
}

void EXTI15_10_IRQHandler() {
	EXTI->PR |= EXTI_PR_PR10;

	if (digitalRead(GPIOA, 10)) {
		TIM7->CNT = 0;
	} else {
		uint32_t counter_value = TIM7->CNT;
		measure = counter_value*10/58;
	}
}

int main()
{
	//static int prevPot = -1;
	setup();
	while(1)
	{
		//clear digits x,y,w,h,color
		tft.fillRect(tft.width()/2-2*FONT_WIDTH,
			             4*FONT_HEIGHT+1,4*FONT_WIDTH,
			             FONT_HEIGHT,0);
		tft.setCursor(tft.width()/2-2*FONT_WIDTH,5*FONT_HEIGHT);
		tft.setTextColor(ST7735_GREEN);
		tft.print(measure);
		tft.print(" mm");
	}
}

#ifdef __cplusplus
 }
#endif /* __cplusplus */