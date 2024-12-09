#include "r_cg_uart_cmd.h"
#include "r_cg_macrodriver.h"


#include <stdlib.h>
#include <string.h>

// Init Uart
void init_Uart(){
    // Step 1: Initialize UART0
    R_SAU0_Create();
    
    // Step 2: Start UART0 operation
    R_UART0_Start();
};

// Send Character
void sendString(char comment) {
    // Send the character through UART
    R_UART0_Send((uint8_t *)&comment, 1);  // Send one byte
    
    P1 |= (1 << 0);  // Set P10 for transmit (ADM4853)
    delay(100000);   // Delay to ensure transmission is complete
}

// Send String
void sendComment(const char *comment) {
    int length = 0;
    
    // Calculate the length of the string (excluding null terminator)
    while (comment[length] != '\0') {
        length++;
    }
    
    // Send the string through UART
    R_UART0_Send((uint8_t *)comment, length);  
    
    P1 |= (1 << 0);  // Set P10 for transmit (ADM4853)
    delay(100000);    // Delay to ensure transmission is complete
}

// Send String With Comment
void sendCommentEnter(const char *comment) {
    int length = 0;
    char *buffer;
    uint8_t i; 
    
    // Calculate the length of the string
    while (comment[length] != '\0') {
        length++;
    }

    // Dynamically allocate memory for the comment plus the newline and null terminator
    buffer = (char *)malloc(length + 2);  // One extra for '\n' and one for '\0'
    
    if (buffer == NULL) {
        // Handle memory allocation failure
        return;
    }

    // Copy the comment into the buffer
    for (i = 0; i < length; i++) {
        buffer[i] = comment[i];
    }

    // Add newline and null terminator at the end
    buffer[length] = '\n';  // Add the newline character
    buffer[length + 1] = '\0';  // Null terminator

    // Send the string with newline through UART
    R_UART0_Send((uint8_t *)buffer, length + 1);  // +1 for the newline character

    P1 |= (1 << 0);  // Set P10 for transmit (ADM4853)
    delay(100000);    // Delay to ensure transmission is complete

    // Free the allocated memory
    free(buffer);
}

uint8_t calculate_bcc(const uint8_t *buffer, uint8_t length) {
    uint8_t bcc = 0;
    uint8_t i; 
    // Ensure that we only calculate for the first 'length' bytes
    for (i = 0; i < length; i++) {
        bcc ^= buffer[i]; // Perform XOR operation
    }

    return bcc; // Return the calculated BCC
}
/*
// Function to send formatted data through UART
void sendFormattedUART(const char* format, int length) {
    R_UART0_Send(format, length);  // Send the string through UART
    P1 |= (1 << 0);  // Set P10 for transmit (ADM4853)
    delay(3000);    // Delay
}

// Function to send formatted data through UART
void sendFormattedUART_Data(const char* format, int length) {
    R_UART0_Send(format, length);  // Send the string through UART
    P1 |= (1 << 0);  // Set P10 for transmit (ADM4853)
    delay(6000);    // Delay
}
*/
// BBC calculation (NR and GE)
uint8_t calculateBCC_adj_addres(uint8_t stx, uint8_t question1, uint8_t question2, uint8_t cmd1, uint8_t cmd2, uint8_t etx) {
    return stx ^ question1 ^ question2 ^ cmd1 ^ cmd2 ^ etx;
}

// BBC calculation (GS)
uint8_t calculateBCC_GS(uint8_t stx, uint8_t question1, uint8_t question2, uint8_t cmd1, uint8_t cmd2, uint8_t data1, uint8_t data2, uint8_t etx) {
    return stx ^ question1 ^ question2 ^ cmd1 ^ cmd2 ^ data1 ^ data2 ^ etx;
}

// BBC calculation (B ON)
uint8_t calculateBCC_B(uint8_t stx, uint8_t b, uint8_t address1, uint8_t address2, uint8_t cmd, uint8_t light1, uint8_t light2, uint8_t etx) {
    return stx ^ b ^ address1 ^ address2 ^ cmd ^ light1 ^ light2 ^ etx;
}

// BBC calculation (B OFF)
uint8_t calculateBCC_B_OFF(uint8_t stx, uint8_t b, uint8_t address1, uint8_t address2, uint8_t cmd1, uint8_t cmd2, uint8_t offData, uint8_t etx) {
    return stx ^ b ^ address1 ^ address2 ^ cmd1 ^ cmd2 ^ offData ^ etx;
}

// BBC calculation (E)
uint8_t calculateBCC_E(uint8_t stx, uint8_t address1, uint8_t address2, uint8_t cmd, uint8_t etx) {
    return stx ^ address1 ^ address2 ^ cmd ^ etx;
}

// BBC calculation (A)
uint8_t calculateBCC_A(uint8_t stx, uint8_t address1, uint8_t address2, uint8_t cmd, uint8_t data[], uint8_t etx) {
    uint8_t bcc = stx ^ address1 ^ address2 ^ cmd;
    uint8_t i; 
    
    for (i = 0; i < 6; i++) {  // XOR each byte in the data array
        bcc ^= data[i];
    }
    bcc ^= etx;
    return bcc;
}

// Send NR and GE to UART
void IU_Adj_Adres(uint8_t stx, uint8_t question[], uint8_t cmd[], uint8_t etx) {
    // Calculate BCC based on the given bytes
    uint8_t bcc = calculateBCC_adj_addres(stx, question[0], question[1], cmd[0], cmd[1], etx);
    
    // Prepare the message to send
    char message[7] = {stx, question[0], question[1], cmd[0], cmd[1], etx, bcc};
    
    // Send the formatted message through UART
    sendFormattedUART(message, 7);
//    P1 &= ~(1 << 0);  // Set P10 for receive (ADM4853)

    
}

// Send GS to UART
void IU_Adj_Adres_GS(uint8_t stx, uint8_t question[], uint8_t cmd[], uint8_t data[], uint8_t etx) {
    // Calculate BCC based on the given bytes
    uint8_t bcc1 = calculateBCC_GS(stx, question[0], question[1], cmd[0], cmd[1], data[0], data[1], etx);
    
    // Prepare the message to send
    char message[9] = {stx, question[0], question[1], cmd[0], cmd[1], data[0], data[1], etx, bcc1};
    
    // Send the formatted message through UART
    sendFormattedUART(message, 9);
    delay(100000);

}

// Send B ON to UART
void IU_B_Cmd(uint8_t stx, uint8_t b, uint8_t address[], uint8_t cmd, uint8_t light[], uint8_t etx) {
    // Calculate BCC based on the given bytes
    uint8_t bcc = calculateBCC_B(stx, b, address[0], address[1], cmd, light[0], light[1], etx);
    
    // Prepare the message to send
    char message[9] = {stx, b, address[0], address[1], cmd, light[0], light[1], etx, bcc};

    // Send the formatted message through UART
    sendFormattedUART(message, 9);
    delay(3000);
}

// Send B OFF to UART
void IU_B_OFF_Cmd(uint8_t stx, uint8_t b, uint8_t address[], uint8_t cmd[], uint8_t offData, uint8_t etx) {
    // Calculate BCC based on the given bytes
    uint8_t bcc = calculateBCC_B_OFF(stx, b, address[0], address[1], cmd[0], cmd[1], offData, etx);
    
    // Prepare the message to send
    char message[9] = {stx, b, address[0], address[1], cmd[0], cmd[1], offData, etx, bcc};

    // Send the formatted message through UART
    sendFormattedUART(message, 9);
    delay(10000);  // Adjust delay as needed
}

// Send E to UART
void IU_E_Cmd(uint8_t stx, uint8_t address[], uint8_t cmd, uint8_t etx) {
    // Calculate BCC for the E command
    uint8_t bcc = calculateBCC_E(stx, address[0], address[1], cmd, etx);
    
    // Prepare the message to send
    char message[6] = {stx, address[0], address[1], cmd, etx, bcc};
    
    // Send the formatted message through UART
    sendFormattedUART(message, 6);
}

// Send A to UART
void IU_A_Cmd(uint8_t stx, uint8_t address[], uint8_t cmd, uint8_t data[], uint8_t etx) {
    // Calculate BCC for the A command
    uint8_t bcc = calculateBCC_A(stx, address[0], address[1], cmd, data, etx);
    
    // Prepare the message to send
    char message[12] = {stx, address[0], address[1], cmd,
                        data[0], data[1], data[2], data[3], data[4], data[5],
                        etx, bcc};
    
    // Send the formatted message through UART
    sendFormattedUART(message, 12);
    delay(3000);
}


// Function to increment the address in ASCII hex format 
void incrementAddress(uint8_t address[]) {
    // Convert ASCII hex to a numeric value
    uint8_t highNibble = address[0] - 0x30;
    uint8_t lowNibble = address[1] - 0x30;
    uint8_t value = highNibble * 10 + lowNibble;

    // Increment the value, wrap around to 0 after reaching 99
    value = (value + 1) % 100;

    // Convert back to ASCII hex representation
    address[0] = (value / 10) + 0x30;  // Tens place
    address[1] = (value % 10) + 0x30;  // Ones place
}


// Storage Array After Scan
void address_storage(uint8_t address[]) {
    // Store the converted value in hexadecimal format in data_storage
    if (data_index < MAX_DATA_ENTRIES) {
        data_storage[data_index][0] = address[0];                // Optional leading byte for padding
        data_storage[data_index][1] = address[1];               // Tens place in hexadecimal
//        data_storage[data_index][2] = (value % 10);           // Ones place in hexadecimal
        data_index++;                                           // Move to the next entry for the next call
    }
}

// Storage Array Verify Again
void address_storage_check(uint8_t address[]) {  
    
    if (data_index < MAX_DATA_ENTRIES) {
        data_storage_check[data_index1][0] = address[0];        // Optional leading byte for padding
        data_storage_check[data_index1][1] = address[1];        // Tens place in hexadecimal
//        data_storage[data_index][2] = (value % 10);           // Ones place in hexadecimal
        data_index1++;                                          // Move to the next entry for the next call
    }
}

