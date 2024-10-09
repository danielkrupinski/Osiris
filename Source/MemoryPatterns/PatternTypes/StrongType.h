#pragma once

#define STRONG_TYPE(name, ...) \
struct name { using Type = __VA_ARGS__; };
