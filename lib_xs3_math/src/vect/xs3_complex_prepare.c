// Copyright 2020-2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>

#include "xs3_math.h"




////////////////////////////////////////
//  Shared params (16- and 32-bit)    //
////////////////////////////////////////


void xs3_vect_complex_mag_prepare(
    exponent_t* a_exp,
    right_shift_t* b_shr,
    const exponent_t b_exp,
    const headroom_t b_hr)
{
    // Needs 1 bit of headroom. For 16 bits, consider that complex mantissa values near (INT16_MAX + INT16_MAX*j) can't 
    // be rotated to the real axis without going beyond the range of a 16-bit integer.
    *b_shr = 1-(int)b_hr;
    *a_exp = b_exp + *b_shr;
}


////////////////////////////////////////
//      Params for 16-bit             //
////////////////////////////////////////




void xs3_vect_complex_s16_real_mul_prepare(
    exponent_t* a_exp,
    right_shift_t* sat,
    const exponent_t b_exp,
    const exponent_t c_exp,
    const headroom_t b_hr,
    const headroom_t c_hr)
{
    /*

    Input elements X are in the range
        -2^15 <= X < 2^15
    This range is right-shifted by the headroom of the corresponding input.

    If neither input has any headroom, then the 32-bit product P's possible range is:
        -2^15 * 2^15 < P <= -2^15 * -2^15
        -2^30 < P <= 2^30
    
    The product's range is right-shifted by the total headroom (b_hr + c_hr):
        -2^(30-b_hr-c_hr) < P <= 2^(30-b_hr-c_hr)

    The 32-bit product is divided by 2^(sat). sat should be chosen to minimize the result's
    headroom in that worst case.

    Since the result R must fit in 16 bits, that means
        sat = 15 - (b_hr + c_hr)
    will leave the result in the range
        -2^(30-b_hr-c_hr) / 2^(15-(b_hr+c_hr)) < R <= 2^(30-b_hr-c_hr) / 2^(15-(b_hr+c_hr))
        -2^15 < R <= 2^15

    Slightly annoyingly, 2^15 can't actually fit in a signed 16-bit integer:
        INT16_MAX = (2^15)-1

    Due to the symmetric saturation logic in the VPU, -2^15 cannot be output either.

    If one pair of inputs happens to be  -2^15 and (2^15)-1, with a sat value of 15, the 
     (rounded) result prior to saturation ends up being:

        ((-2^15 * ((2^15)-1) + 2^14) >> 15
      = (2^15*(-2^15) - 1*(-2^15) + 2^14) >> 15
      = (-2^30 + 2^15 + 2^14) >> 15
      = (-0x40000000 + 0xC000) >> 15
      = ((int32_t) 0xC000C000) >> 15
      = -0x7FFF
    
        (adding the 2^14 followed by a truncating shift is equivalent to the rounding
         operation applied by the VPU)
    Which, conveniently, is the lower saturation bound.

    So, saturation can only occur on the positive end. If the 'allow saturation' parameter is
    true, the output values for 'a_exp' and 'sat' will allow for the possibility of saturation.
    Otherwise, that will be explicitly avoided. (Beware: avoiding saturation effectively loses
    a bit of precision).

    Note that if the vector inputs B and C to bfp_complex_s16_real_mul() are the result of a previous VPU 
    operation, the saturating condition cannot occur, because the VPU cannot output -0x8000. In that case
    there's no reason not to allow saturation.
    */

    *sat = 15 - (b_hr + c_hr);

    // sat must be non-negative.
    if(*sat < 0)
        *sat = 0;

    // Output exponent is the sum of input exponents plus however many bits we had to
    // right-shift the 32-bit product (shifting the mantissa down one bit is balanced by
    // adding one to the exponent)
    *a_exp = b_exp + c_exp + *sat;
}

void xs3_vect_complex_s16_mul_prepare(
    exponent_t* a_exp,
    right_shift_t* sat,
    const exponent_t b_exp,
    const exponent_t c_exp,
    const headroom_t b_hr,
    const headroom_t c_hr)
{
    /*
        B[] has b_hr headroom, and C[] has c_hr headroom, so suppose
            B[0] = (-2^15 - j*2^15) >> b_hr
            C[0] = (-2^15 - j*2^15) >> c_hr
            
        Then, we want to find the value of sat that makes its imaginary part 0x8000 (or 0x4000 if saturation is not
         allowed).

            A[0].im = ( B[0].re * C[0].im + B[0].im * C[0].re ) >> sat
                    = ( -2^15 * 2^(-b_hr) * -2^15 * 2^(-c_hr) + -2^15 * 2^(-b_hr) * -2^15 * 2^(-c_hr) ) >> sat
                    = ( 2^30 * 2^(-b_hr-c_hr) + 2^30 * 2^(-b_hr-c_hr) ) * 2^(-sat)
                    = ( 2^(30-b_hr-c_hr) + 2^(30-b_hr-c_hr) ) * 2^(-sat)
                    = 2^(31-b_hr-c_hr) * 2^(-sat)
                    = 2^(31-b_hr-c_hr-sat)

            total_hr = b_hr + c_hr

            0x8000 = 2^(31-total_hr-sat)
            2^15 = 2^(31-total_hr-sat)
            15 = 31 - total_hr - sat
            sat = 31 - 15 - total_hr
            sat = 16 - total_hr
            
    */


    headroom_t total_hr = b_hr+c_hr;

    *sat = MAX(0, (int) (16 - total_hr) );

    *a_exp = b_exp + c_exp + *sat;
}

void xs3_vect_complex_s16_squared_mag_prepare(
    exponent_t* a_exp,
    right_shift_t* sat,
    const exponent_t b_exp,
    const headroom_t b_hr)
{
    /*
        Operation is
            A = (B.re * B.re + B.im * B.im) >> sat

        Maximum result is obtained when  B.re = B.im = -0x8000 >> b_hr

        And we want the result for that case to be 0x7FFF (0x8000 before saturation)

            0x8000 = ( (-0x8000 >> b_hr) * (-0x8000 >> b_hr) + (-0x8000 >> b_hr) * (-0x8000 >> b_hr) ) >> sat
            2^15 = ( (-2*15 * 2^-b_hr) * (-2*15 * 2^-b_hr) + (-2*15 * 2^-b_hr) * (-2*15 * 2^-b_hr) )*2^-sat
            2^15 = ( 2^(30-2*b_hr) + 2^(30-2*b_hr) ) * 2^-sat
            2^15 = 2^1 * 2^(30-2*b_hr) * 2^-sat
            2^15 = 2^(31-2*b_hr-sat)
            15 = 31 - 2*b_hr - sat
            sat = 31 - 2*b_hr - 15
            sat = (16) - 2*b_hr
            sat = 16 - 2*b_hr
    */

   *sat = MAX(0, 16 - 2*((int)b_hr));

   *a_exp = 2 * b_exp + *sat;
}


////////////////////////////////////////
//      Params for 32-bit             //
////////////////////////////////////////




void xs3_vect_complex_s32_real_mul_prepare(
    exponent_t* a_exp,
    right_shift_t* b_shr,
    right_shift_t* c_shr,
    const exponent_t b_exp,
    const exponent_t c_exp,
    const headroom_t b_hr,
    const headroom_t c_hr)
{

    /*

        2^31 = ( -2^31 * 2^-b_hr * 2^-b_shr * -2^31 * 2^-c_hr * 2^-c_shr ) * 2^-30
        2^31 = ( 2^(31+31-b_hr-b_shr-c_hr-c_shr) ) * 2^-30
        2^31 = 2^(62-b_hr-b_shr-c_hr-c_shr) * 2^-30
        2^31 = 2^(32-b_hr-b_shr-c_hr-c_shr)
        31 = 32-b_hr-b_shr-c_hr-c_shr
            total_hr = b_hr + c_hr
            total_shr = b_shr + b_shr
        31 = 32-total_hr-total_shr
        total_shr = (2-1) - total_hr
        total_shr = (1) - total_hr

    */
    headroom_t total_hr = b_hr + c_hr;

    right_shift_t total_shr = 1 - total_hr;

    if(b_hr <= c_hr)
        *b_shr = total_shr - (total_shr >> 1);
    else 
        *b_shr = (total_shr >> 1);

    *c_shr = total_shr - *b_shr;
    *a_exp = b_exp + c_exp + *b_shr + *c_shr + 30;
}

void xs3_vect_complex_s32_mul_prepare(
    exponent_t* a_exp,
    right_shift_t* b_shr,
    right_shift_t* c_shr,
    const exponent_t b_exp,
    const exponent_t c_exp,
    const headroom_t b_hr,
    const headroom_t c_hr)
{
    /*

        2^31 = ( -2^31 * 2^-b_hr * 2^-b_shr * -2^31 * 2^-c_hr * 2^-c_shr 
                            + -2^31 * 2^-b_hr * 2^-b_shr * -2^31 * 2^-c_hr * 2^-c_shr ) * 2^-30
        2^(31) = ( 2^(31+31-b_hr-b_shr-c_hr-c_shr) + 2^(31+31-b_hr-b_shr-c_hr-c_shr) ) * 2^-30
        2^(31) = 2 * 2^(62-b_hr-b_shr-c_hr-c_shr) * 2^-30
        2^(31) = 2^(33-b_hr-b_shr-c_hr-c_shr)
        31 = 33-b_hr-b_shr-c_hr-c_shr
            total_hr = b_hr + c_hr
            total_shr = b_shr + b_shr
        31 = 33-total_hr-total_shr
        total_shr = (2) - total_hr

    */
    headroom_t total_hr = b_hr + c_hr;

    right_shift_t total_shr = 2 - total_hr;

    //  total_shr <= 3
    
    if(total_shr < 0){
        *b_shr = MAX(total_shr, ((int)-b_hr));
    } else {
        if( b_hr <= c_hr )
            *b_shr = total_shr - (total_shr >> 1);
        else
            *b_shr = (total_shr >> 1);
    }

    *c_shr = total_shr - *b_shr;
    *a_exp = b_exp + c_exp + *b_shr + *c_shr + 30;
}


void xs3_vect_complex_s32_scale_prepare(
    exponent_t* a_exp,
    right_shift_t* b_shr,
    right_shift_t* c_shr,
    const exponent_t b_exp,
    const exponent_t c_exp,
    const headroom_t b_hr,
    const headroom_t c_hr)
{
    /*
        = (-2^31 * 2^(-b_hr-b_shr) * -2^31 * 2^(-c_hr) + 2^31 * 2^(-b_hr-b_shr) * 2^31 * 2^(-c_hr)) >> 30
        = (2^(62 - b_hr - b_shr - c_hr) + 2^(62 - b_hr - b_shr - c_hr) ) >> 30
        = 2^(63 - b_hr - c_hr - b_shr) * 2^(-30)
        = 2^(33 - total_hr - b_shr)
        = 2^31 * 2^(2 - total_hr - b_shr)
        = 0x80000000 * 2^(2 - total_hr - b_shr)
        
        0 = 2-total_hr-b_shr
        b_shr = 2-total_hr
    */

    headroom_t total_hr = b_hr + c_hr;
    *b_shr = 2 - total_hr;
    *c_shr = 0;

    // We can't left-shift B more than b_hr bits, so the
    // rest must come from alpha
    if( *b_shr < ((int)-b_hr) ){
        *c_shr = *b_shr + b_hr;
        *b_shr = -b_hr;
    }

    *a_exp = b_exp + c_exp + 30 + *b_shr + *c_shr;
}


void xs3_vect_complex_s32_squared_mag_prepare(
    exponent_t* a_exp,
    right_shift_t* b_shr,
    const exponent_t b_exp,
    const headroom_t b_hr)
{
    /*
        Operation is
            A = ((B.re >> b_shr) * (B.re >> B_shr) + (B.im >> B_shr) * (B.im >> B_shr) ) >> 30

        Maximum result is obtained when  B.re = B.im = -0x80000000 >> b_hr = -2^31 * 2^-b_hr

        And we want the result for that case to be 0x7FFFFFFF (0x80000000 before saturation).

            0x80000000 = ( (-2^31 * 2^-b_hr * 2^-b_shr) * (-2^31 * 2^-b_hr * 2^-b_shr) 
                                    + (-2^31 * 2^-b_hr * 2^-b_shr) * (-2^31 * 2^-b_hr * 2^-b_shr) ) >> 30
            2^31 = ( 2^1 * 2^( 31 - b_hr - b_shr + 31 - b_hr - b_shr ) ) * 2^-30
            2^31 = ( 2^( 63 - 2*b_hr - 2*b_shr ) * 2^-30
            2^31 = 2^( 33 - 2*b_hr - 2*b_shr
            31 = 33 - 2*b_hr - 2*b_shr
            2*b_shr = 33 - 2*b_hr - 31
            2*b_shr = 2 - 2*b_hr
            b_shr = 1 - b_hr
    */

    *b_shr = 1 - b_hr;
    *a_exp = 2*(b_exp + *b_shr) + 30;
}


void xs3_vect_complex_s32_sum_prepare(
    exponent_t* a_exp,
    right_shift_t* b_shr,
    const exponent_t b_exp,
    const headroom_t b_hr,
    const unsigned length)
{
    // Adding together 2^N elements means the accumulator needs N bits of headroom to avoid saturation.
    // Accumulator is 40 bits, values are 32 bits, so the available accumulator headroom is 8+b_hr bits.

    const headroom_t acc_hr = 8 + b_hr;

    const unsigned cl2 = ceil_log2(length);

    *b_shr = MAX(0, (int) (cl2 - acc_hr) );
    *a_exp = b_exp + *b_shr;
}
