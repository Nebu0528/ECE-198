
//create random 8 digit number
#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>
#include <helper_functions.h>
int sequenceGenerator(void) {
    int sequence = 0;
    srand(time(NULL));
    sequence = rand() % 99999999;
    return sequence;
}

