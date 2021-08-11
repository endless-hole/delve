#include "math_lib.h"

#define asint(_x)		*(int *)(&_x);
#define asfloat(_x)		*(float *)(&_x);


#define F64_EXP_BIAS	1023
#define F64_EXP_MAX		2047
#define DBL_MAX_EXP		1024
#define DBL_MIN_EXP		-1021
#define	M_LN_MAX_D		(M_LN2 * DBL_MAX_EXP)
#define	M_LN_MIN_D		(M_LN2 * (DBL_MIN_EXP - 1))

#define HUGE_VAL (huge_val.d)

#define	POLYNOM1(x, a)	((a)[1]*(x)+(a)[0])
#define	POLYNOM2(x, a)	(POLYNOM1((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM3(x, a)	(POLYNOM2((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM4(x, a)	(POLYNOM3((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM5(x, a)	(POLYNOM4((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM6(x, a)	(POLYNOM5((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM7(x, a)	(POLYNOM6((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM8(x, a)	(POLYNOM7((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM9(x, a)	(POLYNOM8((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM10(x, a)	(POLYNOM9((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM11(x, a)	(POLYNOM10((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM12(x, a)	(POLYNOM11((x),(a)+1)*(x)+(a)[0])
#define	POLYNOM13(x, a)	(POLYNOM12((x),(a)+1)*(x)+(a)[0])

float math_absf(float x)
{
	return x < 0.f ? -x : x;
}

int32_t math_absi32(int32_t x)
{
	int32_t s = x >> 31;
	return (x ^ s) - s;
}

int64_t math_absi64(int64_t x)
{
	int32_t s = x >> 63;
	return (x ^ s) - s;
}

float rcpSqrtIEEEIntApproximation(float inX, const int inRcpSqrtConst)
{
	int x = asint(inX);
	x = inRcpSqrtConst - (x >> 1);
	return asfloat(x);
}

float rcpSqrtNewtonRaphson(float inXHalf, float inRcpX)
{
	return inRcpX * (-inXHalf * (inRcpX * inRcpX) + 1.5f);
}

float fastRcpSqrtNR0(float inX)
{
	float  xRcpSqrt = rcpSqrtIEEEIntApproximation(inX, 
		IEEE_INT_RCP_SQRT_CONST_NR0);
	return xRcpSqrt;
}

float fastRcpSqrtNR1(float inX)
{
	float  xhalf = 0.5f * inX;
	float  xRcpSqrt = rcpSqrtIEEEIntApproximation(inX, 
		IEEE_INT_RCP_SQRT_CONST_NR1);
	xRcpSqrt = rcpSqrtNewtonRaphson(xhalf, xRcpSqrt);
	return xRcpSqrt;
}

float fastRcpSqrtNR2(float inX)
{
	float  xhalf = 0.5f * inX;
	float  xRcpSqrt = rcpSqrtIEEEIntApproximation(inX, IEEE_INT_RCP_SQRT_CONST_NR2);
	xRcpSqrt = rcpSqrtNewtonRaphson(xhalf, xRcpSqrt);
	xRcpSqrt = rcpSqrtNewtonRaphson(xhalf, xRcpSqrt);
	return xRcpSqrt;
}

float sqrtIEEEIntApproximation(float inX, const int inSqrtConst)
{
	int x = asint(inX);
	x = inSqrtConst + (x >> 1);
	return asfloat(x);
}

float fastSqrtNR0(float inX)
{
	float  xRcp = sqrtIEEEIntApproximation(inX, IEEE_INT_SQRT_CONST_NR0);
	return xRcp;
}

float fastSqrtNR1(float inX)
{
	// Inverse Rcp Sqrt
	return inX * fastRcpSqrtNR1(inX);
}

float fastSqrtNR2(float inX)
{
	// Inverse Rcp Sqrt
	return inX * fastRcpSqrtNR2(inX);
}

float rcpIEEEIntApproximation(float inX, const int inRcpConst)
{
	int x = asint(inX);
	x = inRcpConst - x;
	return asfloat(x);
}

float rcpNewtonRaphson(float inX, float inRcpX)
{
	return inRcpX * (-inRcpX * inX + 2.0f);
}

float fastRcpNR0(float inX)
{
	float  xRcp = rcpIEEEIntApproximation(inX, IEEE_INT_RCP_CONST_NR0);
	return xRcp;
}

float fastRcpNR1(float inX)
{
	float  xRcp = rcpIEEEIntApproximation(inX, IEEE_INT_RCP_CONST_NR1);
	xRcp = rcpNewtonRaphson(inX, xRcp);
	return xRcp;
}

float fastRcpNR2(float inX)
{
	float  xRcp = rcpIEEEIntApproximation(inX, IEEE_INT_RCP_CONST_NR2);
	xRcp = rcpNewtonRaphson(inX, xRcp);
	xRcp = rcpNewtonRaphson(inX, xRcp);
	return xRcp;
}

float atanFast4(float inX)
{
	float x = inX;
	return x * (-0.1784f * math_absf(x) - 0.0663f * x * x + 1.0301f);
}

float atanFast2(float x, float y)
{
	const float ONE_QTR_PI = M_PI / 4.0;
	const float THR_QTR_PI = 3.0 * M_PI / 4.0; 

	float r;
	float angle;
	float abs_y = math_absf(y) + 1e-10f;

	if (x < 0.0f)
	{
		r = (x + abs_y) / (abs_y - x);
		angle = THR_QTR_PI;
	}
	else
	{
		r = (x - abs_y) / (x + abs_y);
		angle = ONE_QTR_PI;
	}

	angle += (0.1963f * r * r - 0.9817f) * r;

	if (y < 0.0f)
	{
		return(-angle); // negate if in quad III or IV
	}
	else
	{
		return(angle);
	}
}

bool_t math_is_nan(double n)
{
	/* check if not a number (undefined or unrepresentable value) */
	float64_t* f = (float64_t *)&n;

	if (F64_GET_EXP(f) != F64_EXP_MAX)
	{
		return false;
	}

	return (F64_GET_MANT_LOW(f) != 0 || F64_GET_MANT_HIGH(f) != 0);
}

bool_t math_is_inf(double n)
{
	/* check if infinity */
	float64_t* f = (float64_t *)&n;

	if (F64_GET_EXP(f) != F64_EXP_MAX)
	{
		return false;
	}

	return (F64_GET_MANT_LOW(f) == 0 || F64_GET_MANT_HIGH(f) == 0);
}

double
math_modf(double n, double* int_part)
{
	double tmp = n;

	/* break n into an integral and a fractional part */
	float64_t* f = (float64_t *)&n;

	int e;
	int mask_bits;

	/* mantissa (significand) */
	ulong_t mant;

	e = F64_GET_EXP(f) - F64_EXP_BIAS;

	/* purely fractional, no integral part */
	if (e < 0)
	{
		*int_part = 0;
		return n;
	}

	mask_bits = 52 - e;

	/* purely integral, no fractional part */
	if (mask_bits <= 0)
	{
		*int_part = n;
		return 0;
	}

	/* extract the integral part */
	if (mask_bits >= 32)
	{
		mask_bits -= 32;
		mant = F64_GET_MANT_HIGH(f);
		mant &= ~((1 << mask_bits) - 1);

		F64_SET_MANT_LOW(f, 0);
		F64_SET_MANT_HIGH(f, mant);
	}
	else
	{
		mant = F64_GET_MANT_LOW(f);
		mant &= ~((1 << mask_bits) - 1);

		F64_SET_MANT_LOW(f, mant);
	}

	*int_part = n;

	return tmp - n;
}

double
math_floor(double n)
{
	double val;

	return (math_modf(n, &val) < 0 ? val - 1.0 : val);
}

double math_log(double n)
{
	int	e;

	double znum;
	double zden;
	double z;
	double w;

	/*
		Algorithm and coefficients from:
		"Software manual for the elementary functions"
		by W.J. Cody and W. Waite, Prentice-Hall, 1980
	*/
	static double a[] = 
	{
		-0.64124943423745581147e2,
		 0.16383943563021534222e2,
		-0.78956112887491257267e0
	};

	static double b[] = 
	{
		-0.76949932108494879777e3,
		 0.31203222091924532844e3,
		-0.35667977739034646171e2,
		 1.0
	};

	/* check if it's NaN */
	if(math_is_nan(n))
	{
		return n;
	}

	if(n < 0)
	{
		return -HUGE_VAL;
	}
	else 
	if(n == 0) 
	{
		return -HUGE_VAL;
	}

	if(n > DBL_MAX)
	{
		return n;
	}

	/* get significand and exponent */
	n = math_frexp(n, &e);

	if(n > M_1_SQRT2) 
	{
		znum = (n - 0.5) - 0.5;
		zden = n * 0.5 + 0.5;
	}
	else 
	{
		znum = n - 0.5;
		zden = znum * 0.5 + 0.5;
		e--;
	}

	z = znum / zden; 
	w = z * z;
	n = z + z * w * (POLYNOM2(w,a) / POLYNOM3(w,b));
	z = e;
	n += z * (-2.121944400546905827679e-4);

	return n + z * 0.693359375;
}

double math_log10(double n)
{
	/* check if it's NaN */
	if (math_is_nan(n))
	{
		return n;
	}

	if (n < 0)
	{
		return -HUGE_VAL;
	}
	else if (n == 0)
	{
		return -HUGE_VAL;
	}

	return math_log(n) / M_LN10;
}

double math_frexp(double n, int* eptr)
{
	float64_t* f = (float64_t *)&n;

	int e;
	int e_bias;

	double factor;

	e = F64_GET_EXP(f);
	e_bias = 0;

	/* either infinity or not a number (NaN) */
	if (e == F64_EXP_MAX)
	{
		*eptr = 0;
		return n;
	}

	if (e == 0)
	{
		/* either 0 or denormal */
		if (F64_GET_MANT_LOW(f) == 0 &&
			F64_GET_MANT_HIGH(f) == 0)
		{
			*eptr = 0;
			return n;
		}

		/* multiply by 2^64 */
		factor = 65536.0;	/* 2^16 */
		factor *= factor;	/* 2^32 */
		factor *= factor;	/* 2^64 */
		n *= factor;

		e = F64_GET_EXP(f);
		e_bias = 64;
	}

	F64_SET_EXP(f, F64_EXP_BIAS - 1);

	*eptr = e - F64_EXP_BIAS - e_bias + 1;

	return n;
}

double
math_sin_cos(double n, bool_t cos_flag)
{
	double y;

	int sign = 1;

	/*
		Algorithm and coefficients from:
		"Software manual for the elementary functions"
		by W.J. Cody and W. Waite, Prentice-Hall, 1980
	*/
	static double r[] = 
	{
		-0.16666666666666665052e+0,
		 0.83333333333331650314e-2,
		-0.19841269841201840457e-3,
		 0.27557319210152756119e-5,
		-0.25052106798274584544e-7,
		 0.16058936490371589114e-9,
		-0.76429178068910467734e-12,
		 0.27204790957888846175e-14
	};

	/* check if it's NaN */
	if(math_is_nan(n))
	{
		return n;
	}

	/* if negative, make it positive */
	if(n < 0) 
	{
		n = -n;
		sign = -1;
	}

	if(cos_flag) 
	{
		y = M_PI_2 + n;
		sign = 1;
	}
	else    
	{
		y = n;
	}

	/* avoid loss of significance, if y is too large, error */
	y = y * M_1_PI + 0.5;

	if(y >= DBL_MAX / M_PI) 
	{
		return 0.0;
	}

	/*
		use extended precision to calculate reduced argument;
		here we used 12 bits of the mantissa for a1;
		also split n in integer part n1 and fraction part n2
	*/
	#define A1 3.1416015625
	#define A2 -8.908910206761537356617e-6
	{
		double n1, n2;

		math_modf(y, &y);

		if(math_modf(0.5 * y, &n1)) 
		{
			sign = -sign;
		}

		if(cos_flag)
		{
			y -= 0.5;
		}

		n2 = math_modf(n, &n1);

		n = n1 - y * A1;
		n += n2;
		n -= y * A2;

		#undef A1
		#undef A2
	}
 
	if(n < 0) 
	{
		n = -n;
		sign = -sign;
	}

	/* avoid underflow */
	y = n * n;
	n += n * y * POLYNOM7(y, r);

	return (sign == -1 ? -n : n);
}

double
math_sin(double n)
{
	return math_sin_cos(n, false);
}

double
math_cos(double n)
{
	return math_sin_cos((n < 0 ? -n : n), true);
}

double
math_ldexp(double n, int e)
{
	float64_t* f = (float64_t *)&n;

	int e_old;

	double factor;

	/* save the old exponent */
	e_old = F64_GET_EXP(f);

	/* either infinity or not a number (NaN) */
	if (e_old == F64_EXP_MAX)
	{
		return n;
	}

	/* either 0 or denormal */
	if (e_old == 0)
	{
		if (F64_GET_MANT_LOW(f) == 0 &&
			F64_GET_MANT_HIGH(f) == 0)
		{
			return n;
		}
	}

	/* if exponent is too large return HUGE_VAL or if too small return 0 */
	if (e >= 2 * F64_EXP_MAX)
	{
		return HUGE_VAL;
	}

	if (e <= -2 * F64_EXP_MAX)
	{
		return 0;
	}

	/* normalize a denormal */
	if (e_old == 0)
	{
		/* multiply by 2^64 */
		factor = 65536.0;	/* 2^16 */
		factor *= factor;	/* 2^32 */
		factor *= factor;	/* 2^64 */
		n *= factor;

		e = -64;
		e_old = F64_GET_EXP(f);
	}

	e = e_old + e;

	/* overflow */
	if (e >= F64_EXP_MAX)
	{
		return HUGE_VAL;
	}

	/* normal */
	if (e > 0)
	{
		F64_SET_EXP(f, e);
		return n;
	}

	/* denormal or underflow */
	e += 64;

	F64_SET_EXP(f, e);

	/* divide by 2^64 */
	factor = 65536.0;	/* 2^16 */
	factor *= factor;	/* 2^32 */
	factor *= factor;	/* 2^64 */
	n /= factor;

	return n;
}

double
math_exp(double n)
{
	/*
		Algorithm and coefficients from:
		"Software manual for the elementary functions"
		by W.J. Cody and W. Waite, Prentice-Hall, 1980
	*/
	static double p[] = 
	{
		0.25000000000000000000e+0,
		0.75753180159422776666e-2,
		0.31555192765684646356e-4
	};

	static double q[] = 
	{
		0.50000000000000000000e+0,
		0.56817302698551221787e-1,
		0.63121894374398503557e-3,
		0.75104028399870046114e-6
	};

	double a;
	double g;

	int	x;
	int	negative = n < 0;

	/* check if it's NaN */
	if(math_is_nan(n))
	{
		return n;
	}

	if(n < M_LN_MIN_D)
	{
		return 0.0;
	}

	if(n > M_LN_MAX_D)
	{
		return HUGE_VAL;
	}

	/* make n positive */
	n = (negative ? -n : n);
 
	/* 1/ln(2) = log2(e), 0.5 added for rounding */
	x = (int)(n * M_LOG2E + 0.5);
	a = x;
	{
		double x1 = (long)n;
		double x2 = n - x1;

		g = ((x1 - a * 0.693359375) + x2) - a * (-2.1219444005469058277e-4);
	}

	g = (negative ? -g : g);
	x = (negative ? -x : x);

	a = g * g;
	n = g * POLYNOM2(a, p);
	x += 1;

	return (math_ldexp(0.5 + n / (POLYNOM3(a, q) - n), x));
}

double math_pow(double x, double y)
{
	int	negative = 0;

	double dummy;

	/* base and exponent zero, etc */
	if ((x == 0 && y == 0) ||
		(x < 0 && math_modf(y, &dummy) != 0))
	{
		return 0;
	}

	if (x == 0)
	{
		return x;
	}

	if (x < 0)
	{
		if (math_modf(y / 2.0, &dummy) != 0)
		{
			/* y was odd */
			negative = 1;
		}

		x = -x;
	}

	x = math_log(x);

	if (x < 0)
	{
		x = -x;
		y = -y;
	}

	/* beware of overflow in the multiplication */
	if (x > 1.0 && y > DBL_MAX / x)
	{
		return (negative ? -HUGE_VAL : HUGE_VAL);
	}

	x = math_exp(x * y);

	return (negative ? -x : x);
}
