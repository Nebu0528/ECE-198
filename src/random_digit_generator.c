// //Random Digit Generator

// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>

// int main(){
//     srand(time(NULL));
//     int num = 0;
//     //This is so that it generates a value with 8 digits only
//     for (int i = 0; i <= 7; i++)
//     {
//         num = (num * 10) + (rand() % 10);
//     }
//     printf(" %d \n ", num);
// }