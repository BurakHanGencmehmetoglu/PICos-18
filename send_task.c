#include <math.h>
#include "common.h"
#include "LCD.h"


extern unsigned char refresh_LCD;
extern unsigned char number_of_received_coins;
extern unsigned char program_state;
extern coin coin_array[number_of_coins];
extern char lcd_line0[16];
extern char lcd_line1[16];
extern char lcd_line2[16];
extern char lcd_line3[16];





char command[max_command_length]; // Command for the interrupt handler. It will look these and send proper characters.
char end_command[4];


unsigned char end_flag = 0;  
unsigned char determined_coin_index = 30; // determined coin index in coin array. It will change after determine_coin_coordinate() function call.

int robot_pixel_x = 25; // Information about the robot. Initially, coordinates are 25,25 and degree is 180. I update them properly according the sended command. 
int robot_pixel_y = 25;
int degree = 180;

int determined_coin_pixel_x = -1; // Coordinates of determined coin.
int determined_coin_pixel_y = -1;



void determine_coin_coordinate(void);
void determine_command(void);
int absolute_difference(int first_number,int second_number);
void update_command_numbers(void);
void send_data_to_LCD(char value,unsigned char line,unsigned char column);
int take_mod_of_degree(int degree);


TASK(send_task)
{
    end_command[0] = 'E';
    end_command[1] = 'N';
    end_command[2] = 'D';
    end_command[3] = ':';
    command[1] = ':';
    SetRelAlarm(alarm_for_send, 100, 110);
	while(1) {
        WaitEvent(alarm_event);
        ClearEvent(alarm_event);
        if (program_state == 1) {              // We are in sending command stage.
            determine_coin_coordinate(); // Determine nearest coin coordinate.
            determine_command();  // Determine next command according to coin and robot coordinate.
            TXREG = '$';
            TXSTAbits.TXEN = 1;
            update_command_numbers(); // Decrement remaining command number for coins. 
        }
        else if (program_state == 2) {         // All of the 25 coins are disappeared. We are ending the simulation.
            end_flag = 1;
            TXREG = '$';
            TXSTAbits.TXEN = 1;
            program_state = 3;
        }        
	}
	TerminateTask(); 
}






// I traverse in coin array and find nearest coin. After finding, I save this coin coordinates and index.
void determine_coin_coordinate(void) {
    if (number_of_received_coins == 0) return;
    else {
        unsigned char i;    
        int coin_pixel_x;
        int coin_pixel_y;
        int min_distance = 3000;
        determined_coin_pixel_x = -1;
        determined_coin_pixel_y = -1;
        determined_coin_index = 30;
        for (i=0;i<number_of_received_coins;i++) {
            if (coin_array[i].is_coin_active == 1) {
                coin_pixel_x = (coin_array[i].x_coordinate * 50) + 25; 
                coin_pixel_y = (coin_array[i].y_coordinate * 50) + 25; 
                if (absolute_difference(coin_pixel_x,robot_pixel_x) + absolute_difference(coin_pixel_y,robot_pixel_y) < min_distance) {
                   min_distance = absolute_difference(coin_pixel_x,robot_pixel_x) + absolute_difference(coin_pixel_y,robot_pixel_y);
                   determined_coin_pixel_x = coin_pixel_x;
                   determined_coin_pixel_y = coin_pixel_y;
                   determined_coin_index = i; 
                }   
            }
        }   
    }
}








// I determine next command according to different scenarios.
void determine_command(void) {
    
    // At the beginning of the game, I came 25,75 position to save couple of commands.
    if (number_of_received_coins == 0) {
        if (robot_pixel_y < 75) {
            command[0] = 'F';  
            robot_pixel_y = robot_pixel_y + 5;
        }
        else
            command[0] = 'S'; 
        return;        
    }
    
    // If there is no coin, I send stop command to forward simulation.
    if (determined_coin_pixel_x == -1) {
        command[0] = 'S';    
        return;
    }
    
    
    
    /* We find our coin. Let's send pick command. If degree is not multiple of 90, I firstly align robot. */
    if (robot_pixel_x == determined_coin_pixel_x && robot_pixel_y == determined_coin_pixel_y) {
        if (take_mod_of_degree(degree) == 0) {
            command[0] = 'P';
            coin_array[determined_coin_index].is_coin_active = 0;
            if (coin_array[determined_coin_index].y_coordinate == 0)
                lcd_line0[coin_array[determined_coin_index].x_coordinate] = ' ';
            if (coin_array[determined_coin_index].y_coordinate == 1)
                lcd_line1[coin_array[determined_coin_index].x_coordinate] = ' ';
            if (coin_array[determined_coin_index].y_coordinate == 2)
                lcd_line2[coin_array[determined_coin_index].x_coordinate] = ' ';
            if (coin_array[determined_coin_index].y_coordinate == 3)
                lcd_line3[coin_array[determined_coin_index].x_coordinate] = ' ';

            refresh_LCD = 1;
            SetEvent(receive_task_id, receive_event);
            return;
        } else {
            if (take_mod_of_degree(degree) > 45) {
                command[0] = 'L';
                degree = degree + 9;
                if (degree == 360)
                    degree = 0;
                return;
            } else {
                command[0] = 'R';
                degree = degree - 9;
                if (degree < 0)
                    degree = 351;
                return;
            }
        }
    }
    

    
    
    /* Obstacle case. Destination coin is behind the wall. We will get robot to the second line and then it will move forward through the second line. */
    
                                                                                                             
            if ((determined_coin_pixel_x > 350 && robot_pixel_x < 350) || (determined_coin_pixel_x < 350 && robot_pixel_x > 350)) {
                // We are at the top of the gap. We should move downward.
                if (robot_pixel_y < 75) {
                    if (degree == 180) {
                        command[0] = 'F';
                        robot_pixel_y = robot_pixel_y + 5;
                        return;
                    }


                    if (degree > 0 && degree < 180) {
                        command[0] = 'L';
                        degree = degree + 9;
                        if (degree == 360)
                            degree = 0;
                        return;
                    } else {
                        command[0] = 'R';
                        degree = degree - 9;
                        if (degree < 0)
                            degree = 351;
                        return;
                    }
                }

                // We are at the down of the gap. We should move upward.
                if (robot_pixel_y > 75) {

                    if (degree == 0) {
                        command[0] = 'F';
                        robot_pixel_y = robot_pixel_y - 5;
                        return;
                    }

                    if (degree > 180) {
                        command[0] = 'L';
                        degree = degree + 9;
                        if (degree == 360)
                            degree = 0;
                        return;
                    } else {
                        command[0] = 'R';
                        degree = degree - 9;
                        if (degree < 0)
                            degree = 351;
                        return;
                    }
                } 
            }
     
              
            

    
    
    
    

            if (determined_coin_pixel_x > robot_pixel_x) {
                if (degree == 270) {
                    command[0] = 'F';
                    robot_pixel_x = robot_pixel_x + 5;
                    return;
                }

                if (degree > 90 && degree < 270) {
                    command[0] = 'L';
                    degree = degree + 9;
                    if (degree == 360)
                        degree = 0;
                    return;
                }
                else {
                    command[0] = 'R';
                    degree = degree - 9;
                    if (degree < 0)
                        degree = 351;
                    return;
                }    
            }
    
    
    

    

            if (determined_coin_pixel_x < robot_pixel_x) {
                if (degree == 90) {
                    command[0] = 'F';
                    robot_pixel_x = robot_pixel_x - 5;
                    return;
                }

                if (degree < 90 || degree > 270) {
                    command[0] = 'L';
                    degree = degree + 9;
                    if (degree == 360)
                        degree = 0;
                    return;
                } else {
                    command[0] = 'R';
                    degree = degree - 9;
                    if (degree < 0)
                        degree = 351;
                    return;
                }        
            } 
    
    
    
    
    
    

            if (determined_coin_pixel_y > robot_pixel_y) {
                if (degree == 180) {
                    command[0] = 'F';
                    robot_pixel_y = robot_pixel_y + 5;
                    return;
                }

                if (degree > 0 && degree < 180) {
                    command[0] = 'L';
                    degree = degree + 9;
                    if (degree == 360)
                        degree = 0;
                    return;
                }
                else {
                    command[0] = 'R';
                    degree = degree - 9;
                    if (degree < 0)
                        degree = 351;
                    return;
                }                
            }    
      
    
    
   

            if (determined_coin_pixel_y < robot_pixel_y) {
                if (degree == 0) {
                    command[0] = 'F';
                    robot_pixel_y = robot_pixel_y - 5;
                    return;
                }

                if (degree > 180) {
                    command[0] = 'L';
                    degree = degree + 9;
                    if (degree == 360)
                        degree = 0;
                    return;
                } else {
                    command[0] = 'R';
                    degree = degree - 9;
                    if (degree < 0)
                        degree = 351;
                    return;
                } 
            }             
    
    
    
    
    
             else {
                command[0] = 'S';    
                return;
             }
}







// I traverse in coin array and decrement remaining command number. I update LCD if necessary.
// If all 25 coins are not active, then I change program state to finish simulation.
void update_command_numbers(void) {
    unsigned char i;
    unsigned char flag = 0;
    for (i=0;i<number_of_received_coins;i++) {
        if (coin_array[i].is_coin_active == 1 && coin_array[i].remaining_commands == 1) {
            coin_array[i].is_coin_active = 0;
            if (coin_array[i].y_coordinate == 0)
                lcd_line0[coin_array[i].x_coordinate] = ' ';
            if (coin_array[i].y_coordinate == 1)
                lcd_line1[coin_array[i].x_coordinate] = ' ';
            if (coin_array[i].y_coordinate == 2)
                lcd_line2[coin_array[i].x_coordinate] = ' ';
            if (coin_array[i].y_coordinate == 3)
                lcd_line3[coin_array[i].x_coordinate] = ' '; 
            
            refresh_LCD = 1;
            SetEvent(receive_task_id,receive_event);
        }      
        else
            coin_array[i].remaining_commands = coin_array[i].remaining_commands - 1; 
    }
    
    if (number_of_received_coins == 25) {
        for (i=0;i<25;i++) {
            if (coin_array[i].is_coin_active == 1) {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
            program_state = 2;
    }   
}







// Take absoloute difference. I use this while calculating the nearest coin.
int absolute_difference(int first_number,int second_number) {
    if (first_number > second_number)
        return first_number - second_number;
    return second_number - first_number;    
}



// Take degree according to the 90. I use this before sending pick command.
int take_mod_of_degree(int degree) {
    if (degree==0 || degree==90 || degree==180 || degree==270)
        return 0;
    if (degree < 90)
        return degree;
    if (degree < 180)
        return degree - 90;
    if (degree < 270)
        return degree - 180;
    if (degree < 360)
        return degree - 270;    
}