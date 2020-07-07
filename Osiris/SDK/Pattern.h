#pragma once
#include <array>
#include <string>
#include <type_traits>
#include <Windows.h>
#include <Psapi.h>


#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
namespace Pattern
{
	unsigned int FindPattern(std::string, std::string, bool relativeOffset = false) noexcept;
}