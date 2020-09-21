/* 
 * File:   MessageTest.c
 * Author: arkapal
 *
 * Created on June 1, 2020, 3:05 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "Message.h"
#include "BOARD.h"
#include "BattleBoats.h"
#include <string.h>

/*
 * 
 */
int main() {
    BOARD_Init();
    int tests, totaltests;
    tests = 0;
    totaltests = 0;
    printf("Starting tests for Message.c\n\n");
    const char *payload = "CAT";
    uint8_t checksum, correctchecksum;
    checksum = Message_CalculateChecksum(payload);
    correctchecksum = 86;
    if (checksum == correctchecksum) {
        tests++;
    }
    const char *realpayload = "REV,12345";
    checksum = Message_CalculateChecksum(realpayload);
    correctchecksum = 92;
    if (checksum == correctchecksum) {
        tests++;
    }
    if (tests == 2) {
        totaltests++;
    }
    printf("Checksum passes %d/2 tests\n", tests);
    tests = 0;


    //PARSE MESSAGE
    const char payload2 [] = "SHO,1";
    const char *checksum2 = "49";
    char payloadcpy[MESSAGE_MAX_PAYLOAD_LEN];
    strcpy(payloadcpy, payload2);
    BB_Event *message = malloc(sizeof (BB_Event));
    Message_ParseMessage(payload2, checksum2, message);
    if (message->type == BB_EVENT_ERROR && message->param0
            == BB_ERROR_MESSAGE_PARSE_FAILURE) { //if too few fields
        tests++;
    }
    message->type = BB_EVENT_CHA_RECEIVED;
    Message_ParseMessage("RES,2,0,3", "59", message); //Valid message
    if (message->type == BB_EVENT_RES_RECEIVED) {
        tests++;
    }
    if (message->param0 == 2 && message->param1 == 0 && message->param2 == 3) {
        tests++;
    }
    Message_ParseMessage("RES,3,3,3435434,0", "46", message); //Too many
    if (message->type == BB_EVENT_ERROR && message->param0
            == BB_ERROR_MESSAGE_PARSE_FAILURE) { //if too few fields
        tests++;
    }
    Message_ParseMessage("CHA,43182", "45", message); //Bad checksum
    if (message->type == BB_EVENT_ERROR && message->param0
            == BB_ERROR_BAD_CHECKSUM) {
        tests++;
    }


    if (tests == 5) {
        totaltests++;
    }
    printf("Message parse passes %d/5 tests\n", tests);
    tests = 0;
    //ENCODE
    char message_string[MESSAGE_MAX_LEN];
    int len;
    Message encode = {MESSAGE_SHO, 3, 9};
    len = Message_Encode(message_string, encode);
    if (strcmp(message_string, "$SHO,3,9*5E\n") == 0) {
        tests++;
    }
    if (len == 12) {
        tests++;
    }
    if (tests == 2) {
        totaltests++;
    }
    //DECODE
    BB_Event eventdecode;
    printf("Message Encode passes %d/2 tests\n", tests);
    tests = 0;
    char rev[] = "$REV,123*5D\n";
    int i;
    for (i = 0; i < strlen(rev); i++) {
        Message_Decode(rev[i], &eventdecode);
    }
    if (eventdecode.type == BB_EVENT_REV_RECEIVED) {
        tests++;
    }
    if (eventdecode.param0 == 123) {
        tests++;
    }
    char res[] = "$RES,4,8,0*54\n";
    BB_Event eventdecode2;
    int j;
    for (j = 0; j < strlen(res); j++) {
        Message_Decode(res[j], &eventdecode2);
    }
    if (eventdecode2.type == BB_EVENT_RES_RECEIVED) {
        tests++;
    }
    if (eventdecode2.param0 == 4 && eventdecode2.param1 == 8 &&
            eventdecode2.param2 == 0) {
        tests++;
    }
    if (tests == 4) {
        totaltests++;
    }
    printf("Message Decode passes %d/4 tests\n", tests);
    printf("\nTotal Tests Passed: %d/4\n", totaltests);




    while (1);

}

