#pragma once

#define INCONSTRUCTIBLE(className) \
className() = delete; \
className(className&&) = delete; \
className(const className&) = delete;
