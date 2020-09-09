#include <stdint.h>
#include <inttypes.h>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define Qm  7
#define Qv  10
#define K   (1 << (Qm - 1))
#define M_COLS  128
#define M_ROWS  128
#define TRIALS  10000

void matrix_vector_multiplication(const int8_t *M, const int16_t *V, int16_t *R);
int16_t dot_prod(const int8_t* R, const int16_t *V);

void matrix_vector_multiplication(const int8_t *M, const int16_t *V, int16_t *R){
    int i;
    // tested unrolling this loop, no performce gain
    for (i = 0; i < M_ROWS; i++){
        R[i] = dot_prod(&M[i*M_COLS], V);
    }
}

static inline int16_t q_mul(int8_t small, int16_t large){
    int16_t res;
    int32_t temp; 
    temp = (int32_t)small * (int32_t)large;
    temp += K;
    temp = temp >> Qm;
    if (temp > 0x7FFF) temp=0x7FFF;
    else if (temp < -0x8000) temp=-0x8000;

    return (int16_t)temp;
}

int16_t dot_prod(const int8_t* M, const int16_t *V){
    int i;
    int16_t dotprod = 0;
    // unrolling again to 8 lines per accrued no additional speedup
    for (i = 0; i < M_COLS; i+=4){
        dotprod += q_mul(M[i], V[i]);
        dotprod += q_mul(M[i+1], V[i+1]);
        dotprod += q_mul(M[i+2], V[i+2]);
        dotprod += q_mul(M[i+3], V[i+3]);
    }
    return dotprod;
}


void print_converted_mat(const int16_t *R){
    float temp;
    int i;
    for (i=0; i<M_ROWS; i++){
        temp = (float)(R[i]>>Qv);
        printf("%2.2f, ",temp);
    }
    printf("\n");
}

void fill_MV(int8_t* M, int16_t* V){
    int i, j;
    float temp;
    for (i=0; i<M_COLS; i++){
        V[i] = ((i%16)-7)*(0x01<<Qv);
    }
    for (j=0; j<M_COLS*M_ROWS; j++){
        temp = (float)(j%10)/(float)20;
        M[j] = (temp)*(0x01<<Qm);
    }
}

int main(int argc, char *argv[]){
    int16_t* V = (int16_t*) malloc((M_COLS)*sizeof(int16_t));

    int8_t* M = (int8_t*) malloc(((M_ROWS*M_COLS))*sizeof(int8_t));

    fill_MV(M, V);

    int16_t* R = (int16_t*) malloc(M_ROWS*sizeof(int16_t));

    if ((argc > 1) && (0 == strcmp(argv[1], "-v"))){
        int i;
        clock_t begin = clock();
        for (i=0; i<TRIALS; i++){
            matrix_vector_multiplication(M, V, R);
        }
        clock_t end = clock();
        double time_spent = (double)(end-begin);
        printf("Average time spent in matrix_vector_multiplication: %f\n", time_spent/TRIALS);
    }
    else{
        matrix_vector_multiplication(M, V, R);
        print_converted_mat(R);
    }
    free(M);
    free(V);
    free(R);
}
