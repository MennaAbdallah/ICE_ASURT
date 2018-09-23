#include "adc.h"
#include "lcd.h"


int main(void)
{
	uint16 res_value;
	LCD_init(); /* initialize LCD driver */
	ADC_init(); /* initialize ADC driver */
	LCD_clearScreen(); /* clear LCD at the beginning */
	/* display this string "ADC Value = " only once at LCD */
	LCD_displayString("ADC Value = ");
	//DDRA  = DDRA & (~(1<<PA0)); // configure pin 0 in PORTA as input pin
	DDRB |= 0x7F; // configure first seven pins of PORTD as output pins

	// initialize the 7-segment
	PORTB &= ~((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3)| (1<<PB4)| (1<<PB5)| (1<<PB5));
	PORTB |=(1<<PB6);

    while(1)
    {
    	LCD_goToRowColumn(0,12); /* display the number every time at this position */
    	res_value = ADC_readChannel(0); /* read channel zero where the potentiometer is connect */
    	LCD_intgerToString(res_value); /* display the ADC value on LCD screen */

		if((res_value > 0) && (res_value <= 200))
		{
			PORTB &=~ ((1<<PB1) |(1<<PB2));//1
			PORTB |=((1<<PB0)|(1<<PB3)|(1<<PB4)|(1<<PB5)|(1<<PB6));
		}
		else if((res_value > 200) && (res_value <= 400))
		{
			PORTB &=~ ((1<<PB0) |(1<<PB1)|(1<<PB6)|(1<<PB4)|(1<<PB3));//2
			PORTB |=((1<<PB2)|(1<<PB5));
		}
		else if((res_value > 400) && (res_value <= 600))
		{
			PORTB &=~ ((1<<PB0) |(1<<PB1)|(1<<PB6)|(1<<PB2)|(1<<PB3));//3
			PORTB |=((1<<PB4)|(1<<PB5));
		}
		else if((res_value > 600) && (res_value <= 800))
		{
			PORTB &=~ ((1<<PB1) |(1<<PB5)|(1<<PB6)|(1<<PB2));//4
			PORTB |=((1<<PB0)|(1<<PB3)|(1<<PB4));
		}
		else if((res_value > 800) && (res_value <= 1023))
		{
			PORTB &=~ ((1<<PB0) |(1<<PB5)|(1<<PB6)|(1<<PB2)|(1<<PB3));//5
			PORTB |=((1<<PB1)|(1<<PB4));
		}

    }
}
