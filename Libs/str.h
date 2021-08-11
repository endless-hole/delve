#ifndef _STRING_H_
#define _STRING_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* returns the length of an ansi string */
size_t str_alen(char* str);

/* returns the length of an unicode string */
size_t str_ulen(wchar_t* str);

/* compares two ansi strings */
int str_acmp(const char* a, const char* b);

/* compares two unicode strings */
int str_ucmp(wchar_t* a, wchar_t* b);

/* compares two ansi strings, case insensitive */
int str_acmpi(char* a, char* b);

/* compares two ansi strings, case insensitive */
int str_ancmpi(const char* a, const char* b, size_t n);

int str_ancmp(const char *s1, const char *s2, size_t n);

/* compares two unicode strings, case insensitive */
int str_ucmpi(wchar_t* a, wchar_t* b);

/* returns the first occurrence of b in a, ansi */
const char* str_astr(const char* a, const char* b);

/* returns the first occurrence of b in a, unicode */
wchar_t* str_ustr(wchar_t* a, wchar_t* b);

/* like str_astr but case insensitive */
char* str_astri(char* a, char* b);

/* like str_ustr but case insensitive */
wchar_t* str_ustri(wchar_t* a, wchar_t* b);

/* copies the string from b to a, ansi */
char* str_acpy(char* a, char* b);

/* copies the string from b to a, unicode */
wchar_t* str_ucpy(wchar_t* a, wchar_t* b);

/* concatenates string b to a, ansi */
char* str_acat(char* a, char* b);

/* concatenates string b to a, ansi */
char* str_acatn(char* a, char* b, size_t n);

/* concatenates string b to a, unicode */
wchar_t* str_ucat(wchar_t* a, wchar_t* b);

/* finds the first occurence of c in s, ansi */
char* str_achr(char* s, char c);

/* finds the first occurence of c in s, ansi */
wchar_t* str_uchr(wchar_t* s, wchar_t c);

/* copies n-1 chars from b to a, ansi */
char* str_acpyn(char* a, char* b, size_t n);

/* copies n-1 chars from b to a, unicode */
wchar_t* str_ucpyn(wchar_t* a, wchar_t* b, size_t n);

/* 
	if max=0: duplicates the string s, ansi 
	else: duplicate maximum max-1 chars
*/
char* str_adup(char* s, size_t max);

/* 
	if max=0: duplicates the whole string s, unicode 
	else: duplicate maximum max-1 chars
*/
wchar_t* str_udup(wchar_t* s, size_t max);

/* 
	converts n-1 chars of an unicode string to ansi, 
	returns zero if it's not possible 
*/
char* str_nutoa(char* a, wchar_t* b, size_t n);

/* converts n-1 chars of an ansi string to unicode */
wchar_t* str_natou(wchar_t* a, char* b, size_t n);

/* 
	converts unicode string to ansi, 
	returns zero if it's not possible 
*/
char* str_utoa(char* a, wchar_t* b);

/* reverses the string */
void str_reverse(char* s);

/* converts ansi string to unicode */
wchar_t* str_atou(wchar_t* a, char* b);

/* convert to int32 */
int32_t str_toi32(char* s, char** end, int32_t base);

/* convert to int64 */
int64_t str_toi64(char* s, char** end, int32_t base);

/* convert to uint32 */
uint32_t str_tou32(char* s, char** end, int32_t base);

/* convert to uint64 */
uint64_t str_tou64(char* s, char** end, int32_t base);

/* convert to float */
double str_todouble(char* s, char** end);

/* convert int32 to string */
void str_i32toa(char* str, int32_t val, int32_t base);

/* convert int64 to string */
void str_i64toa(char* str, int64_t val, int32_t base);

/* convert uint32 to string */
void str_u32toa(char* str, uint32_t val, int32_t base);

/* convert uint64 to string */
void str_u64toa(char* str, uint64_t val, int32_t base);

/* convert float to string */
char* str_doubletoa(char *s, double n, double precision);

/* string to bin */
uint64_t str_atobin(const char* str, ulong_t len);

#ifdef __cplusplus
}
#endif

#endif //_STRING_H_