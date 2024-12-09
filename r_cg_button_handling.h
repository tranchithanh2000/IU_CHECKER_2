#ifndef R_CG_BUTTON_HANDLING_H
#define R_CG_BUTTON_HANDLING_H
#include <stdint.h>
// Button Identifiers
#define BUTTON_UP 0
#define BUTTON_DOWN 1
#define BUTTON_SELECT 2
// Modes
#define MODE_0 0
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3
#define MODE_4 4
#define MODE_5 5
#define MODE_6 6
// Function Prototypes
void initializeButtons(void);
void processButtons(void);
void handleButtonPress(uint8_t button);
void displayScreen(void);


#endif // R_CG_BUTTON_HANDLING_H