#include "common.h"
#include "LCD.h"


extern unsigned char received_message_length; // Message variables in interrupt handler.
extern char message[max_message_length];



char lcd_line0[16]; // 4 lines for LCD.
char lcd_line1[16];
char lcd_line2[16];
char lcd_line3[16];
unsigned char lcd_state = 0; // LCD state which determines which lines will be loaded to the LCD.
unsigned char refresh_LCD = 0;  // It indicates just refresh LCD, new coin did not arrive.
unsigned char number_of_received_coins = 0; // I increment this number when new coin arrives.
unsigned char program_state = 0; // Program state. 0 -> idle, 1 -> active, 2 -> end.
coin coin_array[number_of_coins];



void send_data_to_LCD(char value,unsigned char line,unsigned char column);



TASK(receive_task) 
{
    unsigned char i;
    for (i=0;i<16;i++) {
        lcd_line0[i] = 0;
        lcd_line1[i] = 0;
        lcd_line2[i] = 0;
        lcd_line3[i] = 0;
    }
    lcd_line0[7] = 0xff; // Fill proper x coordinate of lines with obstacle.
    lcd_line2[7] = 0xff;
    lcd_line3[7] = 0xff;    
    

	while(1) {
        WaitEvent(receive_event);
        ClearEvent(receive_event);
        if (refresh_LCD == 1) {         // According to LCD state, send lines to the LCD.
            refresh_LCD = 0;
            if (lcd_state == 0) {
                for (i=0;i<16;i++) {
                    send_data_to_LCD(lcd_line0[i],0,i); 
                    send_data_to_LCD(lcd_line1[i],1,i); 
                }
            }
            else {
                for (i=0;i<16;i++) {
                    send_data_to_LCD(lcd_line2[i],0,i); 
                    send_data_to_LCD(lcd_line3[i],1,i); 
                }            
            }
        }
        
        else {
            
            // New command arrived. If program state is 1, it means new coin is arrived. So, we update coin array and LCD (to write G or S).
            // If message is GO, we change program state to the 1 and update LCD (to delete gold rush string). 
            char message_temp[max_message_length];
            unsigned char length_temp = received_message_length;
            received_message_length = 0;
            
            for (i=0;i<length_temp;i++)
                message_temp[i] = message[i];              
                        
            if (program_state == 1) {
                coin_array[number_of_received_coins].x_coordinate = (int) message_temp[2];
                coin_array[number_of_received_coins].y_coordinate = (int) message_temp[3];
                coin_array[number_of_received_coins].coin_type = message_temp[4];
                coin_array[number_of_received_coins].is_coin_active = 1;
                if (coin_array[number_of_received_coins].coin_type == 1) {
                    coin_array[number_of_received_coins].remaining_commands = 90;
                    if (message_temp[3] == 0)
                        lcd_line0[message_temp[2]] = 'G';
                    if (message_temp[3] == 1)
                        lcd_line1[message_temp[2]] = 'G';
                    if (message_temp[3] == 2)
                        lcd_line2[message_temp[2]] = 'G';
                    if (message_temp[3] == 3)
                        lcd_line3[message_temp[2]] = 'G';
                }
                else { 
                    coin_array[number_of_received_coins].remaining_commands = 100;
                    if (message_temp[3] == 0)
                        lcd_line0[message_temp[2]] = 'S';
                    if (message_temp[3] == 1)
                        lcd_line1[message_temp[2]] = 'S';
                    if (message_temp[3] == 2)
                        lcd_line2[message_temp[2]] = 'S';
                    if (message_temp[3] == 3)
                        lcd_line3[message_temp[2]] = 'S';                
                }
                number_of_received_coins++;
                if (lcd_state == 0) {
                    for (i=0;i<16;i++) {
                        send_data_to_LCD(lcd_line0[i],0,i); 
                        send_data_to_LCD(lcd_line1[i],1,i); 
                    }
                }
                else {
                    for (i=0;i<16;i++) {
                        send_data_to_LCD(lcd_line2[i],0,i); 
                        send_data_to_LCD(lcd_line3[i],1,i); 
                    }            
                }               
            }
            
            if (message_temp[1] == 'G') {
                program_state = 1;
                if (lcd_state == 0) {
                    for (i=0;i<16;i++) {
                        send_data_to_LCD(lcd_line0[i],0,i); 
                        send_data_to_LCD(lcd_line1[i],1,i); 
                    }
                }
                else {
                    for (i=0;i<16;i++) {
                        send_data_to_LCD(lcd_line2[i],0,i); 
                        send_data_to_LCD(lcd_line3[i],1,i); 
                    }            
                }                
            }
                    
        }
	}
	TerminateTask();
}