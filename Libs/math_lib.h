/******************************************************************************
    Shader Fast Math Lib (v0.41)

    A shader math library for optimized approximate transcendental functions.
    Optimized and tested on AMD GCN architecture.

    Release notes:
    v0.41   minor bug fixes, missing references
    
    v0.4    new constants calculated for new ranges, minor optimization and precision improvements
            Developed during production of : Far Cry 4, Ubisoft Montreal

    v0.3    added Newton Raphson 1 and 2 iterations
            Newton Raphson methods provided for reference purpose (some code / architectures might still benefit from single NR).

    v0.2    fast IEEE float math sqrt() rsqrt() rcp()

    v0.1    4th order polynomial approximations for atan() asin() acos()
            Developed during production of : Killzone : Shadow Fall, Guerrilla Games, SCEE

    Ubisoft and Guerrilla Games granted permission for open source distribution.

    Contact information:
    Michal Drobot - @MichalDrobot
    hello@drobot.org

    Presented publicly part of a course: 
    Low Level Optimizations for AMD GCN
    (available @ http://michaldrobot.com/publications/)
********************************************************************************/

/******************************************************************************
    The MIT License (MIT)

    Copyright (c) <2014> <Michal Drobot>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
********************************************************************************/
#ifndef SHADER_FAST_MATH_INC_FX
#define SHADER_FAST_MATH_INC_FX

#ifdef __cplusplus
extern "C" {
#endif

#include "..\Libs\types.h"

#define	M_PI			3.14159265358979323846264338327950288
#define	M_2PI			6.28318530717958647692528676655900576
#define	M_3PI_4			2.35619449019234492884698253745962716
#define	M_PI_2			1.57079632679489661923132169163975144
#define	M_3PI_8			1.17809724509617246442349126872981358
#define	M_PI_4			0.78539816339744830961566084581987572
#define	M_PI_8			0.39269908169872415480783042290993786
#define	M_1_PI			0.31830988618379067153776752674502872
#define	M_2_PI			0.63661977236758134307553505349005744
#define	M_4_PI			1.27323954473516268615107010698011488
#define	M_E				2.71828182845904523536028747135266250
#define	M_LOG2E			1.44269504088896340735992468100189213
#define	M_LOG10E		0.43429448190325182765112891891660508
#define	M_LN2			0.69314718055994530941723212145817657
#define	M_LN10			2.30258509299404568401799145468436421
#define	M_SQRT2			1.41421356237309504880168872420969808
#define	M_1_SQRT2		0.70710678118654752440084436210484904
#define	M_EULER			0.57721566490153286060651209008240243
#define DBL_MAX			1.7976931348623158e+308 

#define DEG2RAD( x )	((x) * (M_PI/180.0f))
#define RAD2DEG( x )	((x) * (180.0f/M_PI))

// Derived from batch testing
// TODO : Should be improved
#define IEEE_INT_RCP_CONST_NR0              0x7EF311C2  
#define IEEE_INT_RCP_CONST_NR1              0x7EF311C3 
#define IEEE_INT_RCP_CONST_NR2              0x7EF312AC  

// Derived from batch testing
#define IEEE_INT_SQRT_CONST_NR0             0x1FBD1DF5   

// Biases for global ranges
// 0-1 or 1-2 specific ranges might improve from different bias
// Derived from batch testing
// TODO : Should be improved
#define IEEE_INT_RCP_SQRT_CONST_NR0         0x5f3759df
#define IEEE_INT_RCP_SQRT_CONST_NR1         0x5F375A86 
#define IEEE_INT_RCP_SQRT_CONST_NR2         0x5F375A86 

typedef union 
{ 
	uint8_t c[8]; 
	double d;

} huge_val_t;


typedef struct float64
{
	ulong_t low_word;
	ulong_t high_word;

} float64_t;

/* get the 1-bit sign of the 64-bit float */
#define F64_GET_SIGN(fp) \
	(((fp)->high_word >> 31) & 1)

/* get the 11-bit exponent of the 64-bit float */
#define F64_GET_EXP(fp) \
	(((fp)->high_word >> 20) & 0x7ff)

/* set the 11-bit exponent of the 64-bit float */
#define F64_SET_EXP(fp, val) \
	((fp)->high_word = ((fp)->high_word & ~(0x7ff << 20)) | \
	(((val) & 0x7ff) << 20))

/* get the lower bits of the mantissa (or significand) */
#define F64_GET_MANT_LOW(fp) \
	((fp)->low_word)

/* set the lower bits of the mantissa (or significand) */
#define F64_SET_MANT_LOW(fp, val) \
	((fp)->low_word = (val))

/* get the higher bits of the mantissa (or significand) */
#define F64_GET_MANT_HIGH(fp) \
	(((fp)->high_word >> 0) & 0xfffff)

/* set the higher bits of the mantissa (or significand) */
#define F64_SET_MANT_HIGH(fp, val) \
	((fp)->high_word = ((fp)->high_word & ~(0xfffff << 0)) | \
	(((val) & 0xfffff) << 0))

/*
	big endian:		{ 0x7f, 0xf0, 0, 0, 0, 0, 0, 0 }
	little endian:	{ 0, 0, 0, 0, 0, 0, 0xf0, 0x7f }
*/
static huge_val_t huge_val = {0, 0, 0, 0, 0, 0, 0xf0, 0x7f};

bool_t math_is_nan(double n);

bool_t math_is_inf(double n);

double math_modf(double n, double* int_part);

double math_frexp(double n, int* eptr);

double math_ldexp(double n, int e);

double math_fabs(double n);

double math_sin(double n);

double math_cos(double n);

double math_sqrt(double n);

double math_tan(double n);

double math_asin(double n);

double math_acos(double n);

double math_atan(double n);

double math_atan2(double y, double x);

double math_ceil(double n);

double math_floor(double n);

double math_fmod(double x, double y);

double math_exp(double n);

double math_log(double n);

double math_log10(double n);

double math_pow(double x, double y);

//
//	ABS
//
float math_absf(float x);
int32_t math_absi32(int32_t x);
int64_t math_absi64(int64_t x);

//
// RCP SQRT
//

// Approximate guess using integer 
// float arithmetics based on IEEE floating point standard
float rcpSqrtIEEEIntApproximation(float inX, const int inRcpSqrtConst);
float rcpSqrtNewtonRaphson(float inXHalf, float inRcpX);

//
// Using 0 Newton Raphson iterations
// Relative error : ~3.4% over full
// Precise format : ~small float
// 2 ALU
//
float fastRcpSqrtNR0(float inX);

//
// Using 1 Newton Raphson iterations
// Relative error : ~0.2% over full
// Precise format : ~half float
// 6 ALU
//
float fastRcpSqrtNR1(float inX);

//
// Using 2 Newton Raphson iterations
// Relative error : ~4.6e-004%  over full
// Precise format : ~full float
// 9 ALU
//
float fastRcpSqrtNR2(float inX);

//
// SQRT
//
float sqrtIEEEIntApproximation(float inX, const int inSqrtConst);

//
// Using 0 Newton Raphson iterations
// Relative error : < 0.7% over full
// Precise format : ~small float
// 1 ALU
//
float fastSqrtNR0(float inX);

//
// Use inverse Rcp Sqrt
// Using 1 Newton Raphson iterations
// Relative error : ~0.2% over full
// Precise format : ~half float
// 6 ALU
//
float fastSqrtNR1(float inX);

//
// Use inverse Rcp Sqrt
// Using 2 Newton Raphson iterations
// Relative error : ~4.6e-004%  over full
// Precise format : ~full float
// 9 ALU
//
float fastSqrtNR2(float inX);

//
// RCP
//
float rcpIEEEIntApproximation(float inX, const int inRcpConst);
float rcpNewtonRaphson(float inX, float inRcpX);

//
// Using 0 Newton Raphson iterations
// Relative error : < 0.4% over full
// Precise format : ~small float
// 1 ALU
//
float fastRcpNR0(float inX);

//
// Using 1 Newton Raphson iterations
// Relative error : < 0.02% over full
// Precise format : ~half float
// 3 ALU
//
float fastRcpNR1(float inX);

//
// Using 2 Newton Raphson iterations
// Relative error : < 5.0e-005%  over full
// Precise format : ~full float
// 5 ALU
//
float fastRcpNR2(float inX);

// 4th order hyperbolical approximation
// 4 VGRP, 12 ALU Full Rate
// 7 * 10^-5 radians precision 
// Reference : Efficient approximations for the arctangent function, 
// Rajan, S. Sichun Wang Inkol, R. Joyal, A., May 2006
float atanFast4(float inX);
float atanFast2(float x, float y);

#ifdef __cplusplus
 }
#endif

#endif //SHADER_FAST_MATH_INC_FX