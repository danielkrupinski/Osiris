#pragma once

#include <string>
#include <sstream>

class pstring : public std::string
{
public:
	pstring() : std::string() {}

	template<typename T>
	pstring(const T v) : std::string(v) {}

	template<typename T>
	pstring& operator<<(const T s)
	{
		std::stringstream stream;
		stream << *this;
		stream << s;
		*this = stream.str();
		return *this;
	}

	pstring& operator+(const unsigned int i)
	{
		std::stringstream stream;
		stream << *this;
		stream << i;
		*this = stream.str();
		return *this;
	}
};