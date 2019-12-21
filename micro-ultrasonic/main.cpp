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

Adafruit_ST7735 tft = Adafruit_ST7735();

void setup()
{
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	tft.setFont(&FONT_NAME);
	tft.fillScreen(ST7735_BLACK);
	tft.setTextColor(ST7735_RED);
	tft.setCursor(10, FONT_HEIGHT);
	tft.print("Hello World! ");

	ADCInit();
}

//this is a simple application that uses the TFT display.
// => it first write a string in the top of the screen (in setup)
// => it writes the ADC value of the potentiometer in green, and
//    refreshes it each time the value differs more than 5.
int main()
{
	static int prevPot = -1;
	setup();
	while(1)
	{
		//potentiometer
		int pot = ADCRead(); //12 bits -> 4096 -> 4 digits
		//update only we value changes significantly
		if(abs(prevPot - pot) > 5)
		{
			//clear digits x,y,w,h,color
			tft.fillRect(tft.width()/2-2*FONT_WIDTH,
			             4*FONT_HEIGHT+1,4*FONT_WIDTH,
			             FONT_HEIGHT,0);
			tft.setCursor(tft.width()/2-2*FONT_WIDTH,5*FONT_HEIGHT);
			tft.setTextColor(ST7735_GREEN);
			tft.print(pot);
			prevPot = pot;
		}
	}
}

#ifdef __cplusplus
 }
#endif /* __cplusplus */
