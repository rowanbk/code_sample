#include <stdint.h>
#include <inttypes.h>
#include <arm_neon.h>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define Qm  7
#define Qv  10
#define M_COLS  128
#define M_ROWS  128
#define TRIALS  10000

void matrix_vector_multiplication(const int8_t *M, const int16_t *V, int16_t *R);
int16_t dot_prod_64x1(const int8_t* R, const int16x8_t *V_temps);

void matrix_vector_multiplication(const int8_t *M, const int16_t *V, int16_t *R){
    int i;
    int16x8_t* V_temps = (int16x8_t*) malloc((M_COLS/8)*sizeof(int16x8_t));
    for (i = 0; i < M_COLS/8; i++){
        V_temps[i] = vshrq_n_s16(vld1q_s16(&V[i*8]), Qm);
    }
    for (i = 0; i < M_ROWS; i++){
        R[i] = dot_prod_64x1(&M[i*M_COLS], V_temps);
        R[i] += dot_prod_64x1(&M[(i*M_COLS)+64], &V_temps[8]);
    }
    free(V_temps);
}

int16_t dot_prod_64x1(const int8_t* M, const int16x8_t *V_temps){
    int16x8_t M_temp_1 = vshll_n_s8 (vld1_s8(M), 0);
    int16x8_t R_1 = vmulq_s16(M_temp_1, V_temps[0]);

    int16x8_t M_temp_2 = vshll_n_s8 (vld1_s8(&M[8]), 0);
    int16x8_t R_2 = vmulq_s16(M_temp_2, V_temps[1]);

    int16x8_t M_temp_3 = vshll_n_s8 (vld1_s8(&M[16]), 0);
    int16x8_t R_3 = vmulq_s16(M_temp_3, V_temps[2]);

    int16x8_t M_temp_4 = vshll_n_s8 (vld1_s8(&M[24]), 0);
    int16x8_t R_4 = vmulq_s16(M_temp_4, V_temps[3]);

    int16x8_t M_temp_5 = vshll_n_s8 (vld1_s8(&M[32]), 0);
    int16x8_t R_5 = vmulq_s16(M_temp_5, V_temps[4]);

    int16x8_t M_temp_6 = vshll_n_s8 (vld1_s8(&M[40]), 0);
    int16x8_t R_6 = vmulq_s16(M_temp_6, V_temps[5]);

    int16x8_t M_temp_7 = vshll_n_s8 (vld1_s8(&M[48]), 0);
    int16x8_t R_7 = vmulq_s16(M_temp_7, V_temps[6]);

    int16x8_t M_temp_8 = vshll_n_s8 (vld1_s8(&M[56]), 0);
    int16x8_t R_8 = vmulq_s16(M_temp_8, V_temps[7]);

    R_1 = vaddq_s16(R_1, R_2);
    R_3 = vaddq_s16(R_3, R_4);
    R_5 = vaddq_s16(R_5, R_6);
    R_7 = vaddq_s16(R_7, R_8);

    R_1 = vaddq_s16(R_1, R_3);
    R_5 = vaddq_s16(R_5, R_7);

    R_1 = vaddq_s16(R_1, R_5);

    int16_t dotprod = vaddvq_s16(R_1);
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
