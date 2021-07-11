#ifndef __LCD
#define __LCD
void WriteCommandToLCD(unsigned char);
void WriteDataToLCD(char);
void InitLCD(void);
void Lcd_position(unsigned char line, unsigned char pos);
void send_data_to_LCD(char value,unsigned char line,unsigned char column);
void ToggleEpinOfLCD(void);
#endif