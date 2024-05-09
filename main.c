#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define F256_SIZE sizeof(uint8_t)

uint8_t random_256(int seed){
    time_t random_time;
    time(&random_time);
    return (random_time * (seed + 1) )%256;
}

uint8_t addition(uint8_t a, uint8_t b, uint8_t modulus){
    uint8_t c;
    c = a^b;
    return c;
}

uint8_t substraction(uint8_t a, uint8_t b, uint8_t modulus){
    return addition(a,b,modulus);
}

uint8_t mul_by_X(uint8_t a, uint8_t modulus) {
    uint8_t b = a << 1;
    // filter with 126, the highest 8bit power
    // 7th power of 2, 8th starting from power 0
    if (a & 0b10000000) {
        b ^= modulus;
    }
    return b;
}

uint8_t mul_by_Xn(uint8_t a, uint8_t n, uint8_t modulus) {
    uint8_t b = a;
    // repeat n times
    for (uint8_t i = 0; i < n; i++)
    {
        b = mul_by_X(b,modulus);
    }
    return b;
}

uint8_t multiplication(uint8_t a, uint8_t b, uint8_t modulus) {
    uint8_t c = 0;
    for (int i = 0; i < 8; i++)
    {
        // check ith digit of b
        if (b & (0b1 << i)){
            // add X^i times a[X]
            c = addition(c,mul_by_Xn(a,i,modulus),modulus);
        }
    }
    return c;
}

void display_poly(uint8_t a) {
    for (int i = 7; i >= 1; i--)
    {
        if (a & (1 << i)) {
            printf("X^%d + ",i);
        } else {
            printf("      ");
        }
    }
    if (a & 1) {
        printf("1");
    } else {
        printf("0");
    }
    printf("\n");
}

int main() {
    // here we define the modulus X^8 + X^4 + 1
    // X^8 is invisible because we can only represent up to X^7
    uint8_t modulus_m = 0x11;

    time_t begin;
    time_t end;
    printf("Press Enter for execution\n\n");
    getchar();


    time(&begin);
    printf("Modulus is X^8 plus:\n");
    display_poly(modulus_m);

    uint8_t a = 0b00001000;
    uint8_t b = 0b00100000;
    // uint8_t a = random_256(0);
    // uint8_t b = random_256(1);
    printf("Polynomial a :\n");
    display_poly(a);
    printf("Polynomial b :\n");
    display_poly(b);

    printf("Polynomial a+b :\n");
    display_poly(addition(a,b,modulus_m));
    
    printf("Polynomial a*b :\n");
    display_poly(multiplication(a,b,modulus_m));
    printf("Polynomial a*a :\n");
    display_poly(multiplication(a,a,modulus_m));
    printf("Polynomial b*b :\n");
    display_poly(multiplication(b,b,modulus_m));

    
    // printf("Sum :");

    time(&end);

    // free();

    printf("\nCompute time: %lds\n\n\n", end-begin);
}