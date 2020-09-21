// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//CMPE13 Support Library
#include "BOARD.h"



// User libraries
#include "rpn.h"

// **** Set macros and preprocessor directives ****
// The lab calls for 60 characters of user input
#define MAX_INPUT_LENGTH 60

int main() {
    BOARD_Init();

    //What follows is starter code.  You will need to modify it!

    char rpn_sentence[MAX_INPUT_LENGTH + 2];
    double result;
    int error;
    printf("Welcome to ARPAL'S RPN calculator.  Compiled on %s %s\n", __DATE__, __TIME__);
    while (1) {

        printf("Enter floats and + - / * in RPN format:\n");
        fflush(stdin);
        fgets(rpn_sentence, sizeof (rpn_sentence), stdin);
        // ProcessBackspaces(&rpn_sentence);
        if (strlen(rpn_sentence) > MAX_INPUT_LENGTH) {
            printf("Too many characters");
            //fflush(stdin);
        } else {
            error = RPN_Evaluate(rpn_sentence, &result);
            if (!error) {
                printf("Result: %f\n", result);
            }
            if (error == RPN_ERROR_STACK_OVERFLOW) {
                printf("No more room on stack.\n");
            }
            if (error == RPN_ERROR_STACK_UNDERFLOW) {
                printf("Not enough operands before operator.\n");
            }
            if (error == RPN_ERROR_INVALID_TOKEN) {
                printf("Invalid character in RPN string.\n");
            }
            if (error == RPN_ERROR_DIVIDE_BY_ZERO) {
                printf("Divide by zero error\n");
            }
            if (error == RPN_ERROR_TOO_FEW_ITEMS_REMAIN) {
                printf("Invalid RPN calculation: less than one item in stack.\n");
            }
            if (error == RPN_ERROR_TOO_MANY_ITEMS_REMAIN) {
                printf("Invalid RPN calculation: more than one item in stack.\n");
            }

        }

    }

    while (1);
}
