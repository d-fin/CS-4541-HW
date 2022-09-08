#include <stdio.h>

/* function declarations  */
int problemOne();
int problemTwo();
int problemThree();
int problemFour();
int problemFive();
int problemSix();

/* main - calls functions */
int main(int argc, char *argv[]){
    problemOne();
    problemTwo();
    problemThree();
    problemFour();
    problemFive();
    problemSix();

    return 0;
}

/* below are my functions - made one for each problem */
int problemOne(){
    float x = 2.5;
    printf("\nProblem 1: %.10f\n", x);
    return 0;
}

int problemTwo(){
    float x = -1.0/10.0;
    printf("\nProblem 2: %f\n", x);
    return 0;
}

int problemThree(){
    printf("\nProblem 3:");
    double x = 1/3;
    printf("\n1/3 = %f", x);
    x = 1.0/3.0; 
    printf("\n1.0/3.0 = %f", x);
    return 0;
}

int problemFour(){
    double x = 9999999.3399999999;
    printf("\n\nProblem 4:\nAs double - %f", x);
    printf("\nCasted as float - %f", (float)x);
    return 0;
}

int problemFive(){
    int x = 30000;
    printf("\n\nProblem 5:");

    while(x < 80000){
        int y = x * x;
        printf("\n%d * %d = %d", x, x, y);
        x = x + 10000;
    }

    return 0;
}

int problemSix(){
    printf("\n\nProblem 6:");
    float x = 1e20;
    printf("\na. %f", x);

    float y = (1e20 + 3500000000);
    printf("\nb. %f", y);

    float z = (1e20 + (3500000000 * 1000000000));
    printf("\nc. %f", z);

    float n = 1e20;
    for(int i = 0; i <= 1000000000; i += 3500000000){
        n += i;
    }
    printf("\nd. %f", n);

    return 0;
}