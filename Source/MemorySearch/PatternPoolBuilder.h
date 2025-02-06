#pragma once

#include <Utils/StrongTypeAlias.h>
#include <Utils/TypeList.h>

#include "CodePattern.h"
#include "CodePatternOperation.h"

template <typename TempPatternPool, typename PatternTypesList = TypeList<>>
class PatternPoolBuilder {
public:
    TempPatternPool tempPool;

    using PatternTypes = PatternTypesList;

    [[nodiscard]] consteval auto addPatterns(auto f) noexcept
    {
        return f(*this);
    }

    template <typename PatternType, CodePattern Pattern>
    [[nodiscard]] consteval auto addPattern() noexcept
    {
        static_assert(sizeof(UnpackStrongTypeAliasT<PatternType>) == 8 || Pattern.operation == CodePatternOperation::Read, "Incorrect result size, missing .read() in pattern declaration?");
        tempPool.template addPattern<Pattern>();
        return PatternPoolBuilder<TempPatternPool, typename PatternTypesList::template add<PatternType>>{tempPool};
    }
};
