#include "r_cg_button_handling.h"
#include "r_cg_lcd.h"
#include "r_cg_macrodriver.h"

// Constants
#define DEBOUNCE_THRESHOLD 10
#define MODE_0 0

// Button Indexes
#define BUTTON_UP 0
#define BUTTON_DOWN 1
#define BUTTON_SELECT 2

// Define the number of modes
#define NUM_MODES (sizeof(modeNames) / sizeof(modeNames[0]))

const char *modeNames[] = {
    "MODE 0: Params",
    "MODE 1: IU TEST",
    "MODE 2: IU ADD",
    "MODE 3: Vol CHK",
    "MODE 4: T-MON",
    "MODE 5: T-SUB",
    "MODE 6: Line CHK"
};

// Global Variables
volatile uint8_t currentMode = MODE_0;
volatile uint8_t modeContext = 0;                // 0: Home, 1: Sub-tab
volatile uint8_t selectedOption = 0;             // Tracks selected option in sub-tabs
volatile uint8_t currentTab = 0;                 // Tab navigation
volatile uint8_t currentTab4 = 0;                // Additional tab navigation for MODE_4

// Button states and debounce
static uint8_t debounceCounters[3] = {0, 0, 0};
static uint8_t buttonStates[3] = {1, 1, 1};      // Initial state (not pressed)
static uint8_t currentStates[3];


// Initialize buttons
void initializeButtons(void) {
    uint8_t i;	
    // Configure pins as inputs with pull-up
    P12 |= (1 << 0); // Enable pull-up for P12.0
    P14 |= (1 << 0) | (1 << 1);                  // Enable pull-ups for P14.0 and P14.1

// Initialize button states
    for (i = 0; i < 2; i++) {
        debounceCounters[i] = 0;
        buttonStates[i] = 1; // Not pressed
    }
    buttonStates[2] = 0;
// Ensure the initial mode and context are set
    currentMode = MODE_0;
    modeContext = 0;
    selectedOption = 0;
    
    
// Display the initial screen
    displayScreen();
    delay(50); 
}

// Process button states (debounced)
void processButtons(void) {
    uint8_t i;	
    currentStates[BUTTON_UP] = !(P14 & (1 << 1));   // Check UP button
    currentStates[BUTTON_DOWN] = !(P14 & (1 << 0)); // Check DOWN button
    currentStates[BUTTON_SELECT] = !(P12 & (1 << 0)); // Check SELECT button

for (i = 0; i < 3; i++) {
        if (currentStates[i] != buttonStates[i]) {
            debounceCounters[i]++;
            if (debounceCounters[i] >= DEBOUNCE_THRESHOLD) {
                buttonStates[i] = currentStates[i];
                debounceCounters[i] = 0;
                if (buttonStates[i] == 0) { // Button pressed
                    handleButtonPress(i);
                }
            }
        } else {
            debounceCounters[i] = 0;
        }
    }
}

// Handle button presses
void handleButtonPress(uint8_t button) {
    switch (modeContext) {
        case 0: // Home screen
            if (button == BUTTON_UP) {
                currentMode = (currentMode + 1) % NUM_MODES; // Cycle through modes
            } else if (button == BUTTON_DOWN) {
                currentMode = (currentMode + NUM_MODES - 1) % NUM_MODES; // Reverse cycle
            } else if (button == BUTTON_SELECT) {
                modeContext = 1; // Enter sub-tab
                selectedOption = 0; // Reset option
            }
            break;

case 1: // Sub-tab navigation
            if (button == BUTTON_UP) {
                if (currentMode == MODE_0) {
                    selectedOption = (selectedOption + 1) % 4; // MODE 0 has 4 sub-tabs
                } else if (currentMode == 3) {
                    currentTab = (currentTab + 1) % 3; // MODE 3 has 3 tabs
                } else if (currentMode == 4) {
                    currentTab4 = (currentTab4 + 1) % 3; // MODE 4 has 3 tabs
                }
            } else if (button == BUTTON_DOWN) {
                if (currentMode == MODE_0) {
                    selectedOption = (selectedOption + 3) % 4; // Reverse navigation
                } else if (currentMode == 3) {
                    currentTab = (currentTab + 2) % 3; // Reverse navigation
                } else if (currentMode == 4) {
                    currentTab4 = (currentTab4 + 2) % 3; // Reverse navigation
                }
            } else if (button == BUTTON_SELECT) {
                if (currentMode == MODE_0 && selectedOption == 3) {
                    // Exit selected in MODE 0
                    modeContext = 0;
                } else {
                    modeContext = 0; // For other tabs, return home
                }
            }
            break;
    }
    displayScreen(); // Update the screen
}

uint8_t versionDisplayed = 0;
// Display the current screen
void displayScreen(void) {    
if (!versionDisplayed) {
	//Display Starting
        Clr_LCD4();
        MoveHome_LCD4();                     //First Line
        Puts_LCD4("IU Version 1.0");
        delay_ms(400000);
        versionDisplayed = 1;
    } else if(modeContext == 0) {
	Clr_LCD4();
        MoveHome_LCD4(); 
	Puts_LCD4(modeNames[currentMode]);   // Display the mode name
	
	Set_DDA(0x40);  		     // Second line
	Puts_LCD4(" PUSH SEL START ");       // Display Mode 2 without cursor
	
    } else if (modeContext == 1) {
	Clr_LCD4();
        MoveHome_LCD4();     
        if (currentMode == MODE_0) {         //Params
            // MODE 0 Sub-tab options
            switch (selectedOption) {
                case 0: Puts_LCD4("bps: 38400");Set_DDA(0x40); Puts_LCD4(" PUSH SEL START "); 
	 		break;
                case 1: Puts_LCD4("24V: 10V"); Set_DDA(0x40); Puts_LCD4(" PUSH SEL START "); 
			break;
                case 2: Puts_LCD4("LINE: 1.0V"); Set_DDA(0x40); Puts_LCD4(" PUSH SEL START "); 
			break;
                case 3: Puts_LCD4("Exit"); Set_DDA(0x40); Puts_LCD4(" PUSH SEL START "); 
			break;
            }
        } else if (currentMode == 1) {       // IU ADD
		Puts_LCD4("Start IU ADD");
	}else if (currentMode == 2) {	     // IU TEST
		Puts_LCD4("Start IU TEST"); 
	}else if (currentMode == 3) {
            // MODE 3 Tab navigation
            switch (currentTab) {
                case 0: Puts_LCD4("Tab 1 Mode 3"); break;
                case 1: Puts_LCD4("Tab 2 Mode 3"); break;
                case 2: Puts_LCD4("Tab 3 Mode 3"); break;
            }
        } else if (currentMode == 4) {
            // MODE 4 Tab navigation
            switch (currentTab4) {
                case 0: Puts_LCD4("Tab 1 Mode 4"); break;
                case 1: Puts_LCD4("Tab 2 Mode 4"); break;
                case 2: Puts_LCD4("Tab 3 Mode 4"); break;
            }

        } else if (currentMode == MODE_5) {
            // MODE 3 Tab navigation
            switch (currentTab) {
                case 0: Puts_LCD4("Tab 1 Mode 5"); break;
                case 1: Puts_LCD4("Tab 2 Mode 5"); break;
                case 2: Puts_LCD4("Tab 3 Mode 5"); break;
            }
        } else if (currentMode == MODE_6) {
            // MODE 3 Tab navigation
            switch (currentTab) {
                case 0: Puts_LCD4("Tab 1 Mode 6"); break;
                case 1: Puts_LCD4("Tab 2 Mode 6"); break;
                case 2: Puts_LCD4("Tab 3 Mode 6"); break;
            }
        } 
//	    else {
            // Other modes
//            Puts_LCD4("Tab");
//        }
    }
}
