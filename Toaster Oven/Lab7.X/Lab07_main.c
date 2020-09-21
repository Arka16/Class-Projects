// **** Include libraries here ****
// Standard libraries
#include <stdio.h>

//CSE13E Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <sys/attribs.h>
#include "Oled.h"
#include "Leds.h"
#include "Buttons.h"
#include "Ascii.h"
#include "OledDriver.h"
#include "Adc.h"

#include <xc.h>
#include <sys/attribs.h>

// **** Set any macros or preprocessor directives here ****
// Set a macro for resetting the timer, makes the code a little clearer.
#define TIMER_2HZ_RESET() (TMR1 = 0)
//#define CorrectedTime FRT/5
#define TIMER_2HZ_RESET()(TMR1 = 0)
// **** Set any local typedefs here ****

//States of Oven

typedef enum {
    SETUP, SELECTOR_CHANGE_PENDING, COOKING, RESET_PENDING, ALERT
} OvenState;

typedef enum {
    BAKE, BROIL, TOAST //Modes of Oven
} OvenMode;
//Controls whether Oven is on or off

typedef enum {
    ON, OFF
} Switch;
//Settings of Oven

typedef enum {
    TIME, TEMP
} Settings;

typedef struct { //Oven properties
    OvenState state;
    OvenMode mode;
    //add more members to this struct
    int CookTime; //time when Oven is in cook mode
    int Temperature; //Temperature of oven
    Settings settings; //switches between Time and temp

} OvenData;

OvenData Oven = {}; //new Oven

// **** Declare any datatypes here ****


// **** Define any module-level, global, or external variables here ****
static int cooktime; //Time to count down 
static int FRT, starttime, elapsedTime; //keeps track of events
//static char TIMER_TICK = FALSE;
static char ADC_flag;
static char TIMER_TICK = FALSE;
static int BakeTemp;
static char FRT_flag = FALSE;
Switch CurrentState = OFF;
uint8_t ButtonEvents;
uint16_t LED_StartTime;
#define LONG_PRESS  5 // 1 second
#define CorrectTime  (cooktime / 5) //converts hertz to seconds
#define STARTTIME 1   //minimum time
#define STARTTEMP 300 //minimun temp


#define LED8 0xFF
#define LED7 0xFE //11111110 , turns last led off
#define LED6 0xFC //11111100 , turns second last led off
#define LED5 0xF8//turns third led off
#define LED4 0xF0
#define LED3 0xE0
#define LED2 0xC0
#define LED1 0x80
#define LED0 0x00


#define TOTAL_LEDS 8
#define SEVEN_LEDS 7
#define SIX_LEDS 6
#define FIVE_LEDS 5
#define FOUR_LEDS 4
#define THREE_LEDS 3
#define TWO_LEDS 2
#define ONE_LED 1

#define CONVERT 60
#define SecondsToFRT(s) s^5
#define charX8(c) c c c c c

#define invert 4
#define normal 2
#define zero 0


char OvenDisplay[OLED_NUM_LINE][OLED_CHARS_PER_LINE];

// **** Put any helper functions here ****

void ledbar(void) { //Helper function for led bar
    if (CorrectTime == (((Oven.CookTime * SEVEN_LEDS) / TOTAL_LEDS))) {
        LEDS_SET(LED7); //determines how many leds will remain on
    }
    if (CorrectTime == (((Oven.CookTime * SIX_LEDS) / TOTAL_LEDS))) {
        LEDS_SET(LED6);
    }
    if (CorrectTime == (((Oven.CookTime * FIVE_LEDS) / TOTAL_LEDS))) {
        LEDS_SET(LED5);
    }
    if (CorrectTime == (((Oven.CookTime * FOUR_LEDS) / TOTAL_LEDS))) {
        LEDS_SET(LED4);
    }
    if (CorrectTime == (((Oven.CookTime * THREE_LEDS) / TOTAL_LEDS))) {
        LEDS_SET(LED3);
    }
    if (CorrectTime == (((Oven.CookTime * TWO_LEDS) / TOTAL_LEDS))) {
        LEDS_SET(LED2);
    }
    if (CorrectTime == (((Oven.CookTime * ONE_LED) / TOTAL_LEDS))) {
        LEDS_SET(LED1);
    }
    if (CorrectTime == (((Oven.CookTime * LED0) / TOTAL_LEDS))) {
        LEDS_SET(LED1);
    }
}

/*This function will update your OLED to reflect the state .*/
void updateOvenOLED(void) {
    OledClear(OLED_COLOR_BLACK); //clears Oled and prevents overlap
    int Min, Sec;
    if (CurrentState == OFF) {
        Min = Oven.CookTime / CONVERT; //converts the total #secs to min:sec
        Sec = Oven.CookTime % CONVERT;
    } else {
        Min = CorrectTime / CONVERT;
        Sec = CorrectTime % CONVERT;
    }
    /*Prints appropiate text/image on OLED depending on selected properties*/
    if (Oven.mode == BAKE) {
        if (CurrentState == OFF) {
            sprintf(OvenDisplay[0], "|%s|  Mode: Bake",
                    charX8(OVEN_TOP_OFF));
            sprintf(OvenDisplay[3], "|%s|",
                    charX8(OVEN_BOTTOM_OFF));
        } else if (CurrentState == ON) {
            sprintf(OvenDisplay[0], "|%s|  Mode: Bake",
                    charX8(OVEN_TOP_ON));
            sprintf(OvenDisplay[3], "|%s|",
                    charX8(OVEN_BOTTOM_ON));
        }
        if (Oven.settings == TIME && CurrentState == OFF) {
            sprintf(OvenDisplay[1], "|%s| >Time: %d:%.2d ",
                    charX8(" "), Min, Sec);
            sprintf(OvenDisplay[2], "|%s|  Temp: %d%sF", charX8("-"),
                    Oven.Temperature, DEGREE_SYMBOL);
        } else if (Oven.settings == TEMP && CurrentState == OFF) {
            sprintf(OvenDisplay[1], "|%s|  Time: %d:%.2d ",
                    charX8(" "), Min, Sec);
            sprintf(OvenDisplay[2], "|%s| >Temp: %d%sF", charX8("-"),
                    Oven.Temperature, DEGREE_SYMBOL);
        }
        if (Oven.settings == TIME && CurrentState == ON) {
            sprintf(OvenDisplay[1], "|%s|  Time: %d:%.2d ",
                    charX8(" "), Min, Sec);
            sprintf(OvenDisplay[2], "|%s|  Temp: %d%sF", charX8("-"),
                    Oven.Temperature, DEGREE_SYMBOL);
        } else if (Oven.settings == TEMP && CurrentState == ON) {
            sprintf(OvenDisplay[1], "|%s|  Time: %d:%.2d ",
                    charX8(" "), Min, Sec);
            sprintf(OvenDisplay[2], "|%s|  Temp: %d%sF", charX8("-"),
                    Oven.Temperature, DEGREE_SYMBOL);
        }
        if (Oven.state == ALERT) {
            sprintf(OvenDisplay[1], "|%s|  Time: %d:%.2d ",
                    charX8(" "), Min, Sec);
            sprintf(OvenDisplay[2], "|%s|  Temp: %d%sF", charX8("-"),
                    Oven.Temperature, DEGREE_SYMBOL);
        }


    } else if (Oven.mode == TOAST) {
        if (CurrentState == OFF) {
            sprintf(OvenDisplay[0], "|%s|  Mode: Toast", charX8(OVEN_TOP_OFF));
            sprintf(OvenDisplay[3], "|%s|", charX8(OVEN_BOTTOM_OFF));
        } else if (CurrentState == ON) {
            sprintf(OvenDisplay[0], "|%s|  Mode: Toast", charX8(OVEN_TOP_OFF));
            sprintf(OvenDisplay[3], "|%s|", charX8(OVEN_BOTTOM_ON));
        }
        sprintf(OvenDisplay[1], "|%s|  Time: %d:%.2d ", charX8(" "), Min, Sec);
        sprintf(OvenDisplay[2], "|%s|", charX8("-")); //Exclude TEMP
    } else if (Oven.mode == BROIL) {
        if (CurrentState == OFF) {
            Oven.Temperature = 500;
            sprintf(OvenDisplay[0], "|%s|  Mode: Broil", charX8(OVEN_TOP_OFF));
            sprintf(OvenDisplay[3], "|%s|", charX8(OVEN_BOTTOM_OFF));
        } else if (CurrentState == ON) {
            sprintf(OvenDisplay[0], "|%s|  Mode: Broil", charX8(OVEN_TOP_ON));
            sprintf(OvenDisplay[3], "|%s|", charX8(OVEN_BOTTOM_OFF));
        }
        sprintf(OvenDisplay[1], "|%s|  Time: %d:%.2d ", charX8(" "), Min, Sec);
        sprintf(OvenDisplay[2], "|%s|  Temp: %d%sF", charX8("-"),
                Oven.Temperature, DEGREE_SYMBOL);
    }
    char finalString[OLED_NUM_LINE * (OLED_CHARS_PER_LINE + 1)];
    sprintf(finalString, "%s\n%s\n%s\n%s", OvenDisplay[0], OvenDisplay[1],
            OvenDisplay[2], OvenDisplay[3]);

    //update OLED here

    OledDrawString(finalString);
    OledUpdate();
    //OledClear(NULL); //clears Oled and prevents overlap


}

/*This function will execute your state machine.  
 * It should ONLY run if an event flag has been set.*/
void runOvenSM(void) {

    //write your SM logic here.
    switch (Oven.state) {
        case SETUP:
            if (ButtonEvents & BUTTON_EVENT_3DOWN) {
                //cooktime = Oven.initialCookTime * 5;
                starttime = FRT; //store FRT
                Oven.state = SELECTOR_CHANGE_PENDING;

            } else {
                if (Oven.settings == TIME) { //if user wants to configure time
                    //Range time 1-256 secs
                    Oven.CookTime = (AdcRead() / 4) + STARTTIME;


                } else { //if user wants to configure temperaturea
                    Oven.Temperature = (AdcRead() / 4) + STARTTEMP;
                }
                updateOvenOLED(); //update Oled

            }
            if (ButtonEvents == BUTTON_EVENT_4DOWN) {
                cooktime = (Oven.CookTime) * 5; // Store initial time 
                starttime = FRT; //stores FRT
                updateOvenOLED(); //update Oled
                CurrentState = ON;
                LEDS_SET(LED8); //update LED bar
                Oven.state = COOKING;
                TIMER_TICK = TRUE;
            }
            break;
        case SELECTOR_CHANGE_PENDING:
            //change between modes 
            elapsedTime = FRT - starttime; //stores time elapsed 
            if (ButtonEvents & BUTTON_EVENT_3UP) {
                if (elapsedTime < LONG_PRESS) {
                    if (Oven.mode == BAKE) {
                        BakeTemp = Oven.Temperature; //stores bake temperature
                        Oven.mode = TOAST;
                        Oven.settings = TIME;
                    } else if (Oven.mode == TOAST) {
                        Oven.mode = BROIL;
                        Oven.settings = TIME;
                    } else if (Oven.mode == BROIL) {
                        Oven.mode = BAKE;
                        Oven.Temperature = BakeTemp;
                        //Oven.settings = TIME;
                    }
                    Oven.state = SETUP;
                } else {

                    if (Oven.settings == TIME && Oven.mode == BAKE) {
                        Oven.settings = TEMP;
                    } else if (Oven.settings == TEMP && Oven.mode == BAKE) {
                        Oven.settings = TIME;
                    }
                    Oven.state = SETUP;
                }
            }

            break;
        case COOKING:
            if (TIMER_TICK) { //Counts down as long as TIMERTICK is False
                //continuosly updates elapsed time 
                elapsedTime = FRT - starttime;
                if (cooktime > 0) {
                    updateOvenOLED(); //update OLED
                    ledbar(); //update LED
                } else {
                    LEDS_SET(LED0); //turns of last LED
                    TIMER_TICK = FALSE; //stop timer
                    CurrentState = OFF;
                    Oven.state = ALERT;
                    starttime = FRT;
                    updateOvenOLED(); //update OLED 
                }
            }
            if (ButtonEvents & BUTTON_EVENT_4DOWN) {
                starttime = FRT; //stores FRT
                Oven.state = RESET_PENDING;
            }
            break;
        case RESET_PENDING:
            elapsedTime = FRT - starttime; //stores time elapsed 
            if (ButtonEvents & BUTTON_EVENT_4UP) {
                Oven.state = COOKING;
            }
            if (TIMER_TICK) {
                if (elapsedTime >= LONG_PRESS) {
                    LEDS_SET(LED0); //turns of last LED
                    TIMER_TICK = FALSE; //stop timer
                    CurrentState = OFF;
                    Oven.state = SETUP;
                    updateOvenOLED(); //update OLED 
                } else {
                    updateOvenOLED(); //update OLED
                    ledbar(); //update LED
                }
            }
            break;
        case ALERT:
            elapsedTime = FRT - starttime;
            if (elapsedTime % 2 == 0
                    && elapsedTime % 4 != 0) { //normal every 2 hz but not 4
                OledSetDisplayNormal();
            } else if (elapsedTime % 4 == 0) { //invert every 4 hz
                OledSetDisplayInverted();
            } else if (elapsedTime == 0) { //starts out inverted
                OledSetDisplayInverted();
            }
            //4up to prevent button down to start timer again            
            if (ButtonEvents & BUTTON_EVENT_4UP) {
                OledSetDisplayNormal();
                Oven.settings = TIME;
                Oven.state = SETUP;

            }
            break;


    }


}

int main() {
    BOARD_Init();
    OledInit();
    //initalize timers and timer ISRs:
    // <editor-fold defaultstate="collapsed" desc="TIMER SETUP">

    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a .01s timer.

    T2CON = 0; // everything should be off
    T2CONbits.TCKPS = 0b100; // 1:16 prescaler
    PR2 = BOARD_GetPBClock() / 16 / 100; // interrupt at .5s intervals
    T2CONbits.ON = 1; // turn the timer on

    // Set up the timer interrupt with a priority of 4.
    IFS0bits.T2IF = 0; //clear the interrupt flag before configuring
    IPC2bits.T2IP = 4; // priority of  4
    IPC2bits.T2IS = 0; // subpriority of 0 arbitrarily 
    IEC0bits.T2IE = 1; // turn the interrupt on

    // Configure Timer 3 using PBCLK as input. We configure it using a 1:256 prescaler, so each timer
    // tick is actually at F_PB / 256 Hz, so setting PR3 to F_PB / 256 / 5 yields a .2s timer.

    T3CON = 0; // everything should be off
    T3CONbits.TCKPS = 0b111; // 1:256 prescaler
    PR3 = BOARD_GetPBClock() / 256 / 5; // interrupt at .5s intervals
    T3CONbits.ON = 1; // turn the timer on

    // Set up the timer interrupt with a priority of 4.
    IFS0bits.T3IF = 0; //clear the interrupt flag before configuring
    IPC3bits.T3IP = 4; // priority of  4
    IPC3bits.T3IS = 0; // subpriority of 0 arbitrarily 
    IEC0bits.T3IE = 1; // turn the interrupt on;

    // </editor-fold>

    //div = CorrectTime / 8;
    printf("Welcome to ARPAL's Lab07 (Toaster Oven).  "
            "Compiled on %s %s.", __TIME__, __DATE__);
    //initialize state machine (and anything else you need to init) here
    Oven.mode = BAKE; //initialize mode to BAKE
    Oven.state = SETUP; //initialize state to SETUP
    Oven.Temperature = 350; //sets default baking temp
    Oven.CookTime = 1; //initial cook time
    CurrentState = OFF; //State starts of as OFF
    Oven.settings = TIME; //pointer for BAKE points at TIME first

    ButtonsInit();
    LEDS_INIT();
    AdcInit();

#ifdef Test

#endif

    while (1) {
        //if button event
        //runOvenSM();
        if (ButtonEvents) {
            runOvenSM();
            ButtonEvents = BUTTON_EVENT_NONE; //clear Button event
        }
        //if adc event
        if (ADC_flag) {
            runOvenSM();
            ADC_flag = FALSE; //clear ADC event
        }
        if (FRT_flag) {
            runOvenSM();
            FRT_flag = FALSE; //clear FRT flag
        }
    }

}

/*The 5hz timer is used to update the free-running timer and to generate 
 * TIMER_TICK events*/
void __ISR(_TIMER_3_VECTOR, ipl4auto) TimerInterrupt5Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 12;
    if (TIMER_TICK) { //Decrements cook time only if oven is cooking
        cooktime--; //decrement timer in oven
    }
    FRT++;
    FRT_flag = TRUE; //resets FRT flag

    //Keeps track of total time 


    //add event-checking code here
}

/*The 100hz timer is used to check for button and ADC events*/
void __ISR(_TIMER_2_VECTOR, ipl4auto) TimerInterrupt100Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;
    ButtonEvents = ButtonsCheckEvents(); //reset button event
    ADC_flag = TRUE; //reset ADC event

    //add event-checking code here
}
