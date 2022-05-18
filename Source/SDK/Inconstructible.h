#pragma once

#define INCONSTRUCTIBLE(className) \
className() = delete; \
className(className&&) = delete; \
className(const className&) = delete; \
className& operator=(const className&) = delete;
