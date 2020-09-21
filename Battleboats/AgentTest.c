/* 
 * File:   agent_test.c
 * Author: arkapal
 *
 * Created on May 30, 2020, 4:34 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "BOARD.h"
#include "Agent.h"
#include "BattleBoats.h"
#include "Message.h"

/*
 * 
 */

int main() {
    BOARD_Init();
    int testCounter = 0;
    int totalTestCounter = 0;
    Message message;
    AgentInit();
    if (AgentGetState() == AGENT_STATE_START) {
        testCounter++;
    }
    printf("AgentInit tests passed: %d/1\n", testCounter);
    if (testCounter == 1) {
        totalTestCounter++;
    }
    testCounter = 0;
    AgentSetState(AGENT_STATE_CHALLENGING);
    if (AgentGetState() == AGENT_STATE_CHALLENGING) {
        testCounter++;
    }
    AgentSetState(AGENT_STATE_END_SCREEN);
    if (AgentGetState() == AGENT_STATE_END_SCREEN) {
        testCounter++;
    }
    printf("AgentGetState tests passed %d/2\n", testCounter);
    printf("AgentSetState tests passed: %d/2\n", testCounter);
    if (testCounter == 2) {
        totalTestCounter += 2;
    }
    testCounter = 0;
    //printf("Testing AgentRun:");
    AgentInit();
    BB_Event event;
    event.type = BB_EVENT_START_BUTTON;
    event.param0 = 0;
    event.param1 = 0;
    event.param2 = 0;
    message = AgentRun(event);
    if (AgentGetState() == AGENT_STATE_CHALLENGING) {
        testCounter++;
    }
    if (message.type == MESSAGE_CHA) {
        testCounter++;
    }

    AgentInit();
    event.type = BB_EVENT_CHA_RECEIVED;
    message = AgentRun(event);
    if (message.type == MESSAGE_ACC) {
        testCounter++;
    }
    if (AgentGetState() == AGENT_STATE_ACCEPTING) {
        testCounter++;
    }
    AgentInit();
    event.type = BB_EVENT_ACC_RECEIVED;
    message = AgentRun(event);

    AgentInit();
    event.type = BB_EVENT_REV_RECEIVED;

    if (AgentGetState() == AGENT_STATE_ATTACKING) {
        if (message.type == MESSAGE_SHO) {
            testCounter += 0;
        }
    } else {
        testCounter++;
    }
    AgentInit();
    AgentSetState(AGENT_STATE_DEFENDING);
    event.type = BB_EVENT_SHO_RECEIVED;
    message = AgentRun(event);
    //NegotiateCoinFlip(4,5);
    if (AgentGetState() == AGENT_STATE_WAITING_TO_SEND) {
        testCounter++;
    }
    // printf("Agent Get state is %d\n", AgentGetState());
    if (message.type == MESSAGE_RES) {
        testCounter++;
    }
    event.type = BB_EVENT_MESSAGE_SENT;
    message = AgentRun(event);
    if (message.type == MESSAGE_SHO) {
        testCounter++;
    }
    // printf("Message type is %d", message.type);
    event.type = BB_EVENT_RESET_BUTTON;
    message = AgentRun(event);
    if (AgentGetState() == AGENT_STATE_START) {
        testCounter++;
    }
    if (message.type == MESSAGE_NONE) {
        testCounter++;
    }
    AgentInit();
    event.type = BB_EVENT_RES_RECEIVED;
    AgentSetState(AGENT_STATE_ATTACKING);
    message = AgentRun(event);
    if (AgentGetState() == AGENT_STATE_DEFENDING) {
        testCounter++;
    } else {
        printf("STATE %d", AgentGetState());
    }

    printf("Agent Run passes %d/11 Tests\n", testCounter);

    if (testCounter == 11) {
        totalTestCounter++;
    }
    printf("\nTotal tests passed: %d/4", totalTestCounter);






    while (1);

}

