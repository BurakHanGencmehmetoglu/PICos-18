#ifndef COMMON_H
#define COMMON_H

#include "device.h"

#define _10MHZ	63320
#define _16MHZ	61768
#define _20MHZ	60768
#define _32MHZ	57768
#define _40MHZ 	55768


#define alarm_event       0x80
#define receive_event     0x40


#define send_task_id             1
#define receive_task_id          2


#define send_task_prio           5
#define receive_task_prio        10



#define alarm_for_send          0


#define number_of_coins 25
#define max_message_length 10
#define max_command_length 2


typedef struct {
    unsigned char is_coin_active;
    int x_coordinate;
    int y_coordinate;
    unsigned char remaining_commands;
    unsigned char coin_type;
} coin;


#endif