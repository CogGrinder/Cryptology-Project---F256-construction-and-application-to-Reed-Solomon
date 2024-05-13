#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FIELD_SIZE 256 // size of the finite field
#define GENERATOR 2    // chosen generator of the multiplicative group

// forward declarations 
uint8_t inv(uint8_t a);
uint64_t gcdExtended(uint64_t a, uint64_t b, uint64_t *u, uint64_t *v);

// function definitions 

// addition 
uint8_t add(uint8_t a, uint8_t b) {
	return a^b; // addition in finite field is bitwise XOR 
}

// subtraction
uint8_t sub(uint8_t a, uint8_t b) {
	return add(a, b); // same as addition 
}

// multiplication by X 
uint8_t mulByX (uint8_t a) { // in F256 = F2 [X]/( X ^8+ X ^4+ X ^3+ X ^2+1)
	uint8_t b = a << 1; // left shift by 1 is equivalent to multiplying by X
	if ( a & 128 ) // check if a is of degree 7
		b ^= 29; // if degree is 7, subtract irreducible polynomial to maintain field properties X^8 = X ^4+ X ^3+ X ^2+1 = 29
	return b ;
}

// multiplication 
uint8_t mul (uint8_t a, uint8_t b) { // in F256 = F2 [ X ]/( X ^8+ X ^4+ X ^3+ X ^2+1)
	uint8_t r = 0;
	for (int i =0; i < 8; i ++) {
		if ( a & 1 ) r ^= b; // if current bit of a is set, XOR b into the result 
		a >>= 1; // right shift to consider next bit 
		b = mulByX ( b ) ; // multiply b for each bit of a 
	}
	return r ;
}

// division 
uint8_t field_div(uint8_t n, uint8_t d) {
	return mul(n, inv(d)); // division is multiplication by the inverse 
}

// inverse 
uint8_t inv(uint8_t a) {
	uint64_t u, v;
	gcdExtended(a, 285, &u, &v); // find inverse using extended euclidean alg
	return (uint8_t)u;
}

uint64_t gmul (uint64_t a, uint64_t b) {
	uint64_t r = 0;
	for (int i =0; i < 64; i ++) {
		if (a & 1) r ^= b;
		a >>= 1;
		b <<= 1;
	}
	return r;
}

uint64_t power(uint64_t a, uint64_t e){
	uint64_t r = 1;
	for (uint64_t i = 0; i < e; i++){
		r = mul(r,a);
		// printf("(%lu) %lu\n", e, r);
	}
	return r;
}

// division with remainder (for use later in extended euclid. alg.)
uint64_t euclideDiv(uint64_t a, uint64_t b, uint64_t *r) {
	// printf("%lu / %lu\n", a, b);
	uint64_t bs = 0;
	while (b>>bs) bs++;
	uint64_t q = 0, rs;
	rs = 0;
	while (a>>rs) rs++;
	while (rs >= bs) {
		a ^= (b<<(rs-bs));
		q ^= 1<<(rs-bs);
		rs = 0;
		while (a>>rs) rs++;
		// printf("(%lu, %lu, %lu)\n", a, rs, q);
	}
	*r = a;
	// printf(" : %lu, %lu, %lu | ", q, a, *r);
	return q;
}

// extended euclidean algo
uint64_t gcdExtended(uint64_t a, uint64_t b, uint64_t *u, uint64_t *v){
	uint64_t i, r0, r1, u0, u1, v0, v1, tmp, q;

	r0 = a;
	u0 = 1;
	v0 = 0;

	r1 = b;
	u1 = 0;
	v1 = 1;

	while (r1 > 0) {
		q = euclideDiv(r0,r1,&tmp);

		r0 = r1;
		r1 = tmp;

		tmp = u0^mul(q,u1);
		u0 = u1;
		u1 = tmp;

		tmp = v0^mul(q,v1);
		v0 = v1;
		v1 = tmp;
	}

	*u = u0;
	*v = v0;
	return r0;
}