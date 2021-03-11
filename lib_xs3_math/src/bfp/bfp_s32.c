// Copyright 2020-2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.


#include "bfp_math.h"

#include "vect/xs3_vect_s32.h"
#include "vect/xs3_vect_s16.h"

#include <assert.h>
#include <stdio.h>


    
headroom_t bfp_s32_headroom(
    bfp_s32_t* a)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(a->length != 0);
#endif

     a->hr = xs3_vect_s32_headroom(a->data, a->length);

     return a->hr;
}


void bfp_s32_shl(
    bfp_s32_t* a,
    const bfp_s32_t* b,
    const left_shift_t shl)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(a->length == b->length);
    assert(b->length != 0);
#endif
    a->length = b->length;
    a->exp = b->exp;
    a->hr = xs3_vect_s32_shl(a->data, b->data, b->length, shl);
}


void bfp_s32_add(
    bfp_s32_t* a, 
    const bfp_s32_t* b, 
    const bfp_s32_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == c->length);
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    right_shift_t b_shr, c_shr;

    xs3_vect_add_sub_prepare(&a->exp, &b_shr, &c_shr, b->exp, c->exp, b->hr, c->hr);

    a->hr = xs3_vect_s32_add(a->data, b->data, c->data, b->length, b_shr, c_shr);
}


void bfp_s32_sub(
    bfp_s32_t* a, 
    const bfp_s32_t* b, 
    const bfp_s32_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == c->length);
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    right_shift_t b_shr, c_shr;

    xs3_vect_add_sub_prepare(&a->exp, &b_shr, &c_shr, b->exp, c->exp, b->hr, c->hr);

    a->hr = xs3_vect_s32_sub(a->data, b->data, c->data, b->length, b_shr, c_shr);
}


void bfp_s32_mul(
    bfp_s32_t* a, 
    const bfp_s32_t* b, 
    const bfp_s32_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == c->length);
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    right_shift_t b_shr, c_shr;
    xs3_vect_s32_mul_prepare(&a->exp, &b_shr, &c_shr, b->exp, c->exp, b->hr, c->hr); 

    a->hr = xs3_vect_s32_mul(a->data, b->data, c->data, b->length, b_shr, c_shr);
}


void bfp_s32_scale(
    bfp_s32_t* a, 
    const bfp_s32_t* b,
    const float_s32_t c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    right_shift_t b_shr, c_shr;

    headroom_t c_hr = HR_S32(c.mant);

    xs3_vect_s32_mul_prepare(&a->exp, &b_shr, &c_shr, b->exp, c.exp, b->hr, c_hr);

    a->hr = xs3_vect_s32_scale(a->data, b->data, b->length, c.mant, b_shr, c_shr);
}


void bfp_s32_abs(
    bfp_s32_t* a,
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    a->exp = b->exp;
    a->hr = xs3_vect_s32_abs(a->data, b->data, b->length);
}


float_s64_t bfp_s32_sum(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    float_s64_t a;
    a.mant = xs3_vect_s32_sum(b->data, b->length);
    a.exp = b->exp;
    return a;
}


float_s64_t bfp_s32_dot(
    const bfp_s32_t* b, 
    const bfp_s32_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == c->length);
    assert(b->length != 0);
#endif

    float_s64_t a;
    right_shift_t b_shr, c_shr;

    xs3_vect_s32_dot_prepare(&a.exp, &b_shr, &c_shr, b->exp, c->exp, b->hr, c->hr, b->length);

    a.mant = xs3_vect_s32_dot(b->data, c->data, b->length, b_shr, c_shr);
    return a;
}


void bfp_s32_clip(
    bfp_s32_t* a, 
    const bfp_s32_t* b, 
    const int32_t lower_bound, 
    const int32_t upper_bound, 
    const int bound_exp)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    assert(lower_bound <= upper_bound);

    // Suppose we say a->exp = b->exp. Then, we have to shift the bounds so that
    //  they match b->exp. So, bound_shr = b->exp - bound_exp. Two possibilities:
    //  A) bound_shr is negative (gets larger)
    //  B) bound_shr is non-negative (gets smaller (or stays same)

    // In case A, we shift the bound left. If upper_bound is positive and saturates, then of course all elements of b
    //  were already less than the upper bound. Likewise, if lower_bound is negative and saturates, then all elements of
    //  b were greater than the lower bound. If upper is negative and saturates or lower is positive and saturates, then
    //  we just set all of the elements of the output to upper or lower (accordingly), since nothing could possible be
    //  within the range specified.

    // In case B, we shift the bounds right, and we lose some precision on them, but that's it.
    
    exponent_t a_exp = b->exp;//minimum b exponent
    
    right_shift_t bound_shr = a_exp - bound_exp;
    right_shift_t b_shr = a_exp - b->exp;

    int32_t lb;
    int32_t ub;

    if(bound_shr < 0){
        int64_t ub64 = ((int64_t)upper_bound) << (-bound_shr);
        int64_t lb64 = ((int64_t)lower_bound) << (-bound_shr);

        ub = (ub64 >= VPU_INT32_MAX)? VPU_INT32_MAX : (ub64 <= VPU_INT32_MIN)? VPU_INT32_MIN : ub64;
        lb = (lb64 >= VPU_INT32_MAX)? VPU_INT32_MAX : (lb64 <= VPU_INT32_MIN)? VPU_INT32_MIN : lb64;
    } else {
        // TODO: Should force upper_bound to round downwards to enforce the guarantee that no output can be larger than 
        // upper bound?
        ub = upper_bound >> bound_shr;
        // And lower bound upwards?
        lb = (lower_bound + ((1<<bound_shr)-1)) >> bound_shr;
    }

    if(ub == VPU_INT32_MIN){
        /* upper bound must be smaller than any element of b, so set everything to that */
        a->exp = bound_exp;
        a->hr = HR_S32(upper_bound);
        xs3_vect_s32_set(a->data, upper_bound, b->length);
    } else if(lb == VPU_INT32_MAX){
        /* lower bound must be larger than any element of b, so set everything to that */
        a->exp = bound_exp;
        a->hr = HR_S32(lower_bound);
        xs3_vect_s32_set(a->data, lower_bound, b->length);
    } else if(ub == lb){
        /* upper and lower bounds are indistinguishable */
        a->exp = a_exp;
        a->hr = HR_S32(ub);
        xs3_vect_s32_set(a->data, ub, b->length);

    } else {
        /* some elements may be between bounds */
        a->exp = a_exp;
        a->hr = xs3_vect_s32_clip(a->data, b->data, b->length, lb, ub, b_shr);
    }
}


void bfp_s32_rect(
    bfp_s32_t* a,
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    a->exp = b->exp;
    a->hr = xs3_vect_s32_rect(a->data, b->data, b->length);
}


void bfp_s32_sqrt(
    bfp_s32_t* a,
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    right_shift_t b_shr;

    xs3_vect_s32_sqrt_prepare(&a->exp, &b_shr, b->exp, b->hr);

    a->hr = xs3_vect_s32_sqrt(a->data, b->data, b->length, b_shr, XS3_BFP_SQRT_DEPTH_S32);
}


void bfp_s32_inverse(
    bfp_s32_t* a,
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == a->length);
    assert(b->length != 0);
#endif

    unsigned scale;

    xs3_vect_s32_inverse_prepare(&a->exp, &scale, b->data, b->exp, b->length);

    a->hr = xs3_vect_s32_inverse(a->data, b->data, b->length, scale);
}


float_s64_t bfp_s32_abs_sum(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    float_s64_t a;
    a.mant = xs3_vect_s32_abs_sum(b->data, b->length);
    a.exp = b->exp;
    return a;
}


float_s32_t bfp_s32_mean(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    float_s32_t a;

    int64_t sum = xs3_vect_s32_sum(b->data, b->length);
    
    headroom_t hr = HR_S64(sum);
    sum = sum << hr;
    int64_t mean = sum / ((int)b->length);
    right_shift_t shr = MAX(0, 32 - HR_S64(mean));

    if(shr > 0)
        mean += 1 << (shr-1);
    
    a.mant = mean >> shr;
    a.exp = b->exp - hr + shr;

    return a;

}


float_s64_t bfp_s32_energy(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    float_s64_t a;
    right_shift_t b_shr;
    xs3_vect_s32_energy_prepare(&a.exp, &b_shr, b->length, b->exp, b->hr);
    a.mant = xs3_vect_s32_energy(b->data, b->length, b_shr);
    return a;
}


float_s32_t bfp_s32_rms(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    float_s32_t a;
    exponent_t exp, len_inv_exp;
    const float_s64_t energy64 = bfp_s32_energy(b);
    const int32_t energy32 = xs3_scalar_s64_to_s32(&exp, energy64.mant, energy64.exp);
    const int32_t len_inv = xs3_inverse_s32(&len_inv_exp, b->length);
    const int32_t mean_energy = xs3_mul_s32(&exp, energy32, len_inv, exp, len_inv_exp);

    a.mant = xs3_sqrt_s32(&a.exp, mean_energy, exp, XS3_BFP_SQRT_DEPTH_S32);
    return a;
}


float_s32_t bfp_s32_max(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    float_s32_t a;
    a.mant = xs3_vect_s32_max(b->data, b->length);
    a.exp = b->exp;
    return a;
}


float_s32_t bfp_s32_min(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    float_s32_t a;
    a.mant = xs3_vect_s32_min(b->data, b->length);
    a.exp = b->exp;
    return a;
}


unsigned bfp_s32_argmax(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    return xs3_vect_s32_argmax(b->data, b->length);
}


unsigned bfp_s32_argmin(
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length != 0);
#endif

    return xs3_vect_s32_argmin(b->data, b->length);
}


void bfp_s32_to_s16(
    bfp_s16_t* a,
    const bfp_s32_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS) // See xs3_math_conf.h
    assert(b->length == a->length);
    assert(b->length != 0);
#endif
    
    right_shift_t b_shr = 16 - b->hr;

    a->exp = b->exp + b_shr;
    a->hr = 0;

    xs3_vect_s32_to_s16(a->data, b->data, b->length, b_shr);
}
