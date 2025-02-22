// Copyright 2020-2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "xs3_math.h"

#include "../src/vect/vpu_helper.h"

#include "../../tst_common.h"

#include "unity.h"


static unsigned seed = 2314567;
static char msg_buff[200];

#define TEST_ASSERT_EQUAL_MSG(EXPECTED, ACTUAL, LINE_NUM)   do{       \
    if((EXPECTED)!=(ACTUAL)) {                                        \
      sprintf(msg_buff, "(test vector @ line %u)", (LINE_NUM));       \
      TEST_ASSERT_EQUAL_MESSAGE((EXPECTED), (ACTUAL), msg_buff);      \
    }} while(0)


#if DEBUG_ON || 0
#undef DEBUG_ON
#define DEBUG_ON    (1)
#endif


static complex_s16_t mul_complex_conj_s16(int16_t b_re, int16_t b_im, 
                                     int16_t c_re, int16_t c_im, 
                                     right_shift_t sat)
{
    int64_t bp_re = b_re;
    int64_t bp_im = b_im;
    int64_t cp_re = c_re;
    int64_t cp_im = c_im;

    int64_t a_re = (bp_re * cp_re + bp_im * cp_im);
    int64_t a_im = (bp_im * cp_re - bp_re * cp_im);

    a_re = SAT(16)(ROUND_SHR(a_re, sat));
    a_im = SAT(16)(ROUND_SHR(a_im, sat));

    complex_s16_t res = {a_re, a_im};
    return res;
}



static complex_s32_t mul_complex_conj_s32(complex_s32_t b, complex_s32_t c, int b_shr, int c_shr)
{
    int64_t bp_re = b.re;
    int64_t bp_im = b.im;
    int64_t cp_re = c.re;
    int64_t cp_im = c.im;

    bp_re = ASHR(32)(bp_re, b_shr);
    bp_im = ASHR(32)(bp_im, b_shr);

    cp_re = ASHR(32)(cp_re, c_shr);
    cp_im = ASHR(32)(cp_im, c_shr);

    bp_re = SAT(32)(bp_re);
    bp_im = SAT(32)(bp_im);

    cp_re = SAT(32)(cp_re);
    cp_im = SAT(32)(cp_im);

    // printf("B = %ld\t%ld\t\t%d\n", b.re, b.im, b_shr);
    // printf("C = %ld\t%ld\t\t%d\n", c.re, c.im, c_shr);

    int64_t q1 = ((bp_re * cp_re) + (1<<29)) >> 30;
    int64_t q2 = ((bp_im * cp_im) + (1<<29)) >> 30;
    int64_t q3 = ((bp_re * cp_im) + (1<<29)) >> 30;
    int64_t q4 = ((bp_im * cp_re) + (1<<29)) >> 30;

    int64_t a_re = (q1 + q2);
    int64_t a_im = (q4 - q3);

    // int64_t a_re = ((bp_re * cp_re - bp_im * cp_im) + (1<<29)) >> 30;
    // int64_t a_im = ((bp_re * cp_im + bp_im * cp_re) + (1<<29)) >> 30;

    a_re = SAT(32)(a_re);
    a_im = SAT(32)(a_im);

    complex_s32_t res = {a_re, a_im};
    return res;
}















static void test_xs3_vect_complex_s16_conj_mul_basic()
{
    PRINTF("%s...\n", __func__);

    typedef struct {
        struct {    complex_s16_t b;  
                    complex_s16_t c;  } value;
        right_shift_t sat;
        complex_s16_t expected;
        unsigned line;
    } test_case_t;

    test_case_t casses[] = {
        // value{ b{     re,     im},  c{     re,     im} },   sat,  exp{     re,     im},    line num
        {       {  { 0x0000, 0x0000},   { 0x0000,-0x0000} },     0,     { 0x0000, 0x0000},    __LINE__},
        {       {  { 0x0001, 0x0001},   { 0x0000,-0x0000} },     0,     { 0x0000, 0x0000},    __LINE__},
        {       {  { 0x0000, 0x0000},   { 0x0001,-0x0001} },     0,     { 0x0000, 0x0000},    __LINE__},
        {       {  { 0x0001, 0x0000},   { 0x0001,-0x0000} },     0,     { 0x0001, 0x0000},    __LINE__},
        {       {  { 0x0001, 0x0001},   { 0x0001, 0x0001} },     0,     { 0x0002, 0x0000},    __LINE__},
        {       {  {-0x0001, 0x0001},   {-0x0001,-0x0001} },     0,     { 0x0000,-0x0002},    __LINE__},
        {       {  { 0x0010, 0x0020},   { 0x0030,-0x0040} },     0,     {-0x0500, 0x0a00},    __LINE__},
        {       {  { 0x0010, 0x0020},   { 0x0030,-0x0040} },     4,     {-0x0050, 0x00a0},    __LINE__},
        {       {  {-0x4000,-0x4000},   {-0x4000, 0x4000} },     0,     { 0x0000, 0x7fff},    __LINE__},
        {       {  {-0x4000,-0x4000},   {-0x4000, 0x4000} },    15,     { 0x0000, 0x4000},    __LINE__},


    };

    const unsigned N_cases = sizeof(casses)/sizeof(test_case_t);

    const unsigned start_case = 0;

    for(int v = start_case; v < N_cases; v++){
        PRINTF("\ttest vector %d..\n", v);
        
        test_case_t* casse = &casses[v];

        //Verify mul_complex_s16() is correct. It's used in other test cases.
        complex_s16_t tmp = mul_complex_conj_s16(casse->value.b.re, casse->value.b.im, 
                                            casse->value.c.re, casse->value.c.im, 
                                            casse->sat);

        TEST_ASSERT_EQUAL_MSG(casse->expected.re, tmp.re, casse->line);
        TEST_ASSERT_EQUAL_MSG(casse->expected.im, tmp.im, casse->line);

        unsigned lengths[] = {1, 4, 16, 32, 40 };
        

        for( int l = 0; l < sizeof(lengths)/sizeof(lengths[0]); l++){
            unsigned len = lengths[l];

            headroom_t hr;
            struct { 
                int16_t real[40]; 
                int16_t imag[40]; 
            } A, B, C;

            for(int i = 0; i < len; i++){
                A.real[i] = 0xCC;
                A.imag[i] = 0xCC;
                B.real[i] = casse->value.b.re;
                B.imag[i] = casse->value.b.im;
                C.real[i] = casse->value.c.re;
                C.imag[i] = casse->value.c.im;
            }

            hr = xs3_vect_complex_s16_conj_mul(A.real, A.imag, 
                                                  B.real, B.imag, 
                                                  C.real, C.imag, 
                                                  len, casse->sat);
            headroom_t hrre, hrim;

            for(int i = 0; i < len; i++){
                TEST_ASSERT_EQUAL_MSG(casse->expected.re, A.real[0], casse->line);
                TEST_ASSERT_EQUAL_MSG(casse->expected.im, A.imag[0], casse->line);
            }
            hrre = xs3_vect_s16_headroom(A.real, len); hrim = xs3_vect_s16_headroom(A.imag, len);
            TEST_ASSERT_EQUAL_MSG((hrre <= hrim)? hrre : hrim, hr, casse->line);

            memcpy(&A, &B, sizeof(A));
            hr = xs3_vect_complex_s16_conj_mul(A.real, A.imag,
                                                  A.real, A.imag,
                                                  C.real, C.imag,
                                                  len, casse->sat);

            for(int i = 0; i < len; i++){
                TEST_ASSERT_EQUAL_MSG(casse->expected.re, A.real[0], casse->line);
                TEST_ASSERT_EQUAL_MSG(casse->expected.im, A.imag[0], casse->line);
            }
            hrre = xs3_vect_s16_headroom(A.real, len); hrim = xs3_vect_s16_headroom(A.imag, len);
            TEST_ASSERT_EQUAL_MSG((hrre <= hrim)? hrre : hrim, hr, casse->line);

            /* NOTE: It is NOT safe to operate in-place with A == C */

        }
    }
}




#define MAX_LEN     100
#define REPS        (100)
static void test_xs3_vect_complex_s16_conj_mul_random()
{
    PRINTF("%s...\n", __func__);
    seed = 0xAD04D98D;

    headroom_t hr;
    
    struct { 
        int16_t real[MAX_LEN]; 
        int16_t imag[MAX_LEN]; 
    } A, B, C;

    for(int v = 0; v < REPS; v++){

        PRINTF("\trepetition % 3d..\t(seed: 0x%08X)\n", v, seed);

        unsigned len = (pseudo_rand_uint32(&seed) % MAX_LEN) + 1;
        
        for(int i = 0; i < len; i++){
            unsigned shr = pseudo_rand_uint32(&seed) % 8;
            B.real[i] = pseudo_rand_int16(&seed) >> shr;
            C.real[i] = pseudo_rand_int16(&seed) >> shr;
            B.imag[i] = pseudo_rand_int16(&seed) >> shr;
            C.imag[i] = pseudo_rand_int16(&seed) >> shr;
        }

        int sat = (pseudo_rand_uint32(&seed) % 10);
        
        hr = xs3_vect_complex_s16_conj_mul(A.real, A.imag, 
                                              B.real, B.imag, 
                                              C.real, C.imag, 
                                              len, sat);

        headroom_t hrre, hrim;

        for(int i = 0; i < len; i++){
            complex_s16_t expected = mul_complex_conj_s16(B.real[i], B.imag[i], C.real[i], C.imag[i], sat);
            
            TEST_ASSERT_EQUAL_MESSAGE(expected.re, A.real[i], msg_buff);
            TEST_ASSERT_EQUAL_MESSAGE(expected.im, A.imag[i], msg_buff);
        }
        hrre = xs3_vect_s16_headroom(A.real, len); hrim = xs3_vect_s16_headroom(A.imag, len);
        TEST_ASSERT_EQUAL_MSG((hrre <= hrim)? hrre : hrim, hr, v);
        
        memcpy(&A, &B, sizeof(A));
        hr = xs3_vect_complex_s16_conj_mul(A.real, A.imag, 
                                              A.real, A.imag, 
                                              C.real, C.imag, 
                                              len, sat);

        for(int i = 0; i < len; i++){
            complex_s16_t expected = mul_complex_conj_s16(B.real[i], B.imag[i], C.real[i], C.imag[i], sat);
            TEST_ASSERT_EQUAL_MESSAGE(expected.re, A.real[i], msg_buff);
            TEST_ASSERT_EQUAL_MESSAGE(expected.im, A.imag[i], msg_buff);
        }
        hrre = xs3_vect_s16_headroom(A.real, len); hrim = xs3_vect_s16_headroom(A.imag, len);
        TEST_ASSERT_EQUAL_MSG((hrre <= hrim)? hrre : hrim, hr, v);
        
        /* NOT safe to do A==C in place !! */

        
    }
}
#undef MAX_LEN
#undef REPS










static void test_xs3_vect_complex_s32_conj_mul_basic()
{
    PRINTF("%s...\n", __func__);

    typedef struct {
        struct {    complex_s32_t b;  
                    complex_s32_t c;  } value;
        struct {    int b;      int c;      } shr;
        complex_s32_t expected;
        unsigned line;
    } test_case_t;

    test_case_t casses[] = {
        //{{   val.b.re,   val.b.im}, {   val.c.re,   val.c.im} }, shr{   b,   c}, {     exp.re,     exp.im}, line num
        { {{ 0x00000000, 0x00000000}, { 0x00000000,-0x00000000} },    {   0,   0}, { 0x00000000, 0x00000000}, __LINE__},
        { {{ 0x00000001, 0x00000001}, { 0x00000000,-0x00000000} },    {   0,   0}, { 0x00000000, 0x00000000}, __LINE__},
        { {{ 0x00000000, 0x00000000}, { 0x00000001,-0x00000001} },    { -20, -20}, { 0x00000000, 0x00000000}, __LINE__},
        { {{ 0x00000001, 0x00000000}, { 0x00000001,-0x00000000} },    { -15, -15}, { 0x00000001, 0x00000000}, __LINE__},
        { {{ 0x00000001, 0x00000000}, { 0x00000001,-0x00000000} },    { -16, -15}, { 0x00000002, 0x00000000}, __LINE__},
        { {{ 0x00000001, 0x00000000}, { 0x00000001,-0x00000000} },    { -15, -16}, { 0x00000002, 0x00000000}, __LINE__},
        { {{ 0x00000001, 0x00000000}, { 0x00000000,-0x00000001} },    { -15, -15}, { 0x00000000, 0x00000001}, __LINE__},
        { {{ 0x00000001, 0x00000001}, { 0x00000001,-0x00000000} },    { -15, -15}, { 0x00000001, 0x00000001}, __LINE__},
        { {{ 0x00000010, 0x00000020}, { 0x00000030,-0x00000040} },    { -17, -17}, {-0x00005000, 0x0000a000}, __LINE__},
        { {{ 0x40000000, 0x00000000}, { 0x40000000,-0x00000000} },    {   0,   0}, { 0x40000000, 0x00000000}, __LINE__},
        { {{ 0x00000000, 0x40000000}, { 0x00000000,-0x40000000} },    {   0,   0}, {-0x40000000, 0x00000000}, __LINE__},
    };

    const unsigned N_cases = sizeof(casses)/sizeof(test_case_t);

    const unsigned start_case = 0;

    for(int v = start_case; v < N_cases; v++){
        PRINTF("\ttest vector %d..\n", v);
        
        test_case_t* casse = &casses[v];

        //Verify mul__complexs32() is correct. It's used in other test cases.
        complex_s32_t tmp = mul_complex_conj_s32(casse->value.b, casse->value.c, casse->shr.b, casse->shr.c);

        TEST_ASSERT_EQUAL_MSG(casse->expected.re, tmp.re, casse->line);
        TEST_ASSERT_EQUAL_MSG(casse->expected.im, tmp.im, casse->line);

        unsigned lengths[] = {1, 4, 16, 32, 40 };
        

        for( int l = 0; l < sizeof(lengths)/sizeof(lengths[0]); l++){
            unsigned len = lengths[l];

            headroom_t hr;
            complex_s32_t A[40];
            complex_s32_t B[40];
            complex_s32_t C[40];

            for(int i = 0; i < len; i++){
                A[i].re = 0xCC;
                A[i].im = 0xCC;
                B[i].re = casse->value.b.re;
                B[i].im = casse->value.b.im;
                C[i].re = casse->value.c.re;
                C[i].im = casse->value.c.im;
            }

            hr = xs3_vect_complex_s32_conj_mul(A, B, C, len, casse->shr.b, casse->shr.c);

            for(int i = 0; i < len; i++){
                TEST_ASSERT_EQUAL_MSG(casse->expected.re, A[0].re, casse->line);
                TEST_ASSERT_EQUAL_MSG(casse->expected.im, A[0].im, casse->line);
            }
            TEST_ASSERT_EQUAL_MSG(xs3_vect_s32_headroom((int32_t*) A, 2*len), hr, casse->line);

            memcpy(A, B, sizeof(A));
            hr = xs3_vect_complex_s32_conj_mul(A, A, C, len, casse->shr.b, casse->shr.c);

            for(int i = 0; i < len; i++){
                TEST_ASSERT_EQUAL_MSG(casse->expected.re, A[0].re, casse->line);
                TEST_ASSERT_EQUAL_MSG(casse->expected.im, A[0].im, casse->line);
            }
            TEST_ASSERT_EQUAL_MSG(xs3_vect_s32_headroom((int32_t*) A, 2*len), hr, casse->line);

            memcpy(A, C, sizeof(A));
            hr = xs3_vect_complex_s32_conj_mul(A, B, A, len, casse->shr.b, casse->shr.c);

            for(int i = 0; i < len; i++){
                TEST_ASSERT_EQUAL_MSG(casse->expected.re, A[0].re, casse->line);
                TEST_ASSERT_EQUAL_MSG(casse->expected.im, A[0].im, casse->line);
            }
            TEST_ASSERT_EQUAL_MSG(xs3_vect_s32_headroom((int32_t*) A, 2*len), hr, casse->line);
        }
    }
}







#define MAX_LEN     100
#define REPS        (100)
static void test_xs3_vect_complex_s32_conj_mul_random()
{
    PRINTF("%s...\n", __func__);
    seed = 0xAD04D98D;

    headroom_t hr;
    
    complex_s32_t A[MAX_LEN];
    complex_s32_t B[MAX_LEN];
    complex_s32_t C[MAX_LEN];

    for(int v = 0; v < REPS; v++){

        PRINTF("\trepetition % 3d..\t(seed: 0x%08X)\n", v, seed);

        unsigned len = (pseudo_rand_uint32(&seed) % MAX_LEN) + 1;
        
        for(int i = 0; i < len; i++){
            unsigned shr = pseudo_rand_uint32(&seed) % 8;
            B[i].re = pseudo_rand_int32(&seed) >> shr;
            C[i].re = pseudo_rand_int32(&seed) >> shr;
            B[i].im = pseudo_rand_int32(&seed) >> shr;
            C[i].im = pseudo_rand_int32(&seed) >> shr;
        }

        int b_shr = (pseudo_rand_uint32(&seed) % 10);
        int c_shr = (pseudo_rand_uint32(&seed) % 10);
        
        hr = xs3_vect_complex_s32_conj_mul(A, B, C, len, b_shr, c_shr);

        headroom_t hrre, hrim;

        for(int i = 0; i < len; i++){
            complex_s32_t expected = mul_complex_conj_s32(B[i], C[i], b_shr, c_shr);
            TEST_ASSERT_EQUAL_MESSAGE(expected.re, A[i].re, msg_buff);
            TEST_ASSERT_EQUAL_MESSAGE(expected.im, A[i].im, msg_buff);
        }
        TEST_ASSERT_EQUAL_MSG(xs3_vect_s32_headroom((int32_t*) A, 2*len), hr, v);
        
        memcpy(&A, &B, sizeof(A));
        hr = xs3_vect_complex_s32_conj_mul(A, A, C, len, b_shr, c_shr);

        for(int i = 0; i < len; i++){
            complex_s32_t expected = mul_complex_conj_s32(B[i], C[i], b_shr, c_shr);
            TEST_ASSERT_EQUAL_MESSAGE(expected.re, A[i].re, msg_buff);
            TEST_ASSERT_EQUAL_MESSAGE(expected.im, A[i].im, msg_buff);
        }
        TEST_ASSERT_EQUAL_MSG(xs3_vect_s32_headroom((int32_t*) A, 2*len), hr, v);
        
        memcpy(&A, &C, sizeof(A));
        hr = xs3_vect_complex_s32_conj_mul(A, B, A, len, b_shr, c_shr);

        for(int i = 0; i < len; i++){
            complex_s32_t expected = mul_complex_conj_s32(B[i], C[i], b_shr, c_shr);
            TEST_ASSERT_EQUAL_MESSAGE(expected.re, A[i].re, msg_buff);
            TEST_ASSERT_EQUAL_MESSAGE(expected.im, A[i].im, msg_buff);
        }
        TEST_ASSERT_EQUAL_MSG(xs3_vect_s32_headroom((int32_t*) A, 2*len), hr, v);

        
    }
}
#undef MAX_LEN
#undef REPS



void test_xs3_complex_conj_mul_vect_complex()
{
    SET_TEST_FILE();

    RUN_TEST(test_xs3_vect_complex_s16_conj_mul_basic);
    RUN_TEST(test_xs3_vect_complex_s16_conj_mul_random);

    RUN_TEST(test_xs3_vect_complex_s32_conj_mul_basic);
    RUN_TEST(test_xs3_vect_complex_s32_conj_mul_random);
}