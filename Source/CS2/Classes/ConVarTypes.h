#pragma once

#define CONVAR_STRINGIFY(name) #name
#define CONVAR(name, valueType) \
struct name { \
    static constexpr auto kName{CONVAR_STRINGIFY(name)}; \
    using ValueType = valueType; \
}

namespace cs2
{

CONVAR(mp_teammates_are_enemies, bool);

}

#undef CONVAR_STRINGIFY
