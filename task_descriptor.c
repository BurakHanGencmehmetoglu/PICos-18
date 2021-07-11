#include "common.h"

Counter Counter_list[] = 
  {
   {
     {
       200,                                
        10,                                
       100                                 
     },
     0,                                    
     0                                    
   }
  };

Counter Counter_kernel = 
  {
    {
      65535,                              
          1,                              
          0                               
    },
    0,                                    
    0                                    
  };

AlarmObject Alarm_list[] = 
  {
   {
     OFF,            
     0,                                    
     0,                                  
     &Counter_kernel,                    
     send_task_id,                           
     alarm_event,                         
     0                              
   },
 };

#define _ALARMNUMBER_          sizeof(Alarm_list)/sizeof(AlarmObject)
#define _COUNTERNUMBER_        sizeof(Counter_list)/sizeof(Counter)
unsigned char ALARMNUMBER    = _ALARMNUMBER_;
unsigned char COUNTERNUMBER  = _COUNTERNUMBER_;
unsigned long global_counter = 0;

Resource Resource_list[] = 
  {
   {
      10,         
       0,                                
       0,                                
   }
  };
  
#define _RESOURCENUMBER_       sizeof(Resource_list)/sizeof(Resource)
unsigned char RESOURCENUMBER = _RESOURCENUMBER_;


#define DEFAULT_STACK_SIZE      256
DeclareTask(send_task);
DeclareTask(receive_task);


#pragma		udata      STACK_A   
volatile unsigned char stack0[DEFAULT_STACK_SIZE];
#pragma		udata      STACK_B   
volatile unsigned char stack1[DEFAULT_STACK_SIZE];
#pragma		udata


#pragma		romdata		DESC_ROM
const rom unsigned int descromarea;


rom_desc_tsk rom_desc_task0 = {
	send_task_prio,            
	stack0,                          
	send_task,                         
	READY,                        
	send_task_id,                         
	sizeof(stack0)                   
};


rom_desc_tsk rom_desc_task1 = {
	receive_task_prio,   
	stack1,           
	receive_task,       
	READY,            
	receive_task_id,      
	sizeof(stack1)                    
};


rom_desc_tsk end = {
	0x00,                              
	0x0000,                            
	0x0000,                           
	0x00,                              
	0x00,                             
	0x0000                          
};

volatile rom unsigned int * taskdesc_addr = (&(descromarea)+1);