#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "lcd.h"


void Timer0_PWM_Init(uint32 set_duty_cycle){

	TCNT0 = 0; // Timer initial value

	OCR0  = set_duty_cycle; //compare value

	DDRB  = DDRB | (1<<PB3); //set OC0 as output pin --> pin where the PWM signal is generated from MC

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}

int main(void)
{
	uint32 duty;
	uint16 res_value;
	LCD_init(); /* initialize LCD driver */
	ADC_init(); /* initialize ADC driver */


	while(1)
	{
		LCD_clearScreen(); /* clear LCD at the beginning */
		LCD_displayString("ADC Value = ");
		LCD_goToRowColumn(1,0);
		LCD_displayString("Duty Value= ");
		LCD_goToRowColumn(0,12); /* display the number every time at this position */
		res_value = ADC_readChannel(0); /* read channel zero where the potentiometer is connect */

		duty = ((uint32)res_value) * 255 / 1023;

		LCD_intgerToString(res_value); /* display the ADC value on LCD screen */
		LCD_goToRowColumn(1,12);
		LCD_intgerToString(duty);

		Timer0_PWM_Init(duty);
		_delay_ms(1000);
	}
}
