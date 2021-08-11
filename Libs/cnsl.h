/****************************************************************************
	hacktheplanetside - cnsl.h
	(c) lowhertz
****************************************************************************/

#ifndef _CNSL_H_
#define _CNSL_H_

#include <stdarg.h>
#include "types.h"
#include "ntdll\ntdll.h"

#ifdef __cplusplus
	extern "C" {
#endif

/****************************************************************************

	exported functions

****************************************************************************/
void cnsl_alloc(void);

ulong_t cnsl_read(void* buffer, ulong_t size);

ulong_t cnsl_write(void* buffer, ulong_t size);

bool_t cnsl_get_char(char* c);

void cnsl_scroll_scrn_buffer(ushort_t delta, PCONSOLE_SCREEN_BUFFER_INFO csbi);

void cnsl_clrscrn(void);

void cnsl_get_color(ushort_t* color);

void cnsl_set_color(ushort_t color);

void cnsl_newline(void);

bool_t cnsl_print(ushort_t color, char* fmt, ...);

#ifndef SUPPRESS_DBG_MSG
#define dbg_msg(fmt, ...) \
    do { cnsl_print(FOREGROUND_GREEN | FOREGROUND_INTENSITY, \
		 fmt "\r\n", ## __VA_ARGS__); } while(0)
#else
#define dbg_msg(fmt, ...) __noop
#endif

#ifdef __cplusplus
	}
#endif

#endif //_CNSL_H_