/* 
 * File:   newmain.c
 * Author: arkapal
 *
 * Created on May 15, 2020, 8:15 PM
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//CMPE13 Support Library
#include "BOARD.h"

/*
 * 
 */
int main() {
    BOARD_Init();

    int a = 7;
    int b = 3;
    char c = 0x30;

    switch (a | b) {
        case 0: case 1: case 2: case 3:case 4:
            c += 2;
        case 5: case 6: case 7: case 8:case 9:
            c += 3;
    }
    printf("%d\n", c);

    struct Packed {
        unsigned int a : 2;
        unsigned int b : 2;
        unsigned int c : 4;
        signed int d : 8;
    };

    int e = sizeof (struct Packed);
    printf("%d\n", e);
    while (1);

}


