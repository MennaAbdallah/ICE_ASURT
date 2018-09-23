#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FAST 2
#define SLOW 4

unsigned char g_tick_fast ;
unsigned char g_tick_slow ;

unsigned char g_Interrupt_Flag = 0;

ISR(TIMER0_OVF_vect)
{
	g_tick_fast++;
	g_tick_slow++;
	if ((g_tick_fast == FAST) && (g_Interrupt_Flag == 3))
	{
		PORTC ^=(1<<PC0);
		g_tick_fast =0;
	}
	if ((g_tick_slow == SLOW) && (g_Interrupt_Flag == 3))
	{
		PORTC ^=(1<<PC1);
		g_tick_slow =0;
	}
	if ((g_tick_fast == FAST) && (g_Interrupt_Flag == 4))
	{
		PORTC ^=(1<<PC1);
		g_tick_fast =0;
	}
	if ((g_tick_slow == SLOW) && (g_Interrupt_Flag == 4))
	{
		PORTC ^=(1<<PC2);
		g_tick_slow =0;
	}
}
/* External INT1 Interrupt Service Routine */
ISR(INT1_vect)
{
	g_Interrupt_Flag++;
	g_tick_fast = 0 ;
	g_tick_slow = 0 ;
	timer0_init_normal_mode(); //start the timer.

}

/* External INT1 enable and configuration function */
void INT1_Init(void)
{
	SREG  &= ~(1<<7);      // Disable interrupts by clearing I-bit
	DDRD  &= (~(1<<PD3));  // Configure INT1/PD3 as input pin
	PORTD |= (1<<PD3);     // Enable the internal pull up resistor at PD3 pin
	GICR  |= (1<<INT1);    // Enable external interrupt pin INT1
	// Trigger INT1 with the falling edge
	MCUCR |= (1<<ISC11);
	MCUCR &= ~(1<<ISC10);
	SREG  |= (1<<7);       // Enable interrupts by setting I-bit
}
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

int main(void)
{
	DDRC  |= (1<<PC0); //configure the led pin to be output pin.
	PORTC &= ~(1<<PC0); //LED is off at the beginning(Positive Logic).
	SREG  |= (1<<7); //enable global interrupts in MC by setting the I-Bit.

	INT1_Init();           // Enable and configure external INT1

	DDRC  = 0x07;          // Configure all PORTC pins as output pins
	PORTC = 0x00;          // All leds are off (Positive Logic configuration)

	while(1)
    {
		if(g_Interrupt_Flag == 0)
		{
			_delay_ms(500);
			PORTC = 0x00;
		}
		else if(g_Interrupt_Flag == 1)
		{
			PORTC = 0x01;
		}
		else if(g_Interrupt_Flag == 2)
		{
			_delay_ms(500);
			PORTC ^=(1<<PC0);
			PORTC |=(1<<PC1);
			PORTC &=~(1<<PC2);
		}
		else if(g_Interrupt_Flag == 3)
		{
			PORTC |=(1<<PC2);
		}
		else if(g_Interrupt_Flag == 4)
		{
			PORTC &=~(1<<PC0);
		}
		/*else if(g_Interrupt_Flag == 3)
		{
			for(int x = 0; x<=5 ; x++)
			{
			PORTC ^=(1<<PC0);
			}
			for(int x = 0; x<=10 ; x++)
			{
			PORTC ^=(1<<PC1);
			}
			PORTC |=(1<<PC2);
		}
		else if(g_Interrupt_Flag == 4)
		{
			PORTC &=~(1<<PC0);
			for(int x = 0; x<=5 ; x++)
			{
			PORTC ^=(1<<PC1);
			}
			for(int x = 0; x<=10 ; x++)
			{
			PORTC ^=(1<<PC2);
			}
		}*/
		else if(g_Interrupt_Flag == 5)
		{
			PORTC &=~(1<<PC0);
			PORTC &=~(1<<PC1);
			_delay_ms(100);
			PORTC ^=(1<<PC2);
		}
		else if(g_Interrupt_Flag == 6)
		{
			g_Interrupt_Flag = 0; //after finish make flag = 0 again
		}
	}
}
