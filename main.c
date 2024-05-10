#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define F256_SIZE sizeof(uint8_t)
#define MODULUS_DEGREE 8
void display_poly(uint8_t a);

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

int degree(uint8_t a) {
    for (int i = 7; i >= 0; i--){
        if (a & (1<<i)) {
            return i;
        }
    }
    return 0;
}

/*
uint8_t inverse(uint8_t a, uint8_t m) {
    if (a = 0) {
        // beware, don't look for the inverse of 0
        return 0;
    } else {
        // r is set to 1 because we are dividing 1
        uint8_t r = 1;
        uint8_t q = 0;
        
        uint8_t degree = degree(a);
    }
}
*/



uint8_t euclidean_division(uint8_t a, uint8_t b, uint8_t m) {
    uint8_t q = 0;
    uint8_t r = a;

    if (b == 0) {
        // beware edge case
        return 0;
    } else if (b == 1) {
        return a;
    } else {    
        int deg = degree(b);

        int x_power;
        uint8_t b_shifted;
        // actually this assumes final r can only be degree 0
        // maybe change back to inversion
        int i=0;
        while (degree(r)!=0 && i<4) {
            if (degree(r)<deg) {
                // here we add the modulus polynomial
                // r now has a "virtual" MODULUS_DEGREE monomial
                r = addition(r,m,m);
                x_power = MODULUS_DEGREE-deg;
                printf("new R: X^8 + \n");
                display_poly(r);
            } else {
                x_power = degree(r)-deg;
            }

            printf("Q before:\n");
            display_poly(q);

            q = addition(q,1<<x_power,m);
            b_shifted = mul_by_Xn(b,x_power,m);
            printf("B shifted:\n");
            display_poly(b_shifted);
            
            r = substraction(r,b_shifted,m);
            
            printf("Q after:\n");
            display_poly(q);
            printf("R:\n");
            display_poly(r);

            i++;
        }
        if (i>=4 && degree(r)!=0) {
            printf("\n*** DIVISION FAILED ***\n\n");
        }
    }

    return q;
}


/*
uint8_t* extended_euclidean(uint8_t a, uint8_t b, uint8_t m) {
    // warning: a needs to be bigger than b
    // because they are r0 and r1
    // and rn is decreasing in degree
    if (degree(a)<degree(b)) {
        // switch a and b, old and new
        temp = a;    a = b;        b = temp;
        temp = s_old;s_old = s_new;s_new = temp;
        temp = t_old;t_old = t_new;t_new = temp;
    }

    uint8_t s_old=1;
    uint8_t t_old=0;

    uint8_t s_new=0;
    uint8_t t_new=1;

    uint9_t temp;

    while (b != 0) {
        // q is a//b
        uint8_t q = euclidean_division(a,b,m);
        temp = b;
        // after this step, new b is smaller than old b
        b = substraction(b,multiplication(q,a,m),m);
        // a becomes b
        a = temp;

        // same with s and t
        temp = s_new;
        s_new = substraction(s_old,multiplication(q,s_new,m),m);
        s_old = temp;
        temp = t_new;
        t_new = substraction(t_old,multiplication(q,t_new,m),m);
        t_old = temp;
    
    uint8_t* ret = malloc(sizeof(uint8_t)*3);
    ret = {b,s_new,t_new};
    return ret;
    }
}
*/

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
    // TODO change modulus

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
    /*
    printf("Polynomial a+b :\n");
    display_poly(addition(a,b,modulus_m));
    
    printf("Polynomial a*b :\n");
    display_poly(multiplication(a,b,modulus_m));
    printf("Polynomial a*a :\n");
    display_poly(multiplication(a,a,modulus_m));
    printf("Polynomial b*b :\n");
    display_poly(multiplication(b,b,modulus_m));
    */
    printf("Polynomial a/b :\n");
    uint8_t q = euclidean_division(a,b,modulus_m);
    display_poly(q);
    printf("Polynomial a/b*b :\n");
    display_poly(multiplication(q,b,modulus_m));

    printf("Polynomial b/a :\n");
    q = euclidean_division(b,a,modulus_m);
    display_poly(q);
    printf("Polynomial b/a*a :\n");
    display_poly(multiplication(q,a,modulus_m));
    
    // printf("Sum :");

    time(&end);

    // free();

    printf("\nCompute time: %lds\n\n\n", end-begin);
}