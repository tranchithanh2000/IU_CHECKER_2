#include "r_cg_iu_test.h"
#include "r_cg_macrodriver.h"
#include "r_cg_uart_cmd.h"
#include "r_cg_lcd.h"

void device_exist_check(uint8_t address[]) {
//    uint8_t data_index; 
	
    if (data_index < MAX_DATA_ENTRIES) {
        device_exist[data_index][0] = address[0];            // Optional leading byte for padding
        device_exist[data_index][1] = address[1];    // Tens place in hexadecimal
        data_index++;                                  // Move to the next entry for the next call
    }
}
/*
void E_CMD_01_99(uint8_t STX, uint8_t address[], uint8_t cmd, uint8_t ETX){
//   uint8_t address_E[] = {0x30,0x31};	
     uint8_t calculated_BCC;	
     
   for (tens = 0x30; tens <= 0x39; tens++) {
        for (ones = 0x30; ones <= 0x39; ones++) {
            address[0] = tens;
            address[1] = ones;
   	   // LCD
           Clr_LCD4();  // Clear the screen
	   MoveHome_LCD4();  // Move cursor to home position   
	   Puts_LCD4("Send E 01-99");  // Display Mode 1 with cursor
	   Set_DDA(0x40);  // Move cursor to second line
	   Puts_LCD4("Device : ");  // Display Mode 2 without cursor
	   
	   //Device Number on LCD
	   Set_DDA(0x40);
	   Cursor_RN(9);	
	   Putc_LCD4(address[0]);
	   Set_DDA(0x40);
	   Cursor_RN(10);	
	   Putc_LCD4(address[1]);
	    
            // Send the IU_E_Cmd command with the current address
//            sendCommentEnter(""); sendComment("Send E ");  
//	    sendString(address_E[0]); sendString(address_E[1]);
//	    sendComment(" : ");
	    IU_E_Cmd(STX, address, cmd, ETX);
            P1 &= ~(1 << 0);  // Set P10 for receive (ADM4853)
            delay(4000);
	    
	    delay(100); //adjust time when send E
//	    sendCommentEnter("");
//	     sendCommentEnter("----");
	    /*
	    //Check if Device 65-99 exist
		sendComment("Rx data: ");
		sendCommentEnter(RX_Buffer); 
		sendComment("Rx BCC: ");
		sendString(RX_Data); 
		sendCommentEnter("");

		//Check BCC
		//processReceivedData(6);
		sendComment("Calculated BCC: ");
		calculated_BCC = calculate_bcc(RX_Buffer, 6);
		sendString(calculated_BCC);
		sendCommentEnter(""); 
	   
	       	calculated_BCC = calculate_bcc(RX_Buffer, 6);	
           
//	   Check if exist any device and save it	
	   if(RX_Data == calculated_BCC && RX_Buffer[2] == address[1]){
//	      sendComment("Address: ");
//	      sendString(address_E[0]); sendString(address_E[1]);
//              sendComment("");
	      address_storage(address);
	      
	   }	
	   
		// Reset buffer and variables after processing
	        RX_Index = 0;          // Reset buffer index
	        RX_Buffer[0] = '\0';   // Clear the buffer
		memset(RX_Buffer, 0, sizeof(RX_Buffer));
		calculated_BCC = 0;
		RX_Data = 0;
		
	   
//	    if(!ledState || ledState1) break; 
        }
//	if(!ledState || ledState1) break;
    }
}
*/