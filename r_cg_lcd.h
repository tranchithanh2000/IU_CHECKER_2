#ifndef LCD_H
#define LCD_H


// Define Data Pins
#define D80 P7_bit.no0
#define D81 P7_bit.no1
#define D82 P7_bit.no2
#define D83 P7_bit.no3
#define D84 P7_bit.no4
#define D85 P7_bit.no5
#define D86 P7_bit.no6
#define D87 P7_bit.no7

// Define Control Pins
#define RS P5_bit.no0
#define E  P5_bit.no1
#define RW P5_bit.no2
#define K  P5_bit.no3

void delay_ms(unsigned int ms);

// Send a command to the LCD in 8-bit mode
void Cmd_LCD4(unsigned char cmd);

// Send data (character) to the LCD in 8-bit mode
void lcd_data(unsigned char data);

// Clear the LCD display
void Clr_LCD4(void);

// Initialize the LCD
void Init_LCD3(void);

// Home position
void MoveHome_LCD4(void);

// Send character 
void Putc_LCD4(unsigned char character);

// Send string
void Puts_LCD4(unsigned char *str);

void Shift_L(void);

void Shift_R(void);

void Shift_LN(int n);

void Shift_RN(int n);

void Cursor_L(void);

void Cursor_R(void);

void Cursor_LN(int n);

void Cursor_RN(int n);

void Set_DDA(unsigned char address);


#endif