// Copyright 2020-2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bfp_math.h"

#include "../tst_common.h"

#include "unity.h"

#if DEBUG_ON || 0
#undef DEBUG_ON
#define DEBUG_ON    (1)
#endif


#define REPS        (50)
#define MAX_LEN     (300)





static void test_bfp_s16_set()
{
    PRINTF("%s...\n", __func__);

    unsigned seed = 363477;
    
    bfp_s16_t A;

    int16_t WORD_ALIGNED data[MAX_LEN];

    int16_t val;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep %d..\n", r);

        memset(data, 0, sizeof(data));

        unsigned length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);

        bfp_s16_init(&A, data, 0, length, 0);

        exponent_t exponent = pseudo_rand_int(&seed, -20, 20);

        val = pseudo_rand_int16(&seed) >> (pseudo_rand_uint32(&seed) % 10);

        // PRINTF("\t\t(length, exp, val) = (%u, %d, %d)\n", length, exponent, val);

        bfp_s16_set(&A, val, exponent);

        headroom_t exp_hr = HR_S16(val);

        TEST_ASSERT_EQUAL(exponent, A.exp);
        TEST_ASSERT_EQUAL(exp_hr, A.hr);

        for(int i = 0; i < length; i++){
            TEST_ASSERT_EQUAL(val, A.data[i]);
        }
        for(int i = length; i < MAX_LEN; i++)
            TEST_ASSERT_EQUAL(0, A.data[i]);
    }
}

static void test_bfp_s32_set()
{
    PRINTF("%s...\n", __func__);

    unsigned seed = 232356;
    
    bfp_s32_t A;

    int32_t WORD_ALIGNED data[MAX_LEN];

    int32_t val;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep %d..\n", r);

        memset(data, 0, sizeof(data));

        unsigned length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);

        bfp_s32_init(&A, data, 0, length, 0);

        exponent_t exponent = pseudo_rand_int(&seed, -20, 20);

        val = pseudo_rand_int32(&seed) >> (pseudo_rand_uint32(&seed) % 10);

        // PRINTF("\t\t(length, exp, val) = (%u, %d, %d)\n", length, exponent, val);

        bfp_s32_set(&A, val, exponent);

        headroom_t exp_hr = HR_S32(val);

        TEST_ASSERT_EQUAL(exponent, A.exp);
        TEST_ASSERT_EQUAL(exp_hr, A.hr);

        for(int i = 0; i < length; i++)
            TEST_ASSERT_EQUAL(val, A.data[i]);
        for(int i = length; i < MAX_LEN; i++)
            TEST_ASSERT_EQUAL(0, A.data[i]);
    }
}




static void test_bfp_complex_s16_set()
{
    PRINTF("%s...\n", __func__);

    unsigned seed = 546777;
    
    bfp_complex_s16_t A;

    int16_t WORD_ALIGNED real_data[MAX_LEN];
    int16_t WORD_ALIGNED imag_data[MAX_LEN];

    complex_s16_t val;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep %d..\n", r);

        memset(real_data, 0, sizeof(real_data));
        memset(imag_data, 0, sizeof(imag_data));

        unsigned length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);

        bfp_complex_s16_init(&A, real_data, imag_data, 0, length, 0);

        exponent_t exponent = pseudo_rand_int(&seed, -20, 20);

        val.re = pseudo_rand_int16(&seed) >> (pseudo_rand_uint32(&seed) % 10);
        val.im = pseudo_rand_int16(&seed) >> (pseudo_rand_uint32(&seed) % 10);

        bfp_complex_s16_set(&A, val, exponent);

        headroom_t exp_hr = HR_S16(val.re) < HR_S16(val.im)? 
                            HR_S16(val.re) : HR_S16(val.im);

        TEST_ASSERT_EQUAL(exponent, A.exp);
        TEST_ASSERT_EQUAL(exp_hr, A.hr);

        for(int i = 0; i < length; i++){
            TEST_ASSERT_EQUAL(val.re, A.real[i]);
            TEST_ASSERT_EQUAL(val.im, A.imag[i]);
        }
        for(int i = length; i < MAX_LEN; i++){
            TEST_ASSERT_EQUAL(0, A.real[i]);
            TEST_ASSERT_EQUAL(0, A.imag[i]);
        }
    }
}




static void test_bfp_complex_s32_set()
{
    PRINTF("%s...\n", __func__);

    unsigned seed = 232356;
    
    bfp_complex_s32_t A;

    complex_s32_t WORD_ALIGNED data[MAX_LEN];

    complex_s32_t val;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep %d..\n", r);

        memset(data, 0, sizeof(data));

        unsigned length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);

        bfp_complex_s32_init(&A, data, 0, length, 0);

        exponent_t exponent = pseudo_rand_int(&seed, -20, 20);

        val.re = pseudo_rand_int32(&seed) >> (pseudo_rand_uint32(&seed) % 10);
        val.im = pseudo_rand_int32(&seed) >> (pseudo_rand_uint32(&seed) % 10);

        bfp_complex_s32_set(&A, val, exponent);

        headroom_t exp_hr = HR_S32(val.re) < HR_S32(val.im)? 
                            HR_S32(val.re) : HR_S32(val.im);

        TEST_ASSERT_EQUAL(exponent, A.exp);
        TEST_ASSERT_EQUAL(exp_hr, A.hr);

        for(int i = 0; i < length; i++){
            TEST_ASSERT_EQUAL(val.re, A.data[i].re);
            TEST_ASSERT_EQUAL(val.im, A.data[i].im);
        }
        for(int i = length; i < MAX_LEN; i++){
            TEST_ASSERT_EQUAL(0, A.data[i].re);
            TEST_ASSERT_EQUAL(0, A.data[i].im);
        }
    }
}




static void test_bfp_ch_pair_s16_set()
{
    PRINTF("%s...\n", __func__);

    unsigned seed = 64568;
    
    bfp_ch_pair_s16_t A;

    ch_pair_s16_t WORD_ALIGNED data[MAX_LEN];

    ch_pair_s16_t val;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep %d..\n", r);

        memset(data, 0, sizeof(data));

        unsigned length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);

        bfp_ch_pair_s16_init(&A, data, 0, length, 0);

        exponent_t exponent = pseudo_rand_int(&seed, -20, 20);

        val.ch_a = pseudo_rand_int16(&seed) >> (pseudo_rand_uint32(&seed) % 10);
        val.ch_b = pseudo_rand_int16(&seed) >> (pseudo_rand_uint32(&seed) % 10);

        bfp_ch_pair_s16_set(&A, val, exponent);

        headroom_t exp_hr = HR_S16(val.ch_a) < HR_S16(val.ch_b)? 
                            HR_S16(val.ch_a) : HR_S16(val.ch_b);

        TEST_ASSERT_EQUAL(exponent, A.exp);
        TEST_ASSERT_EQUAL(exp_hr, A.hr);

        for(int i = 0; i < length; i++){
            TEST_ASSERT_EQUAL(val.ch_a, A.data[i].ch_a);
            TEST_ASSERT_EQUAL(val.ch_b, A.data[i].ch_b);
        }
        for(int i = length; i < MAX_LEN; i++){
            TEST_ASSERT_EQUAL(0, A.data[i].ch_a);
            TEST_ASSERT_EQUAL(0, A.data[i].ch_b);
        }
    }
}




static void test_bfp_ch_pair_s32_set()
{
    PRINTF("%s...\n", __func__);

    unsigned seed = 64568;
    
    bfp_ch_pair_s32_t A;

    ch_pair_s32_t WORD_ALIGNED data[MAX_LEN];

    ch_pair_s32_t val;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep %d..\n", r);

        memset(data, 0, sizeof(data));

        unsigned length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);

        bfp_ch_pair_s32_init(&A, data, 0, length, 0);

        exponent_t exponent = pseudo_rand_int(&seed, -20, 20);

        val.ch_a = pseudo_rand_int32(&seed) >> (pseudo_rand_uint32(&seed) % 10);
        val.ch_b = pseudo_rand_int32(&seed) >> (pseudo_rand_uint32(&seed) % 10);

        bfp_ch_pair_s32_set(&A, val, exponent);

        headroom_t exp_hr = HR_S32(val.ch_a) < HR_S32(val.ch_b)? 
                            HR_S32(val.ch_a) : HR_S32(val.ch_b);

        TEST_ASSERT_EQUAL(exponent, A.exp);
        TEST_ASSERT_EQUAL(exp_hr, A.hr);

        for(int i = 0; i < length; i++){
            TEST_ASSERT_EQUAL(val.ch_a, A.data[i].ch_a);
            TEST_ASSERT_EQUAL(val.ch_b, A.data[i].ch_b);
        }
        for(int i = length; i < MAX_LEN; i++){
            TEST_ASSERT_EQUAL(0, A.data[i].ch_a);
            TEST_ASSERT_EQUAL(0, A.data[i].ch_b);
        }
    }
}






void test_bfp_set_vect()
{
    SET_TEST_FILE();

    RUN_TEST(test_bfp_s16_set);
    RUN_TEST(test_bfp_s32_set);
    RUN_TEST(test_bfp_complex_s16_set);
    RUN_TEST(test_bfp_complex_s32_set);
    RUN_TEST(test_bfp_ch_pair_s16_set);
    RUN_TEST(test_bfp_ch_pair_s32_set);
}