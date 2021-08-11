#include "str.h"
#include "math_lib.h"

static
bool_t
actohex(char c, uint8_t* out)
{
	uint16_t r = 0;

	r |= (c >= '0' && c <= '9' ? 0xff00 | (c - '0') : 0);
	r |= (c >= 'A' && c <= 'F' ? 0xff00 | (10 + c - 'A') : 0);
	r |= (c >= 'a' && c <= 'f' ? 0xff00 | (10 + c - 'a') : 0);

	if (out)
	{
		*out = (uint8_t)r;
	}

	return (r & 0xff00 ? true : false);
}

static char
actolower(char c)
{
	/* ansi char to lowercase */
	return ((c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c);
}

static wchar_t
uctolower(wchar_t c)
{
	/* unicode char to lowercase */
	return ((c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c);
}

int str_acmpi(char* a, char* b)
{
	while (actolower(*a) == actolower(*b++))
	{
		if (*a++ == 0)
		{
			return 0;
		}
	}

	return (*a - *(b - 1));
}

int str_ancmp(const char *s1, const char *s2, size_t n)
{
	for (; n > 0; s1++, s2++, --n)
		if (*s1 != *s2)
			return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
		else if (*s1 == '\0')
			return 0;
	return 0;
}

int 
str_ancmpi(const char* s1, const char* s2, size_t n)
{
	size_t i = 0;
	int c1 = 0;
	int c2 = 0;
	if (s1 != NULL)
	{
		c1 = actolower(*s1);
	}
	if (s2 != NULL)
	{
		c2 = actolower(*s2);
	}
	while (c1 && c2 && c1 == c2 && ++i < n)
	{
		s1++, s2++;
		c1 = actolower(*s1);
		c2 = actolower(*s2);
	}
	return c1 - c2;
}

size_t
str_alen(char* s)
{
	size_t len = 0;

	for (/* */; *s++; len++);

	return len;
}

char*
str_astri(char* a, char* b)
{
	size_t	pos = 0;
	char*	m = 0;

	for (/* */; *a; a++)
	{
		if (actolower(*a) == actolower(*(b + pos)))
		{
			pos++;

			/* first char matches */
			m = (!m ? a : m);

			if (*(b + pos) == 0)
			{
				/* found matching sub-string*/
				return m;
			}
		}
		else
		{
			/* char mismatch, reset */
			pos = 0;
			m = 0;
		}
	}

	return 0;
}

wchar_t*
str_ustri(wchar_t* a, wchar_t* b)
{
	size_t		pos = 0;
	wchar_t*	m = 0;

	for (/* */; *a; a++)
	{
		if (uctolower(*a) == uctolower(*(b + pos)))
		{
			pos++;

			/* first char matches */
			m = (!m ? a : m);

			if (*(b + pos) == 0)
			{
				/* found matching sub-string*/
				return m;
			}
		}
		else
		{
			/* char mismatch, reset */
			pos = 0;
			m = 0;
		}
	}

	return 0;
}

int
str_ucmp(wchar_t* a, wchar_t* b)
{
	while (*a == *b++)
	{
		if (*a++ == 0)
		{
			/* reached end of string a */
			return 0;
		}
	}

	return (*a - *(b - 1));
}

int str_acmp(const char* a, const char* b)
{
    while (*a == *b++)
    {
        if (*a++ == 0)
        {
            /* reached end of string a */
            return 0;
        }
    }

    return (*a - *(b - 1));
}

const char* str_astr(const char* a, const char* b)
{
	size_t	pos = 0;
	const char*	m = 0;

	for (/* */; *a; a++)
	{
		if (*a == *(b + pos))
		{
			pos++;

			/* first char matches */
			m = (!m ? a : m);

			if (*(b + pos) == 0)
			{
				/* found matching sub-string*/
				return m;
			}
		}
		else
		{
			/* char mismatch, reset */
			pos = 0;
			m = 0;
		}
	}

	return 0;
}

wchar_t* str_ustr(wchar_t* a, wchar_t* b)
{
	size_t		pos = 0;
	wchar_t*	m = 0;

	for (/* */; *a; a++)
	{
		if (*a == *(b + pos))
		{
			pos++;

			/* first char matches */
			m = (!m ? a : m);

			if (*(b + pos) == 0)
			{
				/* found matching sub-string*/
				return m;
			}
		}
		else
		{
			/* char mismatch, reset */
			pos = 0;
			m = 0;
		}
	}

	return 0;
}

char*
str_nutoa(char* a, wchar_t* b, size_t n)
{
	char* tmp = a;

	while (*b && --n)
	{
		if (*b & 0xff00)
		{
			/* conversion is not possible (rus charset etc.) */
			return 0;
		}

		*a++ = (char)(*b++ & 0xff);
	}

	/* null terminate */
	*a = 0;

	return tmp;
}

char*
str_utoa(char* a, wchar_t* b)
{
	return str_nutoa(a, b, str_ulen(b) + 1);
}

size_t
str_ulen(wchar_t* s)
{
	size_t len = 0;

	for (/* */; *s++; len++);

	return len;
}

char*
str_achr(char* s, char c)
{
	for (/* */; *s && *s != c; s++);

	return (*s ? s : 0);
}

char*
str_acat(char* a, char* b)
{
	char* tmp = a;

	/* go to the end of the string */
	for (/* */; *a; ++a);

	/* concat the string pointed by b */
	while ((*a++ = *b++) != 0);

	return tmp;
}

char*
str_acpy(char* a, char* b)
{
	char* tmp = a;

	for (/* */; (*a = *b) != 0; a++, b++);

	return tmp;
}

wchar_t*
str_ucpy(wchar_t* a, wchar_t* b)
{
	wchar_t* tmp = a;

	for (/* */; (*a = *b) != 0; a++, b++);

	return tmp;
}

char*
str_acpyn(char* a, char* b, size_t n)
{
	char* tmp = a;

	while (n-- && *b)
	{
		*(a++) = *(b++);
	}

	/* null terminate */
	*a = 0;

	return tmp;
}

uint64_t str_atobin(const char* str, ulong_t len)
{
	uint64_t out = 0;

	// convert at max a qword
	if (len > 16 || !str)
	{
		return 0;
	}

	for (ulong_t i = 0; i < len; i++)
	{
		out <<= 4;

		if (str[i] >= '0' && str[i] <= '9')
		{
			out |= (str[i] - '0');
		}
		else if (str[i] >= 'A' && str[i] <= 'F')
		{
			out |= (10 + (str[i] - 'A'));
		}
		else if (str[i] >= 'a' && str[i] <= 'f')
		{
			out |= (10 + (str[i] - 'a'));
		}
		else if (str[i] == '?')
		{
			out |= 0;
		}
		else
		{
			return 0;
		}
	}

	return out;
}

static void
str_reverse(char* s)
{
	char tmp;

	int i;
	int j;

	for (i = 0, j = str_alen(s) - 1; i < j; i++, j--)
	{
		tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
	}
}

static void
str_reverse_n(char* str, size_t len)
{
	size_t start = 0;
	size_t end = len - 1;

	while (start < end)
	{
		char temp = *(str + start);
		*(str + start) = *(str + end);
		*(str + end) = temp;
		start++;
		end--;
	}
}

int64_t
str_toi64(char* s, char** end, int32_t base)
{
	char		c;
	char*		tmp = s;
	bool_t		neg = false;

	int32_t		any = 0;
	uint64_t	acc = 0;
	int32_t		cutlim;
	uint64_t	cutoff;

	/* find the first char */
	for (/* */; (c = *s) == ' '; s++);

	/* check the sign */
	if (c == '-')
	{
		neg = true;
		c = *s++;
	}
	else
	{
		/* assume positive */
		if (c == '+')
		{
			c = *s++;
		}
	}

	/* check base */
	if ((base == 0 || base == 16) && (c == '0') &&
		(*s == 'x' || *s == 'X') && actohex(*(s + 1), 0))
	{
		/* set base to 16 */
		c = *(s + 1);
		s = s + 2;
		base = 16;
	}

	if (base == 0)
	{
		/* find default base, octal or decimal */
		base = (c == '0' ? 8 : 10);
	}

	if (base < 2 || base > 36)
	{
		/* invalid bases */
		return 0;
	}

	cutoff = (neg ? -(uint64_t)LLONG_MIN : LLONG_MAX);
	cutlim = cutoff % (uint64_t)base;
	cutoff = cutoff / (uint64_t)base;

	while (true)
	{
		c = *s++;

		if (!actohex(c, (uint8_t *)&c) || c >= base)
		{
			break;
		}

		if ((any < 0) ||
			(acc > cutoff) ||
			(acc == cutoff && c > cutlim))
		{
			/* overflow */
			any = -1;
		}
		else
		{
			/* digits consumed */
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if (any < 0)
	{
		/* overflow happened: out of range */
		acc = (neg ? LLONG_MIN : LLONG_MAX);
	}
	else if (neg)
	{
		/* value is negative */
		acc = -acc;
	}

	if (end)
	{
		/* set pointer at the end of the number */
		*end = (any ? s - 1 : tmp);
	}

	return acc;
}

uint64_t
str_tou64(char* s, char** end, int32_t base)
{
	char		c;
	char*		tmp = s;
	bool_t		neg = false;

	int32_t		any = 0;
	uint64_t	acc = 0;
	int32_t		cutlim;
	uint64_t	cutoff;

	/* find the first char */
	for (/* */; (c = *s) == ' '; s++);

	/* check the sign */
	if (c == '-')
	{
		neg = true;
		c = *s++;
	}
	else
	{
		/* assume positive */
		if (c == '+')
		{
			c = *s++;
		}
	}

	/* check base */
	if ((base == 0 || base == 16) && (c == '0') &&
		(*s == 'x' || *s == 'X') && actohex(*(s + 1), 0))
	{
		/* set base to 16 */
		c = *(s + 1);
		s = s + 2;
		base = 16;
	}

	if (base == 0)
	{
		/* find default base, octal or decimal */
		base = (c == '0' ? 8 : 10);
	}

	if (base < 2 || base > 36)
	{
		/* invalid bases */
		return 0;
	}

	cutlim = ULLONG_MAX % base;
	cutoff = ULLONG_MAX / base;

	while (true)
	{
		c = *s++;

		if (!actohex(c, (uint8_t *)&c) || c >= base)
		{
			break;
		}

		if ((any < 0) ||
			(acc > cutoff) ||
			(acc == cutoff && c > cutlim))
		{
			/* overflow */
			any = -1;
		}
		else
		{
			/* digits consumed */
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if (any < 0)
	{
		/* overflow happened: out of range */
		acc = ULLONG_MAX;
	}
	else if (neg)
	{
		/* value is negative */
		acc = -acc;
	}

	if (end)
	{
		/* set pointer at the end of the number */
		*end = (any ? s - 1 : tmp);
	}

	return acc;
}

double
str_todouble(char* s, char** end)
{
	char* dec_end;
	char* frac_end;
	double dec_part;
	double frac_part = 0.0;

	/* get the decimal part */
	dec_part = (double)str_toi64(s, &dec_end, 10);

	/* get the fractional part */
	if (*dec_end++ == '.')
	{
		double temp = (double)str_toi64(dec_end, &frac_end, 10);
		uint32_t len = frac_end - dec_end;

		if (len)
		{
			frac_part = temp;
			frac_part /= math_pow(10.0, len);
		}
	}
	else
	{
		frac_end = dec_end;
	}

	if (end)
	{
		*end = frac_end;
	}

	return (dec_part >= 0.0f) ? (dec_part + frac_part) : (dec_part - frac_part);
}

void
str_i64toa(char* str, int64_t val, int32_t base)
{
	bool_t neg = 0;
	size_t pos = 0;

	/* return if value is zero or base is invalid */
	if (val == 0 || base < 2 || base > 36)
	{
		str[0] = '0';
		str[1] = 0;
		return;
	}

	/* check if the int is negative */
	if (val < 0 && base == 10)
	{
		neg = true;
		val = -val;
	}

	/* convert the number */
	while (val)
	{
		int64_t rem = val % base;
		str[pos++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		val /= base;
	}

	/* append null-terminator and sign if negative */
	if (neg)
	{
		str[pos++] = '-';
	}

	str[pos] = 0;

	/* reverse the string */
	str_reverse_n(str, pos);
}

void
str_u64toa(char* str, uint64_t val, int32_t base)
{
	size_t pos = 0;

	/* return if value is zero or base is invalid */
	if (val == 0 || base < 2 || base > 36)
	{
		str[0] = '0';
		str[1] = 0;
		return;
	}

	/* convert the number */
	while (val)
	{
		int64_t rem = val % base;
		str[pos++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		val /= base;
	}

	/* append null-terminator and sign if negative */
	str[pos] = 0;

	/* reverse the string */
	str_reverse_n(str, pos - 1);
}

char* str_doubletoa(char *s, double n, double precision)
{
	// handle special cases
	if (math_is_nan(n)) 
	{
		str_acpy(s, "nan");
	}
	else if (math_is_inf(n)) 
	{
		str_acpy(s, "inf");
	}
	else if (n == 0.0) 
	{
		str_acpy(s, "0");
	}
	else 
	{
		int32_t digit, m, m1;
		char *c = s;
		int32_t neg = (n < 0);

		if (neg)
		{
			n = -n;
			n -= 0.5 * precision;
		}
		else
		{
			n += 0.5 * precision;
		}

		m = (int32_t)math_log10(n);

		int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);

		if (neg)
		{
			*(c++) = '-';
		}

		// set up for scientific notation
		if (useExp) 
		{
			if (m < 0)
			{
				m -= 1.0;
			}

			n = n / math_pow(10.0, m);
			m1 = m;
			m = 0;
		}

		if (m < 1.0) 
		{
			m = 0;
		}

		while (n > precision || m >= 0) 
		{
			double weight = math_pow(10.0, m);

			if (weight > 0 && !math_is_inf(weight)) 
			{
				digit = math_floor(n / weight);
				n -= (digit * weight);
				*(c++) = '0' + digit;
			}

			if (m == 0 && n > 0)
			{
				*(c++) = '.';
			}

			m--;
		}

		if (useExp) 
		{
			// convert the exponent
			int32_t i, j;

			*(c++) = 'e';

			if (m1 > 0) 
			{
				*(c++) = '+';
			}
			else 
			{
				*(c++) = '-';
				m1 = -m1;
			}

			m = 0;

			while (m1 > 0) 
			{
				*(c++) = '0' + m1 % 10;
				m1 /= 10;
				m++;
			}

			c -= m;

			for (i = 0, j = m - 1; i < j; i++, j--) 
			{
				// swap without temporary
				c[i] ^= c[j];
				c[j] ^= c[i];
				c[i] ^= c[j];
			}

			c += m;
		}

		*(c) = '\0';
	}

	return s;
}

