#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    srand(time(NULL));
    int num = 0;
    
    for (int i = 0; i < 8; i++)
    {
        num = (num * 10) + (rand() % 10);
    }
    printf(" %d \n ", num);
}