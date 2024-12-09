#ifndef R_CG_IU_TEST
#define R_CG_IU_TEST

#include <stdint.h>


#define MAX_DATA_ENTRIES 35 

extern uint8_t device_exist[MAX_DATA_ENTRIES][2]; // Declare the global variable
extern uint8_t data_storage[MAX_DATA_ENTRIES][2]; 
extern uint8_t data_index; // Declare the global index

//Uart
extern volatile uint8_t  RX_Data;         // Stores the latest byte received
extern volatile uint8_t  RX_Buffer[15];    // Buffer to store incoming messages
extern volatile uint8_t RX_Index;      // Index for RX_Buffer


//extern uint8_t address_E[2];  // Address "01" for E command
extern uint8_t stx;  
extern uint8_t etx;  
//extern uint8_t cmd_E;

extern int ledState;
extern int ledState1;

void device_exist_check(uint8_t address[]);
void E_CMD_01_99(uint8_t STX, uint8_t address[], uint8_t cmd, uint8_t ETX);

#endif