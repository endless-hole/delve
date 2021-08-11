/****************************************************************************
	hacktheplanetside - cnsl.c
	(c) lowhertz
****************************************************************************/
#include "mm.h"
#include "str.h"
#include "cnsl.h"
#include "pe.h"
#include "proc.h"
#include "ntdll\ntdll.h"
#include <stdio.h>

#pragma comment(lib, "ntdllp64.lib")

/****************************************************************************

	exported functions

****************************************************************************/
void
cnsl_alloc(void)
{
	AllocConsole();
}

ulong_t 
cnsl_write(void* buffer, ulong_t size)
{
	/* bytes written */
	ulong_t wr = 0;

	/* std output handle */
	handle_t h = NtCurrentPeb()->ProcessParameters->StandardOutput;

	return (WriteFile(h, buffer, size, &wr, 0) ? wr : 0);
}

void
cnsl_get_color(ushort_t* color)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	/* std output handle */
	handle_t h = NtCurrentPeb()->ProcessParameters->StandardOutput;
	
	if(!GetConsoleScreenBufferInfo(h, &csbi))
	{
		return;
	}

	*color = csbi.wAttributes;
}

void
cnsl_set_color(ushort_t color)
{
	/* std output handle */
	handle_t h = NtCurrentPeb()->ProcessParameters->StandardOutput;

	SetConsoleTextAttribute(h, color);
}

bool_t
cnsl_print(ushort_t color, char* fmt, ...)
{
	int len;
	char buffer[2048];
	ushort_t prev_color;
	va_list args;

#if 1 // todo: fix weird issue with lib not importing properly (workaround)
	typedef int(__cdecl* _vsnprintf_signature)
	(
		char *buffer,
		size_t count,
		const char *format,
		va_list argptr
	);

	static _vsnprintf_signature fmt_func_ptr = 0;

	if(!fmt_func_ptr)
	{
		fmt_func_ptr = pe_get_proc_address(
			module_get_base(NtCurrentProcess(), "ntdll.dll", 0, 64), 
				"_vsnprintf", 0, false);
	}

	va_start(args, fmt);
	
	/* write formatted output using a pointer to a list of arguments */
	if((len = fmt_func_ptr(buffer, sizeof(buffer), fmt, args)) < 0)
	{
		return false;
	}

	va_end(args);
#else
	va_start(args, fmt);

	if ((len = _vsnprintf(buffer, sizeof(buffer), fmt, args)) < 0)
	{
		return false;
	}

	va_end(args);
#endif

	cnsl_get_color(&prev_color);

	if(color != 0)
	{
		cnsl_set_color(color);
	}
	
	cnsl_write(buffer, len);
	cnsl_set_color(prev_color);

	return true;
}