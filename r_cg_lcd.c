#include "r_cg_lcd.h"
#include "r_cg_macrodriver.h"

//====================LCD Config======================
void delay_ms(unsigned int ms) {
    volatile unsigned int i;
    while (ms--) {
        for (i = 0; i < 1000; i++);  
    }
}

// Send a command to the LCD in 8-bit mode
void Cmd_LCD4(unsigned char cmd) {
    RS = 0;  // Command mode
    RW = 0;  // Write mode
    E = 1;

    // Set data pins (8-bit command)
    D80 = (cmd >> 0) & 1;
    D81 = (cmd >> 1) & 1;
    D82 = (cmd >> 2) & 1;
    D83 = (cmd >> 3) & 1;
    D84 = (cmd >> 4) & 1;
    D85 = (cmd >> 5) & 1;
    D86 = (cmd >> 6) & 1;
    D87 = (cmd >> 7) & 1;

    E = 0;
    delay_ms(2);
}

// Send data (character) to the LCD in 8-bit mode
void lcd_data(unsigned char data) {
    RS = 1;  // Data mode
    RW = 0;  // Write mode
    E = 1;

    // Set data pins (8-bit data)
    D80 = (data >> 0) & 1;
    D81 = (data >> 1) & 1;
    D82 = (data >> 2) & 1;
    D83 = (data >> 3) & 1;
    D84 = (data >> 4) & 1;
    D85 = (data >> 5) & 1;
    D86 = (data >> 6) & 1;
    D87 = (data >> 7) & 1;

    E = 0;
    delay_ms(2);
}

// Clear the LCD display
void Clr_LCD4(void) {
    Cmd_LCD4(0x01);    // Clear display command
    delay_ms(2);          // Wait for the command to complete
}

// Initialize the LCD
void Init_LCD3(void) {
    delay_ms(20);          // Wait for LCD to stabilize after power-on
    Cmd_LCD4(0x38);     // Function set: 8-bit, 2-line, 5x8 dots
    Cmd_LCD4(0x0C);     // Display ON, Cursor OFF
    Cmd_LCD4(0x06);     // Entry mode set, cursor move direction
    Clr_LCD4();            // Clear display
    delay_ms(1000);
}

// Home position
void MoveHome_LCD4(void) {
    Cmd_LCD4(0x02);    // Return home command
    delay_ms(2);          // Wait for the command to complete
}

// Send character 
void Putc_LCD4(unsigned char character) {
    lcd_data(character);  // Send character data to the LCD
}

// Send stirng
void Puts_LCD4(unsigned char *str) {
    while (*str != '\0') {  // Loop until the end of the string
        Putc_LCD4(*str);    // Output each character
        str++;              // Move to the next character in the string
    }
}

void Shift_L(void) {
    Cmd_LCD4(0x18);   // Command for shifting display left by one character
}

void Shift_R(void) {
    Cmd_LCD4(0x1C);   // Command for shifting display right by one character
}

void Shift_LN(int n) {
    uint8_t i;  	
    for (i = 0; i < n; i++) {
        Shift_L();    // Shift left one character `n` times
    }
}

void Shift_RN(int n) {
    uint8_t i; 
    for (i = 0; i < n; i++) {
        Shift_R();    // Shift right one character `n` times
    }
}

void Cursor_L(void) {
    Cmd_LCD4(0x10);   // Command for moving cursor left by one character
}

void Cursor_R(void) {
    Cmd_LCD4(0x14);   // Command for moving cursor right by one character
}

void Cursor_LN(int n) {
    int i; 	
    for (i = 0; i < n; i++) {
        Cursor_L();   // Move cursor left one character `n` times
    }
}

void Cursor_RN(int n) {
    int i; 
    for (i = 0; i < n; i++) {
        Cursor_R();   // Move cursor right one character `n` times
    }
}

void Set_DDA(unsigned char address) {
    Cmd_LCD4(0x80 | address);  // Command to set DDRAM address
}