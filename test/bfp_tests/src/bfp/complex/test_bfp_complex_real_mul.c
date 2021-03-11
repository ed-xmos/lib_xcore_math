// Copyright 2020-2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bfp_math.h"

#include "../../tst_common.h"

#include "unity.h"

#if DEBUG_ON || 0
#undef DEBUG_ON
#define DEBUG_ON    (1)
#endif


#define REPS        1000
#define MAX_LEN     40 


static unsigned seed = 666;


static char msg_buff[200];

#define TEST_ASSERT_EQUAL_MSG(EXPECTED, ACTUAL, EXTRA, LINE_NUM)   do{          \
    if((EXPECTED)!=(ACTUAL)) {                                                  \
      sprintf(msg_buff, "%s (test vector @ line %u)", (EXTRA), (LINE_NUM));     \
      TEST_ASSERT_EQUAL_MESSAGE((EXPECTED), (ACTUAL), msg_buff);                \
    }} while(0)





void test_bfp_complex_s16_real_mul()
{
    PRINTF("%s...\n", __func__);

    seed = 546457;

    struct {
        int16_t WORD_ALIGNED real[MAX_LEN];
        int16_t WORD_ALIGNED imag[MAX_LEN];
    } WORD_ALIGNED A_data, WORD_ALIGNED B_data;

    int16_t WORD_ALIGNED C_data[MAX_LEN];

    bfp_complex_s16_t A, B;
    bfp_s16_t C;

    struct {
        double real[MAX_LEN];
        double imag[MAX_LEN];
    } Af, Bf;

    double Cf[MAX_LEN];

    struct {
        int16_t real[MAX_LEN];
        int16_t imag[MAX_LEN];
    } expA;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        bfp_complex_s16_init(&B, B_data.real, B_data.imag, 
            pseudo_rand_int(&seed, -30, 30),
            pseudo_rand_uint(&seed, 1, MAX_LEN+1), 0);

        bfp_s16_init(&C, C_data, pseudo_rand_int(&seed, -30, 30), B.length, 0);

        bfp_complex_s16_init(&A, A_data.real, A_data.imag, 0, B.length, 0);

        B.hr = pseudo_rand_uint(&seed, 0, 12);
        C.hr = pseudo_rand_uint(&seed, 0, 12);

        for(int i = 0; i < B.length; i++){
            B.real[i] = pseudo_rand_int16(&seed) >> B.hr;
            B.imag[i] = pseudo_rand_int16(&seed) >> B.hr;
            C.data[i] = pseudo_rand_int16(&seed) >> C.hr;
        }

        test_double_from_complex_s16(Bf.real, Bf.imag, &B);
        test_double_from_s16(Cf, &C);

        for(int i = 0; i < B.length; i++){
            Af.real[i] = Bf.real[i] * Cf[i];
            Af.imag[i] = Bf.imag[i] * Cf[i];
        }

        bfp_complex_s16_real_mul(&A, &B, &C);

        TEST_ASSERT_EQUAL(xs3_vect_complex_s16_headroom(A.real, A.imag, A.length), A.hr);

        test_complex_s16_from_double(expA.real, expA.imag, Af.real, Af.imag, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){

            // printf("A.real[%d] = %d <--  %d       %d\n", i,  A.real[i], B.real[i], C.data[i]);
            // printf("A.imag[%d] = %d <--  %d       %d\n", i,  A.imag[i], B.imag[i], C.data[i]);
            // printf("======\n");

            TEST_ASSERT_INT16_WITHIN(1, expA.real[i], A.real[i]);
            TEST_ASSERT_INT16_WITHIN(1, expA.imag[i], A.imag[i]);
        }
    }
}





void test_bfp_complex_s32_real_mul()
{
    PRINTF("%s...\n", __func__);

    seed = 546457;

    complex_s32_t A_data[MAX_LEN], B_data[MAX_LEN];

    int32_t C_data[MAX_LEN];

    bfp_complex_s32_t A, B;
    bfp_s32_t C;


    struct {
        double real[MAX_LEN];
        double imag[MAX_LEN];
    } Af, Bf;

    double Cf[MAX_LEN];

    complex_s32_t expected[MAX_LEN];

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        bfp_complex_s32_init(&B, B_data, 
            pseudo_rand_int(&seed, -30, 30),
            pseudo_rand_uint(&seed, 1, MAX_LEN+1), 0);
        
        bfp_complex_s32_init(&A, A_data, 0, B.length, 0);

        bfp_s32_init(&C, C_data, pseudo_rand_int(&seed, -30, 30), B.length, 0);

        B.hr = pseudo_rand_uint(&seed, 0, 28);
        C.hr = pseudo_rand_uint(&seed, 0, 28);

        for(int i = 0; i < B.length; i++){

            B.data[i].re = pseudo_rand_int16(&seed) >> B.hr;
            B.data[i].im = pseudo_rand_int16(&seed) >> B.hr;
            C.data[i]    = pseudo_rand_int16(&seed) >> C.hr;
        }

        test_double_from_complex_s32(Bf.real, Bf.imag, &B);
        test_double_from_s32(Cf, &C);

        for(int i = 0; i < B.length; i++){
            Af.real[i] = Bf.real[i] * Cf[i];
            Af.imag[i] = Bf.imag[i] * Cf[i];
        }

        bfp_complex_s32_real_mul(&A, &B, &C);

        TEST_ASSERT_EQUAL(xs3_vect_complex_s32_headroom(A.data, A.length), A.hr);

        test_complex_s32_from_double(expected, Af.real, Af.imag, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){

            // printf("A.real[%d] = %d <--  %d       %d\n", i,  A.real[i], B.real[i], C.data[i]);
            // printf("A.imag[%d] = %d <--  %d       %d\n", i,  A.imag[i], B.imag[i], C.data[i]);
            // printf("======\n");

            TEST_ASSERT_INT32_WITHIN(1, expected[i].re, A.data[i].re);
            TEST_ASSERT_INT32_WITHIN(1, expected[i].im, A.data[i].im);
        }
    }
}





void test_bfp_mul_vect_complex()
{
    SET_TEST_FILE();
    RUN_TEST(test_bfp_complex_s16_real_mul);
    RUN_TEST(test_bfp_complex_s32_real_mul);
}