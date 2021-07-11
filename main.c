#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#include <p18f4620.h>
#include "common.h"
#include "LCD.h"


#define DEFAULT_MODE       0

void send_data_to_LCD(char value,unsigned char line,unsigned char column);
void main (void);
void Init(void);
void StartupHook(void);
void ShutdownHook(StatusType error);
void ErrorHook(StatusType error);
void PreTaskHook(void);
void PostTaskHook(void);

extern union Timers Tmr0;

AppModeType SelectedMode;


/*
 Burak Han Gencmehmetoglu.
****************************************************************** COMMENT ABOUT THE CODE ***************************************************************************************************
 
 - I used two tasks. One of them is sending command to the robot. One of them is receiving command and update LCD if necessary.  
 - receive_task has higher priority. Because I wanted to take new coin info immediately.
 - Receive task receive commands and updates global 25 item long coin_array accordingly. This is struct array and it contains x and y coordinate, is_coin_alive, remaining commands and 
   coin type informations about the coin.
 - After receiving coin info, it updates LCD. It also updates LCD if rb4 button is released.
 - For LCD, I created 4 array each represents line(each of them is 16 character long). If LCD state is 0, I load zeroth and first line to the LCD. If LCD state is 1, I load second and third line
   to the LCD. This LCD state changes according to rb4 button action.
 - send_task calculates next move and send command to the robot accordingly.
 - I choose go to the nearest coin algorithm.
 - So, send_task first looks global coin array and picks nearest coin.
 - Then, according to our position and determined coin position, it sends next command. (forward,turn right,left etc.)
 - After sending command, it decrements remaning command numbers in coin array. 
 - If all of the coins disappered, it sends end command and simulator finishes.
 - I commented for each function and variable. For detailed explanation, you can read them.
 
 */


void main(void) {
    unsigned char i;
    char temp_line0[17] = "      GOLD      ";
    char temp_line1[17] = "      RUSH      ";

    
    STKPTR = 0;
    SelectedMode = DEFAULT_MODE;
    Init();
    

    for (i=0;i<16;i++) {
       send_data_to_LCD(temp_line0[i],0,i); 
       send_data_to_LCD(temp_line1[i],1,i); 
    }

    while(1) {
      StartOS(SelectedMode);
    }
}


void Init(void) {
	FSR0H = 0;
	FSR0L = 0;
	
	/* User setting : actual PIC frequency */
	Tmr0.lt = _10MHZ;
	/* Timer OFF - Enabled by Kernel */
	T0CON = 0x08;
	TMR0H = Tmr0.bt[1];
	TMR0L = Tmr0.bt[0];
	
	/* configure I/O ports */
    TRISCbits.RC7 = 1; // TX1 and RX1 pin configuration
    TRISCbits.RC6 = 0;
    
    ADCON1 = 0x8E;
    ADCON0 = 0x00;
    InitLCD();
	
    TRISB = 0b00010000;
    PORTBbits.RB4 = 1;
    

    
    
    
	/* configure USART transmitter/receiver */
	SPBRG = 64;		// for 10 MHz, to have 9600 baud rate, it should be 64
	TXSTA = 0x04;      // (= 00000100) 8-bit transmit, transmitter NOT enabled,
                        // asynchronous, high speed mode
	RCSTA = 0x90;      // (= 10010000) 8-bit receiver, receiver enabled,
                        // continuous receive, serial port enabled
	
	/* configure the interrupts */
	INTCON = 0;			// clear interrupt register completely
	PIE1bits.TXIE = 1;	// enable USART transmit interrupt
	PIE1bits.RCIE = 1;	// enable USART receive interrupt
	PIR1 = 0;			// clear all peripheral flags
	
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = 1;
    INTCON2bits.RBIP = 0;
    
	INTCONbits.PEIE = 1;// enable peripheral interrupts
	INTCONbits.GIE = 1;	// globally enable interrupts
}
void StartupHook(void){}
void ShutdownHook(StatusType error){}
void ErrorHook(StatusType error){}
void PreTaskHook(void){}
void PostTaskHook(void){}