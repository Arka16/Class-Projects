#include "rpn.h"
#include <string.h>
#include "stack.h"

/* RPN_Evaluate() parses and evaluates a string that contains 
 * a valid Reverse Polish Notation string (no newlines!)  
 * @param:  rpn_string - a string in polish notation.  Tokens must be either 
 *          arithmetic operators or numbers.
 * @param:  result - a pointer to a double that will be modified to contain
 *          the return value of the rpn expression.
 * @return: error - if the rpn expression is invalid, 
 *          an appropriate rpn_error value is returned.
 * 
 * RPN_Evaluate supports the following basic arithmetic operations:
 *   + : addition
 *   - : subtraction
 *   * : multiplication
 *   / : division
 * Numeric tokens can be positive or negative, and can be integers or 
 * decimal floats.  RPN_Evaluate should be able to handle strings of 
 * at least 255 length.
 * */
int RPN_Evaluate(char * rpn_string, double * result) {
    struct Stack stack = {};
    StackInit(&stack); //call stack 
    char *tok = (char*) strtok(rpn_string, " "); //gets first token of the string
    double val_push, val_pop1, val_pop2;
    int count = 0;
    int element = 0;
    while (tok != NULL) {
        if (atof(tok)) { //push if token is a number
            val_push = atof(tok);
            if (!StackIsFull(&stack)) {
                StackPush(&stack, val_push);
            } else {
                return RPN_ERROR_STACK_OVERFLOW; //error if stack full
            }

        } else if (*tok == '0') {
            if (!StackIsFull(&stack)) {
                val_push = 0.0;
                StackPush(&stack, val_push); //push 0 if token is 0 or -0
            } else {
                return RPN_ERROR_STACK_OVERFLOW; //if more than 20items on stack
            }
        } else if (*tok == '+' || *tok == '-' || *tok == '*' || *tok == '/') {
            if (StackIsEmpty(&stack)) {
                return RPN_ERROR_STACK_UNDERFLOW; //nothing to pop
            }
            StackPop(&stack, &val_pop1);
            if (StackIsEmpty(&stack)) {
                return RPN_ERROR_STACK_UNDERFLOW; //only one thing to pop
            }
            StackPop(&stack, &val_pop2);

            //all operations taken care of
            if (*tok == '+') {
                *result = val_pop2 + val_pop1;
            }
            if (*tok == '-') {
                *result = val_pop2 - val_pop1;
            }
            if (*tok == '*') {
                *result = val_pop2 * val_pop1;
            }
            if (*tok == '/') {
                if (val_pop1 == 0) {
                    return RPN_ERROR_DIVIDE_BY_ZERO;
                }
                *result = val_pop2 / val_pop1;

            }

            StackPush(&stack, *result); //pushes result back on stack
        } else if (*tok == '\t') {
            count++; //counts tabs
        } else if (*tok == '\n') {
            return RPN_ERROR_STACK_UNDERFLOW; //enter means stack is empty
        } else {
            return RPN_ERROR_INVALID_TOKEN; //when token is not a number
            //operator
        }
        element++; //Counts number of iterations of loop
        tok = (char*) strtok(NULL, " "); //gets next token
    }

    if (element == 1 && atof(rpn_string)) { // if there's only 1 token
        int dot = 0;
        int j;
        for (j = 0; j < strlen(rpn_string); j++) { //iterate through the token
            if (rpn_string[j] == '.') {
                dot++; //counts decimal points, since atof takes in .
                if (dot > 1) {
                    return RPN_ERROR_INVALID_TOKEN; //invalid if more than 1 .
                }
            }

        }
        *result = atof(rpn_string); //if token is a num, result is that num
    }

    if (StackIsEmpty(&stack) || count == element) { //empty or has only tabs
        return RPN_ERROR_TOO_FEW_ITEMS_REMAIN;
    }
    if (StackGetSize(&stack) >= 2) {
        return RPN_ERROR_TOO_MANY_ITEMS_REMAIN;
    }
    return RPN_NO_ERROR; //No error if nothing is returned
}

/**
 * This function should read through an array of characters, checking for 
 * backspace characters. 
 * When it encounters a backspace character, it eliminates the backspace, 
 * the preceeding character
 * (if such a character exists), and shifts all subsequent characters as 
 * appropriate.
 * @param string_to_modify The string that will be processed for backspaces. 
 *This string is modified "in place", so it is both an input and an output 
 * to the function.
 * @return Returns the size of the resulting string in "string_to_modify".
 * ProcessBackspaces() should be able to handle multiple repeated backspaces 
 * and also strings with more backspaces than characters. It should be able to
 * handle strings of at least 255 length.
 * 
 * */
int ProcessBackspaces(char *rpn_sentence) {
    int i = 0; //index for rpn_sentence array
    int j = 0; //second index
    int L = strlen(rpn_sentence); //length of string
    while (i < L) {
        if (rpn_sentence[i] == '\b' && i > 0) {
            for (j = i - 1; j < L; j++) {
                rpn_sentence[j] = rpn_sentence[j + 2]; //deletes and left shifs
            }
            L -= 2; //new length decrements by 2 
            rpn_sentence[L] = '\0'; //null terminates to remove repeated chars
            i--; //decrement index to prevent skipping chars

        }
        else if (i == 0 && rpn_sentence[i] == '\b') {
            for (j = i; j < L; j++) {
                rpn_sentence[j] = rpn_sentence[j + 1]; //deletes and left shifs
            }
            L -= 1; //new length decrements by 2 
            rpn_sentence[L] = '\0'; //null terminates to remove repeated chars
            i--; //decrement index to prevent skipping chars
        } else {
            i++; //increment index when no \b
        }
    }

    return L; //returns new length of string
}
