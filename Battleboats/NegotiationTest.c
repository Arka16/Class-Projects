/* 
 * File:   NegotiationTest.c
 * Author: arkapal
 *
 * Created on June 3, 2020, 12:29 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Negotiation.h"
#include "BOARD.h"
#include <string.h>
#include <stdint.h>

/*
 * 
 */
int main() {
    BOARD_Init();
    int tests, totaltests;
    tests = 0;
    totaltests = 0;
    NegotiationData N, S, A, B, commitment, hash_a;
    NegotiationOutcome C;
    S = 3;
    N = NegotiationHash(S);
    if (N == 9) {
        tests++;
    }
    S = 12345;
    N = NegotiationHash(S);
    if (N == 43182) {
        tests++;
    }
    if (tests == 2) {
        totaltests++;
    }
    printf("Negotiation Hash passes %d/2 tests\n", tests);
    tests = 0;
    commitment = 43182;
    if (NegotiationVerify(S, commitment)) {
        tests++;
    }
    commitment = 8;
    if (NegotiationVerify(3, commitment) == FALSE) {
        tests++;
    }
    if (tests == 2) {
        totaltests++;
    }
    printf("Negotiation Verify passes %d/2 tests\n", tests);
    tests = 0;
    A = 131;
    B = 1224;
    C = NegotiateCoinFlip(A, B);
    if (C == HEADS) {
        tests++;
    }
    A = 3434;
    B = 4503;
    C = NegotiateCoinFlip(A, B);
    if (C == TAILS) {
        tests++;
    }
    printf("Negotiation Coin Flip passes %d/2 tests\n", tests);
    if (tests == 2) {
        totaltests++;
    }
    tests = 0;
    A = rand() + 1;
    hash_a = NegotiationHash(A);
    B = NegotiateGenerateBGivenHash(hash_a);
    if (NegotiateCoinFlip(A, B) == TAILS) {
        printf("TAILS\n");
    }
    //printf("B is %d\n", B);


    //printf("B is %d\n", eventB);
    printf("\nTOTAL TESTS PASSED: %d/3\n", totaltests);
    while (1);


}

