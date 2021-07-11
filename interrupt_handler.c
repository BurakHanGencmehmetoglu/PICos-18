#include "common.h"
#include <string.h>

extern char command[max_command_length];  // This command will be sent to the robot.
extern unsigned char lcd_state;    // LCD state which determines which lines will be loaded to the LCD.
extern unsigned char refresh_LCD;  // refresh_LCD flag to the receive_task. It indicates just refresh LCD, new coin did not arrive.
extern unsigned char program_state; // Program state. 0 -> idle, 1 -> active, 2 -> end.
extern char end_command[4];  // end command to the  robot.
extern unsigned char end_flag; // end_flag indicates we should send end command to the robot.


unsigned char command_index = 0;  // This indicates which index of command should be sent to the robot.
unsigned char rb4_button_state = 0; // -> 0 not pressed, 1 is pressed.
unsigned char received_message_length = 0; // I increment this when I receive command.

char message[max_message_length]; // I add new received message to this array.



extern void AddOneTick(void);
void InterruptVectorL(void);
void InterruptVectorH(void);


#pragma code IT_vector_low=0x18
void Interrupt_low_vec(void)
{
   _asm goto InterruptVectorL  _endasm
}
#pragma code

#pragma code IT_vector_high=0x08
void Interrupt_high_vec(void)
{
   _asm goto InterruptVectorH  _endasm
}
#pragma code


#pragma	code _INTERRUPT_VECTORL = 0x003000
#pragma interruptlow InterruptVectorL 
void InterruptVectorL(void)
{
	EnterISR();
	
	if (INTCONbits.TMR0IF == 1)
		AddOneTick();
    
    
    // I get new message and if it is ':', I set event for receive task. Because we have new coin.
	if (PIR1bits.RCIF == 1) {
        char received_message = RCREG;
        PIR1bits.RCIF = 0;
        message[received_message_length] = received_message;
        received_message_length++;
        if (received_message == ':')
            SetEvent(receive_task_id,receive_event);          
	}
    
    
    
    
    // I look command array and index and send next character to the robot.
    if (PIR1bits.TXIF == 1) {
        PIR1bits.TXIF = 0; 
        if (end_flag == 1) {
            if(command_index < 4){
                TXREG = end_command[command_index]; 
                command_index++;
            }
            else {
                command_index = 0;
                TXSTAbits.TXEN = 0; 
            }
        }
        else {
            if(command_index < 2){
                TXREG = command[command_index]; 
                command_index++;
            }
            else {
                command_index = 0;
                TXSTAbits.TXEN = 0; 
            }            
        }
    }
    
    
   
    // If RB4 is released, I change LCD state and set event receive task. Because we have to update LCD.
    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = 0;
        if (program_state == 0) {}
        else {
            if (rb4_button_state == 0 && PORTBbits.RB4 == 1) {}
            else if (rb4_button_state == 1 && PORTBbits.RB4 == 0) {}
            else if (rb4_button_state == 0 && PORTBbits.RB4 == 0)
                rb4_button_state = 1;
            else if (rb4_button_state == 1 && PORTBbits.RB4 == 1) {
                rb4_button_state = 0;
                if (lcd_state == 1)
                    lcd_state = 0;
                else 
                    lcd_state = 1;
                refresh_LCD = 1;
                SetEvent(receive_task_id,receive_event);
            }
        }
    }
    
        
        
    if (RCSTAbits.OERR)
    {
      RCSTAbits.CREN = 0;
      RCSTAbits.CREN = 1;
    }
	LeaveISR();
}
#pragma	code
                                            
#pragma	code _INTERRUPT_VECTORH = 0x003300
#pragma interrupt InterruptVectorH nosave=FSR0, TBLPTRL, TBLPTRH, TBLPTRU, TABLAT, PCLATH, PCLATU, PROD, section(".tmpdata"), section("MATH_DATA")
void InterruptVectorH(void)
{
  if (INTCONbits.INT0IF == 1)
    INTCONbits.INT0IF = 0;
}
#pragma	code



extern void _startup (void);
#pragma code _RESET_INTERRUPT_VECTOR = 0x003400
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code