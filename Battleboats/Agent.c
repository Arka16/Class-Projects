#include "Agent.h"
#include "BOARD.h"
#include "Field.h"
#include "Negotiation.h"
#include "Message.h"
#include "BattleBoats.h"
#include "Oled.h"
#include "FieldOled.h"
#include <stdio.h>

typedef struct {
    AgentState state;
    Message message;
    int turnCounter;
    Field ownfield;
    Field oppfield;
    NegotiationData A;
    NegotiationData B;
    NegotiationData hash_a;
    GuessData guess;
    GuessData opp_guess;
    FieldOledTurn turn;
} Agent;

static Agent agent;

/**
 * The Init() function for an Agent sets up everything necessary for an agent before the game
 * starts.  At a minimum, this requires:
 *   -setting the start state of the Agent SM.
 *   -setting turn counter to 0
 * If you are using any other persistent data in Agent SM, that should be reset as well.
 * 
 * It is not advised to call srand() inside of AgentInit.  
 *  */


void AgentInit(void) {
    agent.state = AGENT_STATE_START;
    agent.turnCounter = 0;
    agent.A = 0;
    agent.B = 0;
    agent.hash_a = 0;
    agent.guess.row = 0;
    agent.guess.col = 0;
    agent.guess.result = 0;
    agent.turn = FIELD_OLED_TURN_NONE;
    //FieldInit(&agent.ownfield, &agent.oppfield);
}

/**
 * AgentRun evolves the Agent state machine in response to an event.
 * 
 * @param  The most recently detected event
 * @return Message, a Message struct to send to the opponent. 
 * 
 * If the returned Message struct is a valid message
 * (that is, not of type MESSAGE_NONE), then it will be
 * passed to the transmission module and sent via UART.
 * This is handled at the top level! AgentRun is ONLY responsible 
 * for generating the Message struct, not for encoding or sending it.
 */
Message AgentRun(BB_Event event) {
    //NegotiationData A;
    //NegotiationData B;
    //NegotiationData hash_a;
    //static GuessData guess;
    //static FieldOledTurn turn;
    Message message;
    message.type = MESSAGE_NONE;
    message.param0 = 0;
    message.param1 = 0;
    message.param2 = 0;
    switch (agent.state) {
        case AGENT_STATE_START: //Start
            if (event.type == BB_EVENT_ERROR) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error. Try again");
                OledUpdate();
            } else {
                if (event.type == BB_EVENT_RESET_BUTTON) {
                    AgentInit(); //resets all data
                    //FieldInit(&agent.ownfield, &agent.oppfield);
                    OledClear(OLED_COLOR_BLACK);
                    OledDrawString("START\n\nReady for new game?\nPress BTN4 to start.");
                    OledUpdate();
                } else if (event.type == BB_EVENT_START_BUTTON) {
                    agent.A = rand(); //generate A
                    agent.hash_a = NegotiationHash(agent.A); // generate #a
                    message.type = MESSAGE_CHA; //send CHA
                    message.param0 = agent.hash_a; //set param to #a
                    FieldInit(&agent.ownfield, &agent.oppfield); //initialize fields
                    FieldAIPlaceAllBoats(&agent.ownfield); //Place own boats
                    agent.state = AGENT_STATE_CHALLENGING; //change state to challenging
                    OledClear(OLED_COLOR_BLACK);
                    static char str[100];
                    sprintf(str, "CHALLENGING\n %d = A\n %d = hash_a", agent.A, agent.hash_a);
                    OledDrawString(str);
                    OledUpdate();
                } else if (event.type == BB_EVENT_CHA_RECEIVED) {
                    agent.B = rand(); //generate B
                    message.type = MESSAGE_ACC; //send ACC                    
                    message.param0 = agent.B; //set param0 to B
                    agent.hash_a = event.param0; //get resulting #a
                    FieldInit(&agent.ownfield, &agent.oppfield); //initialize fields
                    FieldAIPlaceAllBoats(&agent.ownfield); //Place own boats                 
                    agent.state = AGENT_STATE_ACCEPTING; //change state to accept
                    OledClear(OLED_COLOR_BLACK);
                    static char str2[100];
                    sprintf(str2, "ACCEPTING\n %d = hash_a\n %d = B", agent.hash_a, agent.B);
                    OledDrawString(str2);
                    OledUpdate();
                }
            }

            break;
        case AGENT_STATE_CHALLENGING: //Challenging 
            if (event.type == BB_EVENT_ERROR) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error. Try again");
                OledUpdate();
            } else if (event.type == BB_EVENT_RESET_BUTTON) {
                AgentInit(); //resets all data
                //FieldInit(&agent.ownfield, &agent.oppfield);
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("START\n\nReady for new game?\nPress BTN4 to start.");
                OledUpdate();
            } else if (event.type == BB_EVENT_ACC_RECEIVED) {
                message.type = MESSAGE_REV;
                message.param0 = agent.A;
                agent.B = event.param0;
                if (NegotiateCoinFlip(agent.A, agent.B) == HEADS) {
                    agent.turn = FIELD_OLED_TURN_NONE;
                    agent.state = AGENT_STATE_WAITING_TO_SEND;
                } else if (NegotiateCoinFlip(agent.A, agent.B) == TAILS) {
                    agent.state = AGENT_STATE_DEFENDING;
                }
                agent.turn = FIELD_OLED_TURN_NONE; //display field
                OledClear(OLED_COLOR_BLACK);
                FieldOledDrawScreen(&agent.ownfield,
                        &agent.oppfield, agent.turn, agent.turnCounter);
                OledUpdate();
            }

            break;
        case AGENT_STATE_ACCEPTING: //Accepting 
            if (event.type == BB_EVENT_ERROR) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error. Try again");
                OledUpdate();
            } else if (event.type == BB_EVENT_RESET_BUTTON) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("START\n\nReady for new game?\nPress BTN4 to start.");
                OledUpdate();
                AgentInit(); //resets all data
                //FieldInit(&agent.ownfield, &agent.oppfield);
            } else if (event.type == BB_EVENT_REV_RECEIVED) {
                agent.A = event.param0;
                if (NegotiationVerify(agent.A, agent.hash_a) == FALSE) { //if cheating detected
                    agent.state = AGENT_STATE_END_SCREEN;
                } else if (NegotiateCoinFlip(agent.A, agent.B) == TAILS) { //if TAILS
                    agent.guess = FieldAIDecideGuess(&agent.oppfield); //decide guess
                    message.type = MESSAGE_SHO; //send SHO
                    message.param0 = agent.guess.row; //send row of guess
                    message.param1 = agent.guess.col; //send col of guess
                    agent.state = AGENT_STATE_ATTACKING; //to Attacking
                } else if (NegotiateCoinFlip(agent.A, agent.B) == HEADS) { //if HEADS
                    agent.state = AGENT_STATE_DEFENDING; //to Defending                   
                }
                agent.turn = FIELD_OLED_TURN_NONE;
                OledClear(OLED_COLOR_BLACK);
                FieldOledDrawScreen(&agent.ownfield,
                        &agent.oppfield, agent.turn, agent.turnCounter);
                OledUpdate();
            }

            break;
        case AGENT_STATE_END_SCREEN: //End Screen
            OledClear(OLED_COLOR_BLACK);
            if (event.type == BB_EVENT_ERROR) {
                OledDrawString("Error. Try again");
            } else if (event.type == BB_EVENT_RESET_BUTTON) {
                OledDrawString("START\n\nReady for new game?\nPress BTN4 to start.");
                AgentInit(); //resets all data
                //FieldInit(&agent.ownfield, &agent.oppfield);
                OledDrawString("START\n\nReady for new game?\nPress BTN4 to start.");
            } else if (NegotiationVerify(agent.A, agent.hash_a) == FALSE) { //if cheating 
                OledDrawString("Game over, cheating detected");
            } else if (FieldGetBoatStates(&agent.ownfield) == 0) {
                OledDrawString("Game over, you lost"); //if lost
            } else {
                OledDrawString("Game over, you Won"); //if won
            }
            OledUpdate();
            break;
            //GAMEPLAY
        case AGENT_STATE_WAITING_TO_SEND: //waiting to send
            if (event.type == BB_EVENT_ERROR) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error. Try again");
                OledUpdate();
            } else if (event.type == BB_EVENT_RESET_BUTTON) {
                AgentInit(); //resets all data
                //FieldInit(&agent.ownfield, &agent.oppfield);
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("START\n\nReady for new game?\nPress BTN4 to start.");
                OledUpdate();
            } else if (event.type == BB_EVENT_MESSAGE_SENT) {
                agent.turnCounter++;
                agent.guess = FieldAIDecideGuess(&agent.oppfield); //make guess
                message.type = MESSAGE_SHO; //send SHO
                message.param0 = agent.guess.row;
                message.param1 = agent.guess.col;
                agent.turn = FIELD_OLED_TURN_NONE;
                /*OledClear(OLED_COLOR_BLACK);
                FieldOledDrawScreen(&agent.ownfield,
                        &agent.oppfield, agent.turn, agent.turnCounter);
                OledUpdate();*/
                agent.state = AGENT_STATE_ATTACKING;
                //agent.turn = FIELD_OLED_TURN_NONE;


            }
            break;
        case AGENT_STATE_ATTACKING: //Attacking 
            if (event.type == BB_EVENT_ERROR) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error. Try again");
                OledUpdate();
            } else if (event.type == BB_EVENT_RESET_BUTTON) {
                AgentInit(); //resets all data
                //FieldInit(&agent.ownfield, &agent.oppfield);
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("START\n\nReady for new game?\nPress BTN4 to start.");
                OledUpdate();
            } else if (event.type == BB_EVENT_RES_RECEIVED) {
                agent.guess.row = event.param0;
                agent.guess.col = event.param1;
                agent.guess.result = event.param2;
                FieldUpdateKnowledge(&agent.oppfield, &agent.guess); //updates if user is winning
                agent.turn = FIELD_OLED_TURN_MINE;
                OledClear(OLED_COLOR_BLACK);
                FieldOledDrawScreen(&agent.ownfield,
                        &agent.oppfield, agent.turn, agent.turnCounter);
                OledUpdate();
                if (FieldGetBoatStates(&agent.oppfield) == 0) { //if victory
                    agent.state = AGENT_STATE_END_SCREEN;
                } else {
                    agent.state = AGENT_STATE_DEFENDING;
                }
            }
            break;
        case AGENT_STATE_DEFENDING: //Defending
            if (event.type == BB_EVENT_ERROR) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error. Try again");
                OledUpdate();
            } else if (event.type == BB_EVENT_RESET_BUTTON) {
                AgentInit(); //resets all data
                //FieldInit(&agent.ownfield, &agent.oppfield);
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("START\n\nReady for new game?\nPress BTN4 to start.");
                OledUpdate();
            } else if (event.type == BB_EVENT_SHO_RECEIVED) {
                agent.guess.row = event.param0;
                agent.guess.col = event.param1;
                FieldRegisterEnemyAttack(&agent.ownfield, &agent.guess); //update own field
                message.type = MESSAGE_RES; //send RES     
                message.param0 = agent.guess.row;
                message.param1 = agent.guess.col;
                message.param2 = agent.guess.result;
                agent.turn = FIELD_OLED_TURN_THEIRS;
                OledClear(OLED_COLOR_BLACK);
                FieldOledDrawScreen(&agent.ownfield,
                        &agent.oppfield, agent.turn, agent.turnCounter);
                OledUpdate();
                if (FieldGetBoatStates(&agent.ownfield) == 0) { //if defeat
                    agent.state = AGENT_STATE_END_SCREEN; //end screen
                } else {
                    agent.state = AGENT_STATE_WAITING_TO_SEND; //go back to wait
                }
            }
            break;
        case AGENT_STATE_SETUP_BOATS:
            if (event.type == BB_EVENT_ERROR) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error. Try again");
                OledUpdate();
            } else if (event.type == BB_EVENT_RESET_BUTTON) {
                AgentInit(); //resets all data
                //FieldInit(&agent.ownfield, &agent.oppfield);
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("START\n\nReady for new game?\nPress BTN4 to start. ");
                OledUpdate();
            }
            /*else{
                if(event.type == BB_EVENT_SOUTH_BUTTON){
                    //move boat up and down
                }
                else if(event.type == BB_EVENT_EAST_BUTTON){
                    //move boat left and right
                }
            }*/
            break;

    }
    return message;




}

/** * 
 * @return Returns the current state that AgentGetState is in.  
 * 
 * This function is very useful for testing AgentRun.
 */
AgentState AgentGetState(void) {

    return agent.state;
}

/** * 
 * @param Force the agent into the state given by AgentState
 * 
 * This function is very useful for testing AgentRun.
 */
void AgentSetState(AgentState newState) {
    agent.state = newState;

}
