#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define F256_SIZE sizeof(uint8_t)
#define MODULUS_DEGREE 8
#define DEBUG_ITERMAX 60
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
    uint8_t oldq = 0;
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
        bool x8_monomial = false;
        // actually this assumes final r can only be degree 0
        // maybe change back to inversion
        int i=0;
        
        if (degree(r)<deg) {
            // here we add the modulus polynomial
            r = addition(r,m,m);
            // r now has a "virtual" MODULUS_DEGREE monomial
            x8_monomial = true;

            x_power = MODULUS_DEGREE-deg;
            printf("new R: X^8 + \n");
            display_poly(r);
        } else {
            x_power = degree(r)-deg;
        }

        q = addition(q,1<<x_power,m);
        printf("B, shift=%d:\n",x_power);
        display_poly(b);
        b_shifted = mul_by_Xn(b,x_power,m);
        printf("B shifted:");
        if (x8_monomial) {
            printf(" X^8 + \n");
        }
        printf("\n");
        display_poly(b_shifted);
        printf("Q after:\n");
        display_poly(q);

        uint8_t oldr = r;
        oldq = q;
        
        // TODO: verify (degree(r)>=degree(oldr)&&degree(r)<deg) condition
        while ((degree(r)<=degree(oldr)||i<2) && (r!=0 || q==0) && i<DEBUG_ITERMAX && (r!=a || i<2)) {
            oldr = r;
            oldq = q;

            // if (degree(r)<deg) {
            //     // here we add the modulus polynomial
            //     r = addition(r,m,m);
            //     // r now has a "virtual" MODULUS_DEGREE monomial
            //     x8_monomial = true;

                // x_power = MODULUS_DEGREE-deg;
            //     printf("new R: X^8 + \n");
            //     display_poly(r);
            // } else {
                x_power = degree(r)-deg;
            // }

            q = addition(q,1<<x_power,m);
            printf("B, shift=%d:\n",x_power);
            display_poly(b);
            b_shifted = mul_by_Xn(b,x_power,m);
            printf("B shifted:");
            if (x8_monomial) {
                printf(" X^8 + \n");
            }
            printf("\n");
            display_poly(b_shifted);
            printf("Q after:\n");
            display_poly(q);

            // printf("R before:\n");
            // display_poly(r);
            
            // add back modulus to r to cancel "virtual" monomial
            if (x8_monomial) {
                printf("Removing X^8...\n");
                r = substraction(r,m,m);
                x8_monomial = false;
            }
            r = substraction(r,b_shifted,m);
            printf("R after:\n");
            display_poly(r);

            // reset the "virtual" monomial
            i++;
        }
        if (i>=DEBUG_ITERMAX || r==1) {
            printf("\n*** DIVISION FAILED ***\n\n");
            if (r==1)  {
                printf("\n*** CYCLE DETECTED AT ITER %d ***\n\n",i);
            } else {
                printf("\n*** MAX ITERATION ***\n\n");

            }
        } else {
            printf("\n*** DIVISION SUCCESS ***\n\n");
            if (r==0) {
                return q;
            }
            else if (degree(r)>degree(oldr)) {
                return oldq;
            } else {
                return q;
            }

        }
        
    }
    if (r==1||r==0)
    {
        return q;
    } else
    {
        return oldq;
    }
}

/*
returns r = gcd(a,b), s, t,
such that r = s*a + t*b mod m
*/
uint8_t* extended_euclidean(uint8_t a, uint8_t b, uint8_t m) {
    // warning: a needs to be bigger than b
    // because they are r0 and r1
    // and rn is decreasing in degree for n>=2
    
    uint8_t s_old=1;
    uint8_t t_old=0;

    uint8_t s_new=0;
    uint8_t t_new=1;
    
    uint8_t temp;
    
    /*if (degree(a)<degree(b)) {
        // switch a and b, old and new
        temp = a;    a = b;        b = temp;
        temp = s_old;s_old = s_new;s_new = temp;
        temp = t_old;t_old = t_new;t_new = temp;
    }*/

    int i = 0;
    while (b != 0 && i<DEBUG_ITERMAX) {
        // q is a//b
        uint8_t q = euclidean_division(a,b,m);
        temp = b;
        // after this step, new b is smaller than old b
        b = substraction(b,multiplication(q,a,m),m);
        printf("EUCLIDEAN ALGO NEW B:\n");
        display_poly(b);
        printf("\n\n");
        // a becomes b
        a = temp;

        // same with s and t
        temp = s_new;
        s_new = substraction(s_old,multiplication(q,s_new,m),m);
        s_old = temp;
        temp = t_new;
        t_new = substraction(t_old,multiplication(q,t_new,m),m);
        t_old = temp;

        i++;
        }
    
    uint8_t* ret = malloc(sizeof(uint8_t)*3);
    ret[0] = b; // TODO : a or b ?
    ret[1] = s_new;
    ret[2] = t_new;

    printf("RETURNED AT ITERATION %d\n",i);
    return ret;
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
    // here we define the modulus
    // X^8 is invisible because we can only represent up to X^7
    // P4
    uint8_t modulus_m = 0b00010001;
    // P5
    // uint8_t modulus_m = 0b00011011;
    // P6
    // uint8_t modulus_m = 0b00011101;

    // P7
    // uint8_t modulus_m = 0b11111111;
    // P8
    // uint8_t modulus_m = 0b00000001;
    
    
    // TODO change modulus
    // uint8_t a = 0b00000001; //increment from here 1
    uint8_t a = 0b00001010; //increment from here 2
    for (int i = 0; i < 10; i++)
    {

    
    time_t begin;
    time_t end;
    printf("Press Enter for execution\n\n");
    getchar();


    time(&begin);
    printf("Modulus is X^8 plus:\n");
    display_poly(modulus_m);

    // uint8_t a = random_256(i);
    // uint8_t a = 0b00001000; // works with P4 and P8
    // uint8_t a = 0b00000011; // works
    // uint8_t a = 0b00000101; // works
    // uint8_t a = 0b00000111; // fails with P4
    // uint8_t a = 0b00001000; // works with P4
    // uint8_t a = 0b00001001; // fails with P4

    // uint8_t a = 0b00001010; // works with P4
    // uint8_t a = 0b00001011; // fails with P4
    // uint8_t a = 0b00001100; // works with P4
    // uint8_t a = 0b00001101; // fails with P4
    // uint8_t a = 0b00001110; // fails with P4
    // uint8_t a = 0b00001111; // works with P4
    // uint8_t a = 0b00010000; // works with P4
    // uint8_t a = 0b00010001; // works with P4
    // uint8_t a = 0b00010010; // fails here with P4


    // uint8_t a = 0b01111111; // doesn't work P4 & P8, reverts back by canceling out
    // uint8_t a = 0b11000111; // doesn't work P4, reverts back by canceling out
    // uint8_t a = 0b11010101; // doesn't work P4, reverts back by canceling out


    uint8_t b = 0b1;
    // uint8_t b = 0b00100000;
    
    // uint8_t a = random_256(0);
    // uint8_t b = random_256(1);
    uint8_t q;

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
   
    // printf("Polynomial a/b :\n");
    // q = euclidean_division(a,b,modulus_m);
    // display_poly(q);
    // printf("Polynomial a/b*b :\n");
    // display_poly(multiplication(q,b,modulus_m));

    printf("Polynomial b/a :\n");
    q = euclidean_division(b,a,modulus_m);
    display_poly(q);
    printf("r = Gcd(b,a) according to extended Euclidean, followed by s and t:\n");
    uint8_t* ret = extended_euclidean(b,a,modulus_m);
    uint8_t r = ret[0];
    uint8_t s = ret[1];
    uint8_t t = ret[2];

    display_poly(r);
    display_poly(s);
    display_poly(t);
    printf("Verify that r = s*b + t*a mod m\n");
    display_poly(addition(multiplication(s,b,modulus_m),multiplication(t,a,modulus_m),modulus_m));

    printf("Polynomial b/a*a :\n"); // doesn't work
    display_poly(multiplication(q,a,modulus_m));
    
    time(&end);

    free(ret);

    printf("\nCompute time: %lds\n\n\n", end-begin);
    a++;
    }
}