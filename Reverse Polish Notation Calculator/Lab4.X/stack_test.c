// **** Include libraries here ****
// Standard libraries
#include <stdio.h>

//CMPE13 Support Library
#include "BOARD.h"


// User libraries
#include "stack.h"

int main() {
    BOARD_Init();

    printf("\n###### Beginning ARPAL's stack test harness: ####\n\n");

    //What follows is starter code.  You will need to modify it!

    //************************STACK INIT*************************************
    printf("Does StackInit() set the currentItemIndex appropriately?\n");
    struct Stack stack = {};
    StackInit(&stack);
    if (stack.currentItemIndex == -1) {
        printf("passed");
    } else {
        printf("failed");
    }
    int test1, test2;
    //*******************PUSH*******************************************
    printf("\n");
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    StackPush(&stack, 45);
    StackPush(&stack, 23);
    int push1 = StackPush(&stack, 45);
    test1 = (push1) ? 1 : 0; //Stack size is 19
    int push2 = StackPush(&stack, 56); //Stack is full
    test2 = (!push2) ? 1 : 0;
    printf("StackPush() %d/2\n", test1 + test2);

    //****************POP*****************************
    double storeval;
    struct Stack stack2 = {};
    StackInit(&stack2);
    int Pop1 = StackPop(&stack2, &storeval);
    test1 = (!Pop1) ? 1 : 0;
    StackPush(&stack2, 45);
    StackPush(&stack2, 4534);
    int Pop2 = StackPop(&stack2, &storeval);
    test2 = (Pop2) ? 1 : 0;
    printf("StackPop() %d/2\n", test1 + test2);

    //***************isEmpty****************************
    struct Stack stack3 = {};
    StackInit(&stack3);
    int empty1 = StackIsEmpty(&stack3);
    test1 = (empty1) ? 1 : 0;
    StackPush(&stack3, 45);
    int empty2 = StackIsEmpty(&stack3);
    test1 = (!empty2) ? 1 : 0;
    printf("StackIsEmpty() %d/2\n", test1 + test2);


    //***************isFull******************************
    struct Stack stack4 = {};
    StackInit(&stack4);
    int full_1 = StackIsFull(&stack4);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    test1 = (!full_1) ? 1 : 0;
    StackPush(&stack4, 45);
    StackPush(&stack4, 45);
    int full_2 = StackIsFull(&stack4);
    test2 = (full_2) ? 1 : 0;
    printf("StackIsFull() %d/2\n", test1 + test2);

    //***************GetSize*****************************
    struct Stack stack5 = {};
    double val;
    StackInit(&stack5);
    int size1 = StackGetSize(&stack5);
    test1 = (size1 == 0) ? 1 : 0;
    StackPush(&stack5, 56);
    StackPush(&stack5, 56);
    StackPush(&stack5, 56);
    StackPush(&stack5, 56);
    StackPush(&stack5, 56);
    StackPush(&stack5, 56);
    StackPush(&stack5, 56);
    int size2 = StackGetSize(&stack5);
    test2 = (size2 == 7) ? 1 : 0;
    StackPop(&stack5, &val);
    StackPop(&stack5, &val);
    StackPop(&stack5, &val);
    StackPop(&stack5, &val);
    int size3 = StackGetSize(&stack5);
    int test3 = (size3 == 3) ? 1 : 0;
    printf("StackGetSize() %d/3\n", test1 + test2 + test3);




    BOARD_End();
    //test stackInit:
    while (1);
    return 0;
}


