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


#define REPS        (1000)
#define MAX_LEN     1024


static unsigned seed = 54346;




static void test_bfp_s16_shl()
{
    PRINTF("%s...\n", __func__);

    seed = 0x038D7C77;

    int16_t dataA[MAX_LEN];
    int16_t dataB[MAX_LEN];
    int16_t dataB_copy[MAX_LEN];
    bfp_s16_t A, B, B_copy;
    A.data = dataA;
    B.data = dataB;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_s16(&B, MAX_LEN, &seed, &A, 0);

        memcpy(&B_copy, &B, sizeof(B));
        memcpy(dataB_copy, dataB, sizeof(dataB));

        unsigned leave_hr = 1;

        bfp_s16_shl(&A, &B, B.hr-leave_hr);

        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.hr, B.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);

        for(int i = 0; i < A.length; i++)
            TEST_ASSERT_EQUAL(B_copy.data[i] << (B_copy.hr-leave_hr), A.data[i]);

        int shl = -(pseudo_rand_uint32(&seed) % 5) - 1;

        memcpy(&B_copy, &A, sizeof(A));
        memcpy(dataB_copy, dataA, sizeof(dataA));

        bfp_s16_shl(&B, &A,  shl);
        
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(-shl+leave_hr, B.hr);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);

        for(int i = 0; i < A.length; i++)
            TEST_ASSERT_EQUAL(B_copy.data[i] >> (-shl), B.data[i]);
    }
}



static void test_bfp_s32_shl()
{
    PRINTF("%s...\n", __func__);

    seed = 373456;

    int32_t dataA[MAX_LEN];
    int32_t dataB[MAX_LEN];
    int32_t dataB_copy[MAX_LEN];
    bfp_s32_t A, B, B_copy;
    A.data = dataA;
    B.data = dataB;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_s32(&B, MAX_LEN, &seed, &A, 0);

        memcpy(&B_copy, &B, sizeof(B));
        memcpy(dataB_copy, dataB, sizeof(dataB));

        unsigned leave_hr = 1;

        bfp_s32_shl(&A, &B, B.hr-leave_hr);

        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.hr, B.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);

        for(int i = 0; i < A.length; i++)
            TEST_ASSERT_EQUAL(B_copy.data[i] << (B_copy.hr-leave_hr), A.data[i]);

        int shl = -(pseudo_rand_uint32(&seed) % 5) - 1;

        memcpy(&B_copy, &A, sizeof(A));
        memcpy(dataB_copy, dataA, sizeof(dataA));

        bfp_s32_shl(&B, &A, shl);
        
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(-shl+leave_hr, B.hr);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);

        for(int i = 0; i < A.length; i++)
            TEST_ASSERT_EQUAL(B_copy.data[i] >> (-shl), B.data[i]);
    }
}







static void test_bfp_ch_pair_s16_shl()
{
    PRINTF("%s...\n", __func__);

    seed = 23335;

    ch_pair_s16_t dataA[MAX_LEN];
    ch_pair_s16_t dataB[MAX_LEN];
    ch_pair_s16_t dataB_copy[MAX_LEN];
    bfp_ch_pair_s16_t A, B, B_copy;
    A.data = dataA;
    B.data = dataB;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_ch_pair_s16(&B, MAX_LEN, &seed, &A, 0);

        memcpy(&B_copy, &B, sizeof(B));
        memcpy(dataB_copy, dataB, sizeof(dataB));

        unsigned leave_hr = 1;

        bfp_ch_pair_s16_shl(&A, &B, B.hr-leave_hr);

        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.hr, B.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL(B_copy.data[i].ch_a << (B_copy.hr-leave_hr), A.data[i].ch_a);
            TEST_ASSERT_EQUAL(B_copy.data[i].ch_b << (B_copy.hr-leave_hr), A.data[i].ch_b);
        }

        int shl = -(pseudo_rand_uint32(&seed) % 5) - 1;

        memcpy(&B_copy, &A, sizeof(A));
        memcpy(dataB_copy, dataA, sizeof(dataA));

        bfp_ch_pair_s16_shl(&B, &A, shl);
        
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(-shl+leave_hr, B.hr);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL(B_copy.data[i].ch_a >> (-shl), B.data[i].ch_a);
            TEST_ASSERT_EQUAL(B_copy.data[i].ch_b >> (-shl), B.data[i].ch_b);
        }
    }
}





static void test_bfp_ch_pair_s32_shl()
{
    PRINTF("%s...\n", __func__);

    seed = 7775533;

    ch_pair_s32_t dataA[MAX_LEN];
    ch_pair_s32_t dataB[MAX_LEN];
    ch_pair_s32_t dataB_copy[MAX_LEN];
    bfp_ch_pair_s32_t A, B, B_copy;
    A.data = dataA;
    B.data = dataB;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_ch_pair_s32(&B, MAX_LEN, &seed, &A, 0);

        memcpy(&B_copy, &B, sizeof(B));
        memcpy(dataB_copy, dataB, sizeof(dataB));

        unsigned leave_hr = 1;

        bfp_ch_pair_s32_shl(&A, &B, B.hr-leave_hr);

        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.hr, B.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL(B_copy.data[i].ch_a << (B_copy.hr-leave_hr), A.data[i].ch_a);
            TEST_ASSERT_EQUAL(B_copy.data[i].ch_b << (B_copy.hr-leave_hr), A.data[i].ch_b);
        }

        int shl = -(pseudo_rand_uint32(&seed) % 5) - 1;

        memcpy(&B_copy, &A, sizeof(A));
        memcpy(dataB_copy, dataA, sizeof(dataA));

        bfp_ch_pair_s32_shl(&B, &A, shl);
        
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(-shl+leave_hr, B.hr);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL(B_copy.data[i].ch_a >> (-shl), B.data[i].ch_a);
            TEST_ASSERT_EQUAL(B_copy.data[i].ch_b >> (-shl), B.data[i].ch_b);
        }
    }
}








void test_bfp_complex_s16_shl()
{
    PRINTF("%s...\n", __func__);

    seed = 546457;

    struct {
        int16_t real[MAX_LEN];
        int16_t imag[MAX_LEN];
    } A_data, B_data;

    bfp_complex_s16_t A, B;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        B.length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);
        B.exp    = pseudo_rand_int(&seed, -30, 30);

        right_shift_t min_hr = pseudo_rand_uint(&seed,2, 8);

        for(int i = 0; i < B.length; i++){
            B_data.real[i] = pseudo_rand_int16(&seed) >> (min_hr + pseudo_rand_uint(&seed, 0, 5));
            B_data.imag[i] = pseudo_rand_int16(&seed) >> (min_hr + pseudo_rand_uint(&seed, 0, 5));
        }

        bfp_complex_s16_init(&B, B_data.real, B_data.imag, B.exp, B.length, 1);
        bfp_complex_s16_init(&A, A_data.real, A_data.imag,     0, B.length, 1);

        left_shift_t shl = pseudo_rand_uint(&seed, 1, min_hr);

        bfp_complex_s16_shl(&A, &B, shl);

        TEST_ASSERT_EQUAL(B.exp, A.exp);
        TEST_ASSERT_EQUAL(B.hr - shl, A.hr);

        for(int i = 0; i < B.length; i++){
            TEST_ASSERT_EQUAL(B.real[i] << shl, A.real[i]);
            TEST_ASSERT_EQUAL(B.imag[i] << shl, A.imag[i]);
        }
    }
}




void test_bfp_complex_s32_shl()
{
    PRINTF("%s...\n", __func__);

    seed = 546457;

    complex_s32_t A_data[MAX_LEN], B_data[MAX_LEN];
    bfp_complex_s32_t A, B;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        B.length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);
        B.exp    = pseudo_rand_int( &seed, -30, 30);

        right_shift_t min_hr = pseudo_rand_uint(&seed,2, 16);

        for(int i = 0; i < B.length; i++){
            B_data[i].re = pseudo_rand_int32(&seed) >> (min_hr + pseudo_rand_uint(&seed, 0, 10));
            B_data[i].im = pseudo_rand_int32(&seed) >> (min_hr + pseudo_rand_uint(&seed, 0, 10));
        }

        bfp_complex_s32_init(&B, B_data, B.exp, B.length, 1);
        bfp_complex_s32_init(&A, A_data,     0, B.length, 1);

        memset(A.data, 0xCC, sizeof(A_data));

        left_shift_t shl = pseudo_rand_uint(&seed, 1, min_hr);

        bfp_complex_s32_shl(&A, &B, shl);

        TEST_ASSERT_EQUAL(B.exp, A.exp);
        TEST_ASSERT_EQUAL(B.hr - shl, A.hr);

        for(int i = 0; i < B.length; i++){
            TEST_ASSERT_EQUAL(B.data[i].re << shl, A.data[i].re);
            TEST_ASSERT_EQUAL(B.data[i].im << shl, A.data[i].im);
        }
    }
}





static void test_bfp_complex_s16_shl_2()
{
    PRINTF("%s...\n", __func__);

    seed = 66777245;

    int16_t realA[MAX_LEN];
    int16_t imagA[MAX_LEN];
    int16_t realB[MAX_LEN];
    int16_t imagB[MAX_LEN];
    int16_t realB_copy[MAX_LEN];
    int16_t imagB_copy[MAX_LEN];
    bfp_complex_s16_t A, B, B_copy;
    A.real = realA;
    A.imag = imagA;
    B.real = realB;
    B.imag = imagB;
    B_copy.real = realB_copy;
    B_copy.imag = imagB_copy;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_complex_s16(&B, MAX_LEN, &seed, &A, 0);

        memcpy(&B_copy, &B, sizeof(B));
        memcpy(realB_copy, realB, sizeof(realB));
        memcpy(imagB_copy, imagB, sizeof(imagB));

        unsigned leave_hr = 1;

        bfp_complex_s16_shl(&A, &B, B.hr-leave_hr);

        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.hr, B.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL(B_copy.real[i] << (B_copy.hr-leave_hr), A.real[i]);
            TEST_ASSERT_EQUAL(B_copy.imag[i] << (B_copy.hr-leave_hr), A.imag[i]);
        }

        int shl = -(pseudo_rand_uint32(&seed) % 5) - 1;

        memcpy(&B_copy, &A, sizeof(A));
        memcpy(realB_copy, realA, sizeof(realA));
        memcpy(imagB_copy, imagA, sizeof(imagA));

        bfp_complex_s16_shl(&B, &A, shl);
        
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(-shl+leave_hr, B.hr);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL(B_copy.real[i] >> (-shl), B.real[i]);
            TEST_ASSERT_EQUAL(B_copy.imag[i] >> (-shl), B.imag[i]);
        }
    }
}





static void test_bfp_complex_s32_shl_2()
{
    PRINTF("%s...\n", __func__);

    seed = 85678488;

    complex_s32_t dataA[MAX_LEN];
    complex_s32_t dataB[MAX_LEN];
    complex_s32_t dataB_copy[MAX_LEN];
    bfp_complex_s32_t A, B, B_copy;
    A.data = dataA;
    B.data = dataB;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_complex_s32(&B, MAX_LEN, &seed, &A, 0);

        memcpy(&B_copy, &B, sizeof(B));
        memcpy(dataB_copy, dataB, sizeof(dataB));

        unsigned leave_hr = 1;

        bfp_complex_s32_shl(&A, &B, B.hr-leave_hr);

        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.hr, B.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL(B_copy.data[i].re << (B_copy.hr-leave_hr), A.data[i].re);
            TEST_ASSERT_EQUAL(B_copy.data[i].im << (B_copy.hr-leave_hr), A.data[i].im);
        }

        int shl = -(pseudo_rand_uint32(&seed) % 5) - 1;

        memcpy(&B_copy, &A, sizeof(A));
        memcpy(dataB_copy, dataA, sizeof(dataA));

        bfp_complex_s32_shl(&B, &A, shl);
        
        TEST_ASSERT_EQUAL(B_copy.length, B.length);
        TEST_ASSERT_EQUAL(B_copy.length, A.length);
        TEST_ASSERT_EQUAL(-shl+leave_hr, B.hr);
        TEST_ASSERT_EQUAL(leave_hr, A.hr);
        TEST_ASSERT_EQUAL(B_copy.exp, B.exp);
        TEST_ASSERT_EQUAL(B_copy.exp, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL(B_copy.data[i].re >> (-shl), B.data[i].re);
            TEST_ASSERT_EQUAL(B_copy.data[i].im >> (-shl), B.data[i].im);
        }
    }
}


void test_bfp_shl_vect()
{
    SET_TEST_FILE();

    RUN_TEST(test_bfp_s16_shl);
    RUN_TEST(test_bfp_s32_shl);
    
    RUN_TEST(test_bfp_ch_pair_s16_shl);
    RUN_TEST(test_bfp_ch_pair_s32_shl);

    RUN_TEST(test_bfp_complex_s16_shl);
    RUN_TEST(test_bfp_complex_s32_shl);
    
    RUN_TEST(test_bfp_complex_s16_shl_2);
    RUN_TEST(test_bfp_complex_s32_shl_2);
}