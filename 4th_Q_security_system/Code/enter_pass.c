#include "lcd.h"
#include "keypad.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define NUMBER_OF_OVERFLOWS_PER_SECOND 4

unsigned char g_tick = 0;

ISR(TIMER0_OVF_vect)
{
	g_tick++;
	if(g_tick == NUMBER_OF_OVERFLOWS_PER_SECOND)
	{
		PORTD ^= (1<<PD7); //toggle led every 1 second
		buzzer_on();
		g_tick = 0; //clear the tick counter again to count a new 1 second
	}
}

/* Description :
 * For clock=1Mhz and prescale F_CPU/1024 every count will take 1ms
 * so put initial timer counter=0  0 --> 255 (256ms per overflow)
 * so we need timer to overflow 2 times to get a 0.5 second
 */
void timer0_init_normal_mode(void)
{
	TCNT0 = 0; //timer initial value
	TIMSK |= (1<<TOIE0); //enable overflow interrupt
	/* Configure the timer control register
	 * 1. Non PWM mode FOC0=1
	 * 2. Normal Mode WGM01=0 & WGM00=0
	 * 3. Normal Mode COM00=0 & COM01=0
	 * 4. clock = F_CPU/1024 CS00=1 CS01=0 CS02=1
	 */
	TCCR0 = (1<<FOC0) | (1<<CS02) | (1<<CS00);
}

/* External INT0 Interrupt Service Routine */
ISR(INT0_vect)
{
	if(BIT_IS_SET(PORTD,PD2))
	{
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_mode();
	}
}

/* External INT0 enable and configuration function */
void INT0_Init(void)
{
	SREG  &= ~(1<<7);                   // Disable interrupts by clearing I-bit
	DDRD  &= (~(1<<PD2));               // Configure INT0/PD2 as input pin
	GICR  |= (1<<INT0);                 // Enable external interrupt pin INT0
	MCUCR |= (1<<ISC00) | (1<<ISC01);   // Trigger INT0 with the raising edge
	SREG  |= (1<<7);                    // Enable interrupts by setting I-bit
}
//Function to initialize Buzzer
void buzzer_pin_config (void)
{
 DDRD |= (1<< PD5) ;		//Setting PORTC 3 as output
 PORTD &=~(1<<PD5);		//Setting PORTC 3 logic low to turnoff buzzer
}

//Function to Initialize PORTS
void port_init (void)
{
 buzzer_pin_config();
}

void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PIND;
 port_restore = port_restore | 0x20;
 PORTD = port_restore;
}

void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PIND;
 port_restore &=~(0x20);
 PORTD = port_restore;
}

void init_devices (void)
{
 cli();             //Clears the global interrupts
 port_init();
 sei();             //Enables the global interrupts
}

int main(void)
{
	init_devices();
	buzzer_off();     //Turn off buzzer

	int trial_times = 0;
	LCD_init();
	LCD_displayStringRowColumn(0,0,"Enter your password");
	INT0_Init();              // Enable external INT0
	DDRB |= (1<<PB0); //green led
	DDRD |= (1<<PD7); //red led
	/* configure pin PD0 and PD1 as output pins MOTOR */
	DDRD |= (1<<PD0);
	DDRD |= (1<<PD1);

	/* Motor is stop at the beginning */
	PORTD &= ~(1<<PD0);
	PORTD &= ~(1<<PD1);

	//red led initially off
	PORTD &=~ (1<<PD7);

	DDRC  |= (1<<PC0); //configure the led pin to be output pin.
	PORTC &= ~(1<<PC0); //LED is off at the beginning(Positive Logic).
	SREG  |= (1<<7); //enable global interrupts in MC by setting the I-Bit.


    while(1)
    {
    	unsigned char key1;
    	unsigned char key2;
    	unsigned char key3;
		/* if any switch pressed for more than 500 ms it counts more than one press */
		key1 = KeyPad_getPressedKey(); /* get the pressed key number */
		if((key1 <= 9) && (key1 >= 0))
		{
			LCD_goToRowColumn(1,0);
			LCD_displayString("*");
		}
		_delay_ms(500); /* Press time */

		key2 = KeyPad_getPressedKey(); /* get the pressed key number */
		if((key2 <= 9) && (key2 >= 0))
		{
			LCD_goToRowColumn(1,1);
			LCD_displayString("*");
		}
		_delay_ms(500); /* Press time */

		key3 = KeyPad_getPressedKey(); /* get the pressed key number */
		if((key3 <= 9) && (key3 >= 0))
		{
			LCD_goToRowColumn(1,2);
			LCD_displayString("*");
		}
		_delay_ms(500); /* Press time */

		/* check if the password is correct (Rotate clock wise) */
		if((key1 == 1)&& (key2 == 2)&& (key3 == 3))
		{
			PORTD &= (~(1<<PD0));
			PORTD |= (1<<PD1);
			LCD_clearScreen();
			LCD_goToRowColumn(0,0);
			LCD_displayString("Correct");
			PORTB |= (1<<PB0); //green led on
		}
		/* check if the password is incorrect (turn off the motor) */
		else if ((key1 != 1)&& (key2 != 2)&& (key3 != 3))
		{
			PORTD &= (~(1<<PD0));
			PORTD &= (~(1<<PD1));

			trial_times ++;
			LCD_goToRowColumn(0,0);
			if (trial_times >= 3)
			{
				LCD_clearScreen();
				LCD_displayString("Alarm");
				timer0_init_normal_mode(); //start the timer.
			}
			else
			{
				LCD_clearScreen();
				LCD_displayString("Wrong ,Re-enter the password");
			}
		}
    }
}
