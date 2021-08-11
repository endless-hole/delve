#pragma once

#include <string>
#include <iostream>

#define LOG(...) Log::Log(__FUNCTION__, __VA_ARGS__)

class Log
{
public:
	template <typename ... args>
	explicit Log( std::string function, args ... to_print )
	{
		std::cout << '[' << function << "] ";
		print( to_print ... );
	}

private:
	template <typename T>
	static void print( T only ) { std::cout << only << std::endl; }

	template <typename T, typename ... args>
	void print( T current, args ... next )
	{
		std::cout << current << ' ';
		print( next... );
	}
};
