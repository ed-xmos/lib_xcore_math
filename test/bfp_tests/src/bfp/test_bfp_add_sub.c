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


#define REPS        1000
#define MAX_LEN     18  //Smaller lengths mean larger variance w.r.t. individual element headroom


static unsigned seed = 666;
static char msg_buff[200];

#define TEST_ASSERT_EQUAL_MSG(EXPECTED, ACTUAL, EXTRA, LINE_NUM)   do{          \
    if((EXPECTED)!=(ACTUAL)) {                                                  \
      sprintf(msg_buff, "%s (test vector @ line %u)", (EXTRA), (LINE_NUM));     \
      TEST_ASSERT_EQUAL_MESSAGE((EXPECTED), (ACTUAL), msg_buff);                \
    }} while(0)





static void test_bfp_s16_add()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 8788764;

    int16_t dataA[MAX_LEN];
    int16_t dataB[MAX_LEN];
    int16_t dataC[MAX_LEN];
    int16_t expA[MAX_LEN];
    bfp_s16_t A, B, C;

    A.data = dataA;
    B.data = dataB;
    C.data = dataC;

    double Af[MAX_LEN];
    double Bf[MAX_LEN];
    double Cf[MAX_LEN];

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_s16(&B, MAX_LEN, &seed, &A, 0);
        test_random_bfp_s16(&C, MAX_LEN, &seed, &A, B.length);

        test_double_from_s16(Bf, &B);
        test_double_from_s16(Cf, &C);

        for(int i = 0; i < B.length; i++){
            Af[i] = Bf[i] + Cf[i];
        }

        bfp_s16_add(&A, &B, &C);

        test_s16_from_double(expA, Af, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_INT16_WITHIN(1, expA[i], A.data[i]);
        }
    }
}








static void test_bfp_s32_add()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 23452342;

    int32_t dataA[MAX_LEN];
    int32_t dataB[MAX_LEN];
    int32_t dataC[MAX_LEN];
    int32_t expA[MAX_LEN];
    bfp_s32_t A, B, C;

    A.data = dataA;
    B.data = dataB;
    C.data = dataC;

    double Af[MAX_LEN];
    double Bf[MAX_LEN];
    double Cf[MAX_LEN];

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_s32(&B, MAX_LEN, &seed, &A, 0);
        test_random_bfp_s32(&C, MAX_LEN, &seed, &A, B.length);

        test_double_from_s32(Bf, &B);
        test_double_from_s32(Cf, &C);

        for(int i = 0; i < B.length; i++){
            Af[i] = Bf[i] + Cf[i];
        }

        bfp_s32_add(&A, &B, &C);

        test_s32_from_double(expA, Af, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_INT32_WITHIN(1, expA[i], A.data[i]);
        }
    }
}





static void test_bfp_s16_sub()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 8788764;

    int16_t dataA[MAX_LEN];
    int16_t dataB[MAX_LEN];
    int16_t dataC[MAX_LEN];
    int16_t expA[MAX_LEN];
    bfp_s16_t A, B, C;

    A.data = dataA;
    B.data = dataB;
    C.data = dataC;

    double Af[MAX_LEN];
    double Bf[MAX_LEN];
    double Cf[MAX_LEN];

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_s16(&B, MAX_LEN, &seed, &A, 0);
        test_random_bfp_s16(&C, MAX_LEN, &seed, &A, B.length);

        test_double_from_s16(Bf, &B);
        test_double_from_s16(Cf, &C);

        for(int i = 0; i < B.length; i++){
            Af[i] = Bf[i] - Cf[i];
        }

        bfp_s16_sub(&A, &B, &C);

        test_s16_from_double(expA, Af, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_INT16_WITHIN(1, expA[i], A.data[i]);
        }
    }
}





static void test_bfp_s32_sub()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 8788764;

    int32_t dataA[MAX_LEN];
    int32_t dataB[MAX_LEN];
    int32_t dataC[MAX_LEN];
    int32_t expA[MAX_LEN];
    bfp_s32_t A, B, C;

    A.data = dataA;
    B.data = dataB;
    C.data = dataC;

    double Af[MAX_LEN];
    double Bf[MAX_LEN];
    double Cf[MAX_LEN];

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_s32(&B, MAX_LEN, &seed, &A, 0);
        test_random_bfp_s32(&C, MAX_LEN, &seed, &A, B.length);

        test_double_from_s32(Bf, &B);
        test_double_from_s32(Cf, &C);

        for(int i = 0; i < B.length; i++){
            Af[i] = Bf[i] - Cf[i];
        }

        bfp_s32_sub(&A, &B, &C);

        test_s32_from_double(expA, Af, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_INT32_WITHIN(1, expA[i], A.data[i]);
        }
    }
}





void test_bfp_add_sub_vect()
{
    SET_TEST_FILE();

    RUN_TEST(test_bfp_s16_add);
    RUN_TEST(test_bfp_s32_add);
    
    
    RUN_TEST(test_bfp_s16_sub);
    RUN_TEST(test_bfp_s32_sub);
}