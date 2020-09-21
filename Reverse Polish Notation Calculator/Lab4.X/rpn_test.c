// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//CMPE13 Support Library
#include "BOARD.h"



// User libraries
#include "rpn.h"

int main() {
    BOARD_Init();
    int error;

    printf("\n###### Beginning ARPAL's rpn test harness: ####\n\n");

    //What follows is starter code.  You will need to modify it!

    //********************Simple test*****************************
    char test0[] = "-12.334232 0 +";
    double result0;
    double expected0 = -12.334232;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test0);
    error = RPN_Evaluate(test0, &result0);
    if (!error && result0 == expected0) {
        printf("test0 PASS\n");
    } else {
        printf("test0 FAILS\n");
        printf("%f\n", result0);
        printf("%f\n", expected0);
    }
    //*****************Complex Test ********************************
    char test1[] = "34 45 - 32 4 / *";
    double result1;
    double expected1 = -88;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test1);
    error = RPN_Evaluate(test1, &result1);
    if (!error && result1 == expected1) {
        printf("test1 PASS\n");

    } else {
        printf("test1 FAILS\n");
    }

    //****************UNDERFLOW*****************************************

    char test2[] = "2 /";
    double result2;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test2);
    error = RPN_Evaluate(test2, &result2);
    if (error == RPN_ERROR_STACK_UNDERFLOW) {
        printf("test2 PASS\n");
    } else {
        printf("test2 FAILS\n");
    }

    //*****************OVERFLOW*******************************************
    char test3[] = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21";
    double result3;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test3);
    error = RPN_Evaluate(test3, &result3);
    if (error == RPN_ERROR_STACK_OVERFLOW) {
        printf("test3 PASS\n");
    } else {
        printf("test3 FAILS\n");
    }
    //******************IF OVERFLOW IS FALSE FOR SIZE 20****************
    char test4[] = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 ";
    double result4;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test4);
    error = RPN_Evaluate(test4, &result4);
    if (error != RPN_ERROR_STACK_OVERFLOW) {
        printf("test4 PASS\n");
    } else {
        printf("test4 FAILS\n");
    }
    //*********************INVALID TOKEN**********************************
    char test5[] = "1 a + ";
    double result5;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test5);
    error = RPN_Evaluate(test5, &result5);
    if (error == RPN_ERROR_INVALID_TOKEN) {
        printf("test5 PASS\n");

    } else {
        printf("test5 FAILS\n");
    }

    //*****************DIVIDE BY ZERO**************************************
    char test6[] = "1 0 /";
    double result6;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test6);
    error = RPN_Evaluate(test6, &result6);
    if (error == RPN_ERROR_DIVIDE_BY_ZERO) {
        printf("test6 PASS\n");
    } else {
        printf("test6 FAILS\n");

    }
    //**********************TOO MANY ITEMS*********************************
    char test7[] = "56 23 5 6";
    double result7;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test7);
    error = RPN_Evaluate(test7, &result7);
    if (error == RPN_ERROR_TOO_MANY_ITEMS_REMAIN) {
        printf("test7 PASS\n");
    } else {
        printf("test7 FAILS\n");
    }

    //***********************TOO FEW ITEMS**********************************
    char test8[] = " ";
    double result8;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test8);
    error = RPN_Evaluate(test8, &result8);
    if (error == RPN_ERROR_TOO_FEW_ITEMS_REMAIN) {
        printf("test8 PASS\n");
    } else {
        printf("test8 FAILS\n");
    }
    //********************TAB***********************************************
    char test9[] = "\t";
    double result9;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test9);
    error = RPN_Evaluate(test9, &result9);
    if (error == RPN_ERROR_TOO_FEW_ITEMS_REMAIN) {
        printf("test9 PASS\n");
    } else {
        printf("test9 FAILS\n");
    }
    //*****************INVALID CHAR ******************************************
    char test10[] = ".";
    double result10;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test10);
    error = RPN_Evaluate(test10, &result10);
    if (error == RPN_ERROR_INVALID_TOKEN) {
        printf("test10 PASS\n");
    } else {
        printf("test10 FAILS\n");
        printf("%f\n", result10);
    }
    //******************if input is 0 only **********************************
    char test11[] = "0";
    double result11;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test11);
    error = RPN_Evaluate(test11, &result11);
    if (!error && result11 == 0) {
        printf("test11 PASS\n");
    } else {
        printf("test11 FAILS\n");
        printf("%f\n", result11);
    }
    //********************JUST NUMBER*****************************************
    char test12[] = "1";
    double result12;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test12);
    error = RPN_Evaluate(test12, &result12);
    if (!error && result12 == 1) { //&& result12 == 12) {
        printf("test12 PASS\n");
    } else {
        printf("test12 FAILS\n");
        printf("%f\n", result12);
    }

    //******************NEGATIVE NUMBER OPERATION******************************
    char test13[] = "-1 -1 +";
    double result13;
    printf("Testing RPN_Evaluate with \"%s\"... \n ", test13);
    error = RPN_Evaluate(test13, &result13);
    if (!error && result13 == -2) {
        printf("test13 PASS\n");
    } else {
        printf("test13 FAILS\n");
        printf("%f\n", result13);
    }

    //**********************BACKSPACE***************************************
    printf("Testing ProcessBackspaces:\n");

    char test_pb1[] = "\b123\b34";
    ProcessBackspaces(test_pb1);
    printf("result    : %s\n", test_pb1);
    printf("should be : 1234\n");


    char test_2pb1[] = "\b12\b\b4";
    ProcessBackspaces(test_2pb1);
    printf("result    : %s\n", test_2pb1);
    printf("should be : 4\n");


    char test_3pb1[] = "\b\b\b23\b\b\b45\b6789\b\b23";
    ProcessBackspaces(test_3pb1);
    printf("result  : %s\n", test_3pb1);
    printf("should be :46723 \n");
    BOARD_End();
    while (1);
}


