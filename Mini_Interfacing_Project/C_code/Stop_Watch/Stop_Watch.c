/*
 * Project: Stop Watch
 * Author: [Amr Mohamed]
 * Date: [Sep, 12, 2024]
 *
 * Description:
 * This project implements a digital Stop Watch using an ATmega32 microcontroller.
 * It uses a multiplexed 7-segment display to show hours, minutes, and seconds.
 * The time is updated every second using Timer1 in CTC mode with a 1-second interrupt.
 *
 * Features:
 * count_up, count_down and time adjustment
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define ZERO_Condition (Sec_Tick_Units + Sec_Tick_Tens + Min_Tick_Units\
		+ Min_Tick_Tens + Hour_Tick_Units + Hour_Tick_Tens == 0)


void Atmega32_pins(void)
{
	//Note: configuring without messing with other unused pins

	DDRA |= 0x3F; 				//Setting PA0..5 to 1
	DDRB &= 0x00; 				//Setting PB0..7 to 0
	DDRC |= 0x0F; 				//Setting PC0..3 to 0
	DDRD = (DDRD | 0x31) & 0xF3;//Setting PD0, PD4, PD5 to 1, And PD2, PD3 to 0
	PORTB |= 0xFF; 				//ensuring HIGH on the port input pins when not pressed
	PORTC &= 0xF0;				//Initial value of decoder
	PORTD |= (1 << PD2);
}


//These variables store the units and tens digits for seconds, minutes, and hours.
unsigned char Sec_Tick_Units  = 0;	//1st digit
unsigned char Sec_Tick_Tens   = 0;	//2nd digit
unsigned char Min_Tick_Units  = 0;	//3rd digit
unsigned char Min_Tick_Tens   = 0;	//4th digit
unsigned char Hour_Tick_Units = 0;	//5th digit
unsigned char Hour_Tick_Tens  = 0;	//6th digit

//Count Up/Down
unsigned char Mode_Switch = 0;

//operation Resume/Pause
unsigned char operation_Flag = 0;
unsigned char flag1 = 0;
unsigned char flag2 = 0;
unsigned char flag3 = 0;
unsigned char flag4 = 0;
unsigned char flag5 = 0;
unsigned char flag6 = 0;

//Mode Button
void Mode_Up_Down()
{
	if (!(PINB & (1 << PB7)) && Mode_Switch == 0)
	{
		while (!(PINB & (1 << PB7)))
		{

		}
		Mode_Switch = 1;
	}

	else if ((!(PINB & (1 << PB7))) && Mode_Switch == 1)
	{
		while (!(PINB & (1 << PB7)))
		{

		}
		Mode_Switch = 0;
	}
}

//Multiplexing the 7-Segment Display:
void Enable_Up_Down()
{
	/*rounding and enabling on the 7-segments displays*/
	PORTA |= (1 << PA5);
	PORTC = Sec_Tick_Units;
	_delay_ms(2);
	PORTA &= ~(1 << PA5);

	PORTA |= (1 << PA4);
	PORTC = Sec_Tick_Tens;
	_delay_ms(2);
	PORTA &= ~(1 << PA4);

	PORTA |= (1 << PA3);
	PORTC = Min_Tick_Units;
	_delay_ms(2);
	PORTA &= ~(1 << PA3);

	PORTA |= (1 << PA2);
	PORTC = Min_Tick_Tens;
	_delay_ms(2);
	PORTA &= ~(1 << PA2);

	PORTA |= (1 << PA1);
	PORTC = Hour_Tick_Units;
	_delay_ms(2);
	PORTA &= ~(1 << PA1);

	PORTA |= (1 << PA0);
	PORTC = Hour_Tick_Tens;
	_delay_ms(2);
	PORTA &= ~(1 << PA0);
}

//Checking for Buzzer when counting down to ZERO
void Buzzer_Check()
{

	if(ZERO_Condition && Mode_Switch == 1 && operation_Flag == 0)
	{
		PORTD |= (1<<PD0); //Buzzer ON
		PORTC=0; //Stay on ZERO, Unless pressing pause or mode switch buttons
	}
	else
	{
		PORTD &= ~(1<<PD0);	//Buzzer ON
	}
}

//increment scenario
void Digit_increment()
{
	/*setting digits increment scenario*/
		if(Sec_Tick_Units==10)
		{
			Sec_Tick_Units = 0;
			Sec_Tick_Tens++;
		}
		if(Sec_Tick_Tens==6)
		{
			Sec_Tick_Tens = 0;
			Min_Tick_Units++;
		}
		if(Min_Tick_Units==10)
		{
			Min_Tick_Units = 0;
			Min_Tick_Tens++;
		}
		if(Min_Tick_Tens==6)
		{
			Min_Tick_Tens = 0;
			Hour_Tick_Units++;
		}
		if(Hour_Tick_Units==10)
		{
			Hour_Tick_Units = 0;
			Hour_Tick_Tens++;
		}
		if(Hour_Tick_Tens==10)
		{
			Hour_Tick_Tens = 0;
		}

}

//decrement scenario
void Digit_decrement()
{

	if(Sec_Tick_Units == 255)
	{
		Sec_Tick_Units = 9;
		Sec_Tick_Tens--;
		if(Sec_Tick_Tens == 255)
		{
			Sec_Tick_Tens = 5;
			Min_Tick_Units--;
			if(Min_Tick_Units == 255)
			{
				Min_Tick_Units = 9;
				Min_Tick_Tens--;
				if(Min_Tick_Tens == 255)
				{
					Min_Tick_Tens = 5;
					Hour_Tick_Units--;
					if(Hour_Tick_Units == 255)
					{
						Hour_Tick_Units = 9;
						Hour_Tick_Tens--;
						if(Hour_Tick_Tens == 255)
						{
							Hour_Tick_Tens=0;
						}
					}
				}
			}
		}
	}
}

//Time base adjustment for counting down
void C_Down_Adjustments()
{
	if(operation_Flag == 1)
	{
		if(!(PINB & (1<<PB1)))	/*Hours increment*/
		{
			Hour_Tick_Units++;
			if(Hour_Tick_Units==10)
			{
				Hour_Tick_Units = 0;
				Hour_Tick_Tens++;
			}
			if(Hour_Tick_Tens==9 && Hour_Tick_Units==9)
			{
				Hour_Tick_Tens = 9;
				Hour_Tick_Units = 9;
			}
			while(!(PINB & (1<<PB1))){Enable_Up_Down();}
		}

		else if(!(PINB & (1<<PB4)))	/*Minutes increment*/
		{

			Min_Tick_Units++;
			if(Min_Tick_Units==10)
			{
				Min_Tick_Units = 0;
				Min_Tick_Tens++;
			}
			if(Min_Tick_Tens==5 && Min_Tick_Units == 9)
			{
				Min_Tick_Tens = 5;
				Min_Tick_Units = 9;
			}
			while(!(PINB & (1<<PB4))){Enable_Up_Down();}
		}

		else if(!(PINB & (1<<PB6)))	/*Seconds increment*/
		{

			Sec_Tick_Units++;
			if(Sec_Tick_Units==10)
			{
				Sec_Tick_Units = 0;
				Sec_Tick_Tens++;
			}
			if(Sec_Tick_Tens==5 && Sec_Tick_Units==9)
			{
				Sec_Tick_Units=9;
				Sec_Tick_Tens=5;
			}
			while(!(PINB & (1<<PB6))){Enable_Up_Down();}
		}

		else if(!(PINB & (1<<PB0))) /*Hours decrement*/
		{

			Hour_Tick_Units--;
			if(Hour_Tick_Units == 255)
			{
				Hour_Tick_Units = 9;
				Hour_Tick_Tens--;
				if(Hour_Tick_Tens == 255)
				{
					Hour_Tick_Units = 0;
					Hour_Tick_Tens = 0;
				}
			}
			while(!(PINB & (1<<PB0))){Enable_Up_Down();}
		}

		else if(!(PINB & (1<<PB3))) /*Minutes decrement*/
		{

			Min_Tick_Units--;
			if(Min_Tick_Units == 255)
			{
				Min_Tick_Units = 9;
				Min_Tick_Tens--;
				if( Min_Tick_Tens == 255)
				{
					Min_Tick_Units = 0;
					Min_Tick_Tens = 0;
				}
			}
			while(!(PINB & (1<<PB3))){Enable_Up_Down();}
		}

		else if(!(PINB & (1<<PB5))) /*Seconds decrement*/
		{

			Sec_Tick_Units--;
			if(Sec_Tick_Units == 255)
			{
				Sec_Tick_Units = 9;
				Sec_Tick_Tens--;
				if(Sec_Tick_Tens == 255)
				{
					Sec_Tick_Tens = 0;
					Sec_Tick_Units = 0;
				}
			}
			while(!(PINB & (1<<PB5))){Enable_Up_Down();}
		}
	}
}

//setting the main timer in the stop watch
void Timer1_Stop_Watch(void)
{
	TCCR1A = 0x00;  //Setting all TCCR1A bits to 0
	TCCR1B = 0x0D;  //Setting bits: WGM12, CS12, CS10 to 1
	TCNT1 = 0x0000; //Setting initial counting high byte to 0
	OCR1A = 15624;  //Setting the compare value for 1sec increment
	TIMSK = 0x10;	//Enables the Timer1 Output Compare A Match Interrupt
}

//setting external interrupts
void Ex_Interrupts(void)
{
	MCUCR = 0x0E; 		//Setting INT1 on rising edge and INT0 on falling edge
	MCUCSR &= ~(1<<6);	//Setting INT2 on falling edge
	GICR = 0xE0; 		//Enable INT0..2
	SREG |= (1<<7); 	//General interrupts enable (I_bit)
}

//timer interrupt
ISR(TIMER1_COMPA_vect)
{
	if(operation_Flag == 0)  //Condition for resume/pause buttons
	{
		if (Mode_Switch == 0)
		{
			Sec_Tick_Units++;
		}
		else if((Mode_Switch == 1 && !ZERO_Condition))
		{
				Sec_Tick_Units--;
		}
	}

}

//Reset
ISR(INT0_vect)
{
	//resetting digits
	 Sec_Tick_Units  = 0;
	 Sec_Tick_Tens   = 0;
	 Min_Tick_Units  = 0;
	 Min_Tick_Tens   = 0;
	 Hour_Tick_Units = 0;
	 Hour_Tick_Tens  = 0;
}

//pause
ISR(INT1_vect)
{
	operation_Flag = 1;
}

//resume
ISR(INT2_vect)
{
	operation_Flag = 0;
}

/*************************************************************************************************/
/*************************************************************************************************/

//MAIN_PROGRAM
int main()
{
	//PINs configuration
	Atmega32_pins();

	//Timer configuration
	Timer1_Stop_Watch();

	//interrupts configuration
	Ex_Interrupts();

	//Main program loop
	while(1)
	{
		//Multiplexing displays
		Enable_Up_Down();

		Buzzer_Check();

		//Handling Overflow when vounting down until less than ZERO


		//Check mode
		Mode_Up_Down();

		if (Mode_Switch == 0) //Count Up
		{
			//LEDs for mode indication
			PORTD |= (1<<PD4);
			PORTD &= ~(1<<PD5);
			Digit_increment();
		}
		else if (Mode_Switch == 1)//Count Down
		{
			//LEDs for mode indication
			PORTD |= (1<<PD5);
			PORTD &= ~(1<<PD4);

			//When pause
			if(operation_Flag == 1)
			{
				C_Down_Adjustments();
			}

			//When Resume
			else
			{
				Digit_decrement();

			}

		}
	}
}


