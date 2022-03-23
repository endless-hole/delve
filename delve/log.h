#pragma once

#include <ntddk.h>




#define log_error __noop
#define log_debug __noop
#define log_success __noop



//#define log_error log_error_
//#define log_debug log_debug_
//#define log_success log_success_

void log_error_( const char* fmt, ... );
void log_debug_( const char* fmt, ... );
void log_success_( const char* fmt, ... );