#pragma once

#ifndef VERIFY

#ifndef NDEBUG
#define VERIFY() true
#else
#define VERIFY() false
#endif

#endif

#if VERIFY()

#include <source_location>
#include "Verifier.h"

#define STRINGIFY(arg) #arg
#define VERIFY_COMPARISON(message, lhs, rhs, comparator) Verifier{ std::source_location::current(), message, STRINGIFY(lhs), STRINGIFY(rhs), lhs, rhs }.verify ## comparator()

#define VERIFY_EQ(message, lhs, rhs) VERIFY_COMPARISON(message, lhs, rhs, Equal)
#define VERIFY_NE(message, lhs, rhs) VERIFY_COMPARISON(message, lhs, rhs, NotEqual)
#define VERIFY_GE(message, lhs, rhs) VERIFY_COMPARISON(message, lhs, rhs, GreaterOrEqual)
#define VERIFY_GT(message, lhs, rhs) VERIFY_COMPARISON(message, lhs, rhs, Greater)
#define VERIFY_IF(shouldVerify, verifier) do { if (shouldVerify) { verifier; } } while (false)

#else

#define VERIFY_EQ(message, lhs, rhs)
#define VERIFY_NE(message, lhs, rhs)
#define VERIFY_GE(message, lhs, rhs)
#define VERIFY_GT(message, lhs, rhs)
#define VERIFY_IF(shouldVerify, verifier)

#endif

