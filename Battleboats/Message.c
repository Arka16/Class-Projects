
#include "Message.h"
#include "BOARD.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/**
 * Given a payload string, calculate its checksum
 * 
 * @param payload       //the string whose checksum we wish to calculate
 * @return   //The resulting 8-bit checksum 
 */

#define CLR '\0'
#define DECIMAL 10
#define HEX 16
#define NEWLINE '\n'
#define START '$'
#define STARTCHECK '*'

uint8_t Message_CalculateChecksum(const char* payload) {
    int len, i;
    uint8_t ascii_val;
    uint8_t checksum = 0;
    char val;
    len = strlen(payload);
    for (i = 0; i < len; i++) { //iterate through string
        val = payload[i]; //gets each char of string
        ascii_val = (uint8_t) val; //gets ascii value of a char
        checksum ^= ascii_val; //Xor's binary ascii value of each char
    }
    return checksum;
}

/**
 * ParseMessage() converts a message string into a BB_Event.  The payload and
 * checksum of a message are passed into ParseMessage(), and it modifies a
 * BB_Event struct in place to reflect the contents of the message.
 * 
 * @param payload       //the payload of a message
 * @param checksum      //the checksum (in string form) of  a message,
 *                          should be exactly 2 chars long, plus a null char
 * @param message_event //A BB_Event which will be modified by this function.
 *                      //If the message could be parsed successfully,
 *                          message_event's type will correspond to the message type and 
 *                          its parameters will match the message's data fields.
 *                      //If the message could not be parsed,
 *                          message_events type will be BB_EVENT_ERROR
 * 
 * @return STANDARD_ERROR if:
 *              the payload does not match the checksum
 *              the checksum string is not two characters long
 *              the message does not match any message template
 *          SUCCESS otherwise
 * 
 * Please note!  sscanf() has a couple compiler bugs that make it a very
 * unreliable tool for implementing this function. * 
 */
int Message_ParseMessage(const char* payload,
        const char* checksum_string, BB_Event * message_event) {
    
    int checksumval, checkint;
    char payloadcpy[MESSAGE_MAX_PAYLOAD_LEN];
    strcpy(payloadcpy, payload);
    if (strlen(checksum_string) != MESSAGE_CHECKSUM_LEN) { //if checksum not 2 chars long
        message_event->type = BB_EVENT_ERROR;
        if (strlen(checksum_string) > MESSAGE_CHECKSUM_LEN) {
            message_event->param0 = BB_ERROR_CHECKSUM_LEN_EXCEEDED;
        } else {
            message_event->param0 = BB_ERROR_CHECKSUM_LEN_INSUFFICIENT;
        }
        return STANDARD_ERROR;
    }
    checksumval = (int) Message_CalculateChecksum(payload);
    if (!strtoul(checksum_string, NULL, HEX)) { //if not parsable from hex
        message_event->type = BB_EVENT_ERROR;
        message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
        return STANDARD_ERROR;
    }
    checkint = strtoul(checksum_string, NULL, HEX);
    //parses checksums
    if (checksumval != checkint) { //checks if checksum is equivalent to payload
        message_event->type = BB_EVENT_ERROR;
        message_event->param0 = BB_ERROR_BAD_CHECKSUM;
        return STANDARD_ERROR;
    }
    //use strtok
    char *tok, *param0, *param1, *param2;
    if (strlen(payloadcpy) > MESSAGE_MAX_PAYLOAD_LEN) {
        message_event->type = BB_EVENT_ERROR;
        message_event->param0 = BB_ERROR_PAYLOAD_LEN_EXCEEDED; //if payload too long
        return STANDARD_ERROR;
    }
    tok = strtok(payloadcpy, ",");
    if (tok == NULL) {
        message_event->type = BB_EVENT_ERROR;
        message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
        return STANDARD_ERROR;
    }
    if (strcmp(tok, "CHA") == 0 || strcmp(tok, "ACC") == 0 || strcmp(tok, "REV") == 0) {
        if (strcmp(tok, "CHA") == 0) {
            message_event->type = BB_EVENT_CHA_RECEIVED;
        } else if (strcmp(tok, "ACC") == 0) {
            message_event->type = BB_EVENT_ACC_RECEIVED;
        } else {
            message_event->type = BB_EVENT_REV_RECEIVED;
        }
        param0 = strtok(NULL, ",");
        if (param0 == NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtoul(param0, NULL, DECIMAL) || (strcmp(param0, "0") == 0)) {
            message_event->param0 = strtoul(param0, NULL, DECIMAL); 
            //set event param1 to message param1
        } else {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtok(NULL, "") != NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
    } else if (strcmp(tok, "SHO") == 0) {
        message_event->type = BB_EVENT_SHO_RECEIVED;
        param0 = strtok(NULL, ",");
        if (param0 == NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtoul(param0, NULL, DECIMAL) || (strcmp(param0, "0") == 0)) {
            message_event->param0 = strtoul(param0, NULL, DECIMAL); 
            //set event param1 to message param1
        } else {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        param1 = strtok(NULL, ",");
        if (param1 == NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtoul(param1, NULL, DECIMAL) || (strcmp(param1, "0") == 0)) {
            message_event->param1 = strtoul(param1, NULL, DECIMAL); 
            //set event param1 to message param1
        } else {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtok(NULL, "") != NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }

    } else if (strcmp(tok, "RES") == 0) {
        message_event->type = BB_EVENT_RES_RECEIVED;
        param0 = strtok(NULL, ",");
        if (param0 == NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtoul(param0, NULL, DECIMAL) || (strcmp(param0, "0") == 0)) {
            message_event->param0 = strtoul(param0, NULL, DECIMAL); 
            //set event param0 to message param0
                    
        } else {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        param1 = strtok(NULL, ",");
        if (param1 == NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtoul(param1, NULL, DECIMAL) || (strcmp(param1, "0") == 0)) {
            message_event->param1 = strtoul(param1, NULL, DECIMAL); 
            //set event param1 to message param1
        } else {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        param2 = strtok(NULL, ",");
        if (param2 == NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtoul(param2, NULL, DECIMAL) || (strcmp(param2, "0") == 0)) {
            message_event->param2 = strtoul(param2, NULL, DECIMAL); 
            //set event param2 to message param2
        } else {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
        if (strtok(NULL, "") != NULL) {
            message_event->type = BB_EVENT_ERROR;
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            return STANDARD_ERROR;
        }
    } else {
        message_event->type = BB_EVENT_ERROR; //invalid type
        message_event->param0 = BB_ERROR_INVALID_MESSAGE_TYPE;
        return STANDARD_ERROR;
    }
    return SUCCESS;

}

/**
 * Encodes the coordinate data for a guess into the string `message`. This string must be big
 * enough to contain all of the necessary data. The format is specified in PAYLOAD_TEMPLATE_COO,
 * which is then wrapped within the message as defined by MESSAGE_TEMPLATE. 
 * 
 * The final length of this
 * message is then returned. There is no failure mode for this function as there is no checking
 * for NULL pointers.
 * 
 * @param message            The character array used for storing the output. 
 *                              Must be long enough to store the entire string,
 *                              see MESSAGE_MAX_LEN.
 * @param message_to_encode  A message to encode
 * @return                   The length of the string stored into 'message_string'.
                             Return 0 if message type is MESSAGE_NONE.
 */
int Message_Encode(char *message_string, Message message_to_encode) {
    char payload[MESSAGE_MAX_PAYLOAD_LEN]; //gets payload 
    uint8_t checksum;
    if (message_to_encode.type == MESSAGE_NONE) { //if no type
        return 0; //zero if not valid type
    } else if (message_to_encode.type == MESSAGE_CHA) {
        //prints according to type
        sprintf(payload, PAYLOAD_TEMPLATE_CHA,
                message_to_encode.param0);
    } else if (message_to_encode.type == MESSAGE_ACC) {
        sprintf(payload, PAYLOAD_TEMPLATE_ACC,
                message_to_encode.param0);
    } else if (message_to_encode.type == MESSAGE_REV) {
        sprintf(payload, PAYLOAD_TEMPLATE_REV,
                message_to_encode.param0);
    } else if (message_to_encode.type == MESSAGE_SHO) {
        sprintf(payload, PAYLOAD_TEMPLATE_SHO,
                message_to_encode.param0, message_to_encode.param1);
    } else if (message_to_encode.type == MESSAGE_RES) {
        sprintf(payload, PAYLOAD_TEMPLATE_RES,
                message_to_encode.param0, message_to_encode.param1,
                message_to_encode.param2);
    }
    checksum = Message_CalculateChecksum(payload);
    sprintf(message_string, MESSAGE_TEMPLATE, payload, checksum);
    return strlen(message_string);
}

/**
 * Message_Decode reads one character at a time.  If it detects a full NMEA message,
 * it translates that message into a BB_Event struct, which can be passed to other 
 * services.
 * 
 * @param char_in - The next character in the NMEA0183 message to be decoded.
 * @param decoded_message - a pointer to a message struct, used to "return" a message
 *                          if char_in is the last character of a valid message, 
 *                              then decoded_message
 *                              should have the appropriate message type.
 *                          if char_in is the last character of an invalid message,
 *                              then decoded_message should have an ERROR type.
 *                          otherwise, it should have type NO_EVENT.
 * @return SUCCESS if no error was detected
 *         STANDARD_ERROR if an error was detected
 * 
 * note that ANY call to Message_Decode may modify decoded_message.
 */


int Message_Decode(unsigned char char_in, BB_Event * decoded_message_event) {
    static char payload[MESSAGE_MAX_PAYLOAD_LEN];
    static char checksum_string[MESSAGE_CHECKSUM_LEN];

    typedef enum {
        Wait,
        Recordpayload,
        Recordchecksum
    } DecodeState;
    static DecodeState State = Wait; //initial state
    char inp = (char) char_in;
    int ascii_val = (int) char_in;
    static int parse_check;
    int retval;


    //printf("char_in: %c\n", char_in);
    switch (State) {
        case Wait:
            if (char_in == START) {
                State = Recordpayload;
            }
            retval = SUCCESS; //value to be return is SUCCESS if no error
            break;
        case Recordpayload:
            if (char_in == START || char_in == NEWLINE) {
                decoded_message_event->type = BB_EVENT_ERROR;
                State = Wait;
                retval = STANDARD_ERROR;
            } else if (char_in == STARTCHECK) {
                State = Recordchecksum;
                retval = SUCCESS;
            } else if (strlen(payload) > MESSAGE_MAX_PAYLOAD_LEN) {
                decoded_message_event->type = BB_EVENT_ERROR;
                State = Wait;
                retval = STANDARD_ERROR;
            } else {
                strncat(payload, &inp, 1);//adding one byte

                retval = SUCCESS;
            }
            break;
        case Recordchecksum:
            if (ascii_val >= 65 && ascii_val <= 70) { //if A-F
                strncat(checksum_string, &inp, 1);
                retval = SUCCESS;
            } else if (ascii_val >= 48 && ascii_val <= 57) { //if 0-9
                strncat(checksum_string, &inp, 1);
                retval = SUCCESS;
            } else if (char_in == NEWLINE) {
                parse_check = Message_ParseMessage(payload,
                        checksum_string, decoded_message_event);
                if (parse_check == SUCCESS) {
                    State = Wait;
                    retval = SUCCESS;
                    payload[0] = CLR; //clear payload
                    checksum_string[0] = CLR; //clear null terminator
                } else {
                    State = Wait;
                    retval = STANDARD_ERROR;
                }

            } else {
                State = Wait;
                retval = STANDARD_ERROR;
            }

            break;
    }
    return retval;


}
