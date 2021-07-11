#include "common.h"
#include <p18cxxx.h>
#include <p18f4620.h>
#include "LCD.h"

void WriteCommandToLCD(unsigned char);
void WriteDataToLCD(char);
void InitLCD(void);
void Lcd_position(unsigned char line, unsigned char pos);
void ToggleEpinOfLCD(void);
void send_data_to_LCD(char value,unsigned char line,unsigned char column);



void send_data_to_LCD(char value,unsigned char line,unsigned char column) {
    WriteCommandToLCD(0x02);
    Lcd_position(line,column);
    WriteDataToLCD(value);
}

void Lcd_position(unsigned char line, unsigned char pos)
{
    switch(line)
    {
        case 0:
            WriteCommandToLCD(0x80+pos);
            break;
        case 1:
            WriteCommandToLCD(0xc0+pos);
            break;
    };
    
}

void WriteCommandToLCD(unsigned char Command)
{
    LATEbits.LATE2 = 0;                 // It is a command

    PORTD = PORTD & 0x0F;               // Make Data pins zero
    PORTD = PORTD |(Command&0xF0);      // Write Upper nibble of data
    ToggleEpinOfLCD();                  // Give pulse on E pin

    PORTD = PORTD & 0x0F;               // Make Data pins zero
    PORTD = PORTD | ((Command<<4)&0xF0);// Write Lower nibble of data
    
    ToggleEpinOfLCD();                  // Give pulse on E pin
}

void WriteDataToLCD(char value)
{
    LATEbits.LATE2 = 1;                 // It is data

    PORTD = PORTD & 0x0F;               // Make Data pins zero
    PORTD = PORTD | (value&0xF0);       // Write Upper nibble of data
    ToggleEpinOfLCD();                  // Give pulse on E pin

    PORTD = PORTD & 0x0F;               // Make Data pins zero
    PORTD = PORTD | ((value<<4)& 0xF0); // Write Lower nibble of data

    ToggleEpinOfLCD();                  // Give pulse on E pin
}

void ToggleEpinOfLCD(void)
{
    LATEbits.LATE1 = 1;                // Give a pulse on E pin
    LATEbits.LATE1 = 0;                // data from data bus
}

void InitLCD(void)
{
    TRISE = 0;
    PORTE = 0;

    LATEbits.LATE1  = 0;   // E  = 0
    LATEbits.LATE2  = 0;   // RS = 0
    LATDbits.LATD4  = 0;   // Data bus = 0
    LATDbits.LATD5  = 0;   // Data bus = 0
    LATDbits.LATD6  = 0;   // Data bus = 0
    LATDbits.LATD7  = 0;   // Data bus = 0
    TRISBbits.TRISB5 = 0;   // Make Output
    TRISBbits.TRISB2 = 0;   // Make Output
    TRISDbits.TRISD4 = 0;   // Make Output
    TRISDbits.TRISD5 = 0;   // Make Output
    TRISDbits.TRISD6 = 0;   // Make Output
    TRISDbits.TRISD7 = 0;   // Make Output
    LATEbits.LATE1   = 0;   // E  = 0
    LATEbits.LATE2   = 0;   // RS = 0

        ///////////////// Reset process from datasheet //////////////

	PORTD &= 0x0F;			  // Make Data pins zero
	PORTD |= 0x30;			  // Write 0x3 value on data bus
	ToggleEpinOfLCD();		  // Give pulse on E pin

	PORTD &= 0x0F;			  // Make Data pins zero
	PORTD |= 0x30;			  // Write 0x3 value on data bus
	ToggleEpinOfLCD();		  // Give pulse on E pin

	PORTD &= 0x0F;			  // Make Data pins zero
	PORTD |= 0x30;			  // Write 0x3 value on data bus
	ToggleEpinOfLCD();		  // Give pulse on E pin

	PORTD &= 0x0F;			  // Make Data pins zero
	PORTD |= 0x20;			  // Write 0x2 value on data bus
	ToggleEpinOfLCD();		  // Give pulse on E pin

  /////////////// Reset Process End ////////////////
	WriteCommandToLCD(0x2C);    //function set  //2C ya da 2D
	WriteCommandToLCD(0x0C);    //display on,cursor off,blink off //OxOC cursor offf
	WriteCommandToLCD(0x01);    //clear display
}