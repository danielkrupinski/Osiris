#pragma once

#include <array>
#include <gtest/gtest.h>
#include <Utils/Optional.h>

TEST(OptionalBoolTest, DefaultConstructedHasNoValue) {
    EXPECT_FALSE(Optional<bool>{}.hasValue());
}

TEST(OptionalBoolTest, ConstructedFromNulloptHasNoValue) {
    EXPECT_FALSE(Optional<bool>{std::nullopt}.hasValue());
}

TEST(OptionalBoolTest, ConstructedFromTrueHasValueOfTrue) {
    Optional<bool> optional{true};
    EXPECT_TRUE(optional.hasValue());
    EXPECT_EQ(optional.value(), true);
}

TEST(OptionalBoolTest, ConstructedFromFalseHasValueOfFalse) {
    Optional<bool> optional{false};
    EXPECT_TRUE(optional.hasValue());
    EXPECT_EQ(optional.value(), false);
}

struct OptionalBoolEqualOperatorTestParam {
    Optional<bool> lhs{};
    Optional<bool> rhs{};
    bool expectedEqual{};
};

class OptionalBoolEqualOperatorTest : public testing::TestWithParam<OptionalBoolEqualOperatorTestParam> {
};

TEST_P(OptionalBoolEqualOperatorTest, OperatorEqualTo) {
    EXPECT_EQ(GetParam().lhs == GetParam().rhs, GetParam().expectedEqual);
}

INSTANTIATE_TEST_SUITE_P(, OptionalBoolEqualOperatorTest, testing::ValuesIn(
    std::to_array<OptionalBoolEqualOperatorTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedEqual = true},
        {.lhs{std::nullopt}, .rhs{true}, .expectedEqual = false},
        {.lhs{std::nullopt}, .rhs{false}, .expectedEqual = false},
        {.lhs{true}, .rhs{true}, .expectedEqual = true},
        {.lhs{true}, .rhs{false}, .expectedEqual = false},
        {.lhs{true}, .rhs{std::nullopt}, .expectedEqual = false},
        {.lhs{false}, .rhs{false}, .expectedEqual = true},
        {.lhs{false}, .rhs{true}, .expectedEqual = false},
        {.lhs{false}, .rhs{std::nullopt}, .expectedEqual = false}
    })
));

struct OptionalBoolValueOrTestParam {
    Optional<bool> optional{};
    bool defaultValue{};
    bool expectedValue{};
};

class OptionalBoolValueOrTest : public testing::TestWithParam<OptionalBoolValueOrTestParam> {
};

TEST_P(OptionalBoolValueOrTest, ConstructedFromFalseHasValueOfFalse) {
    EXPECT_EQ(GetParam().optional.valueOr(GetParam().defaultValue), GetParam().expectedValue);
}

INSTANTIATE_TEST_SUITE_P(, OptionalBoolValueOrTest, testing::ValuesIn(
    std::to_array<OptionalBoolValueOrTestParam>({
        {.optional{std::nullopt}, .defaultValue = true, .expectedValue = true},
        {.optional{std::nullopt}, .defaultValue = false, .expectedValue = false},
        {.optional{true}, .defaultValue = true, .expectedValue = true},
        {.optional{true}, .defaultValue = false, .expectedValue = true},
        {.optional{false}, .defaultValue = true, .expectedValue = false},
        {.optional{false}, .defaultValue = false, .expectedValue = false}
    })
));

TEST(OptionalTest, DefaultConstructedHasNoValue) {
    EXPECT_FALSE(Optional<int>{}.hasValue());
}

TEST(OptionalTest, ConstructedFromNulloptHasNoValue) {
    EXPECT_FALSE(Optional<int>{std::nullopt}.hasValue());
}

class OptionalValueTest : public testing::TestWithParam<int> {
};

TEST_P(OptionalValueTest, ConstructedFromValueHasThatValue) {
    Optional<int> optional{GetParam()};
    EXPECT_TRUE(optional.hasValue());
    EXPECT_EQ(optional.value(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, OptionalValueTest, testing::Values(0, 10));

struct OptionalEqualOperatorTestParam {
    Optional<int> lhs{};
    Optional<int> rhs{};
    bool expectedEqual{};
};

class OptionalEqualOperatorTest : public testing::TestWithParam<OptionalEqualOperatorTestParam> {
};

TEST_P(OptionalEqualOperatorTest, OperatorEqualTo) {
    EXPECT_EQ(GetParam().lhs == GetParam().rhs, GetParam().expectedEqual);
}

INSTANTIATE_TEST_SUITE_P(, OptionalEqualOperatorTest, testing::ValuesIn(
    std::to_array<OptionalEqualOperatorTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedEqual = true},
        {.lhs{std::nullopt}, .rhs{0}, .expectedEqual = false},
        {.lhs{std::nullopt}, .rhs{10}, .expectedEqual = false},
        {.lhs{0}, .rhs{0}, .expectedEqual = true},
        {.lhs{0}, .rhs{10}, .expectedEqual = false},
        {.lhs{0}, .rhs{std::nullopt}, .expectedEqual = false},
        {.lhs{10}, .rhs{10}, .expectedEqual = true},
        {.lhs{10}, .rhs{0}, .expectedEqual = false},
        {.lhs{10}, .rhs{std::nullopt}, .expectedEqual = false}
    })
));

struct OptionalComparisonOperatorOptionalTestParam {
    Optional<int> lhs{};
    Optional<int> rhs{};
    Optional<bool> expectedResult{};
};

struct OptionalComparisonOperatorValueTestParam {
    Optional<int> lhs{};
    int rhs{};
    Optional<bool> expectedResult{};
};

class OptionalEqualMethodOptionalTest : public testing::TestWithParam<OptionalComparisonOperatorOptionalTestParam> {
};

TEST_P(OptionalEqualMethodOptionalTest, Equal) {
    EXPECT_EQ(GetParam().lhs.equal(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalEqualMethodOptionalTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorOptionalTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{0}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{10}, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs{0}, .expectedResult{true}},
        {.lhs{0}, .rhs{10}, .expectedResult{false}},
        {.lhs{0}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{10}, .rhs{10}, .expectedResult{true}},
        {.lhs{10}, .rhs{0}, .expectedResult{false}},
        {.lhs{10}, .rhs{std::nullopt}, .expectedResult{std::nullopt}}
    })
));

class OptionalEqualMethodValueTest : public testing::TestWithParam<OptionalComparisonOperatorValueTestParam> {
};

TEST_P(OptionalEqualMethodValueTest, Equal) {
    EXPECT_EQ(GetParam().lhs.equal(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalEqualMethodValueTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorValueTestParam>({
        {.lhs{std::nullopt}, .rhs = 0, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs = 10, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs = 0, .expectedResult{true}},
        {.lhs{0}, .rhs = 10, .expectedResult{false}},
        {.lhs{10}, .rhs = 10, .expectedResult{true}},
        {.lhs{10}, .rhs = 0, .expectedResult{false}}
    })
));

class OptionalNotEqualMethodOptionalTest : public testing::TestWithParam<OptionalComparisonOperatorOptionalTestParam> {
};

TEST_P(OptionalNotEqualMethodOptionalTest, NotEqual) {
    EXPECT_EQ(GetParam().lhs.notEqual(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalNotEqualMethodOptionalTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorOptionalTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{0}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{10}, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs{0}, .expectedResult{false}},
        {.lhs{0}, .rhs{10}, .expectedResult{true}},
        {.lhs{0}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{10}, .rhs{10}, .expectedResult{false}},
        {.lhs{10}, .rhs{0}, .expectedResult{true}},
        {.lhs{10}, .rhs{std::nullopt}, .expectedResult{std::nullopt}}
    })
));

class OptionalNotEqualMethodValueTest : public testing::TestWithParam<OptionalComparisonOperatorValueTestParam> {
};

TEST_P(OptionalNotEqualMethodValueTest, NotEqual) {
    EXPECT_EQ(GetParam().lhs.notEqual(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalNotEqualMethodValueTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorValueTestParam>({
        {.lhs{std::nullopt}, .rhs = 0, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs = 10, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs = 0, .expectedResult{false}},
        {.lhs{0}, .rhs = 10, .expectedResult{true}},
        {.lhs{10}, .rhs = 10, .expectedResult{false}},
        {.lhs{10}, .rhs = 0, .expectedResult{true}}
    })
));

class OptionalLessThanMethodOptionalTest : public testing::TestWithParam<OptionalComparisonOperatorOptionalTestParam> {
};

TEST_P(OptionalLessThanMethodOptionalTest, LessThan) {
    EXPECT_EQ(GetParam().lhs.lessThan(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalLessThanMethodOptionalTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorOptionalTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{0}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{10}, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs{0}, .expectedResult{false}},
        {.lhs{0}, .rhs{10}, .expectedResult{true}},
        {.lhs{0}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{10}, .rhs{10}, .expectedResult{false}},
        {.lhs{10}, .rhs{0}, .expectedResult{false}},
        {.lhs{10}, .rhs{std::nullopt}, .expectedResult{std::nullopt}}
    })
));

class OptionalLessThanMethodValueTest : public testing::TestWithParam<OptionalComparisonOperatorValueTestParam> {
};

TEST_P(OptionalLessThanMethodValueTest, LessThan) {
    EXPECT_EQ(GetParam().lhs.lessThan(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalLessThanMethodValueTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorValueTestParam>({
        {.lhs{std::nullopt}, .rhs = 0, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs = 10, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs = 0, .expectedResult{false}},
        {.lhs{0}, .rhs = 10, .expectedResult{true}},
        {.lhs{10}, .rhs = 10, .expectedResult{false}},
        {.lhs{10}, .rhs = 0, .expectedResult{false}}
    })
));

class OptionalGreaterThanMethodOptionalTest : public testing::TestWithParam<OptionalComparisonOperatorOptionalTestParam> {
};

TEST_P(OptionalGreaterThanMethodOptionalTest, GreaterThan) {
    EXPECT_EQ(GetParam().lhs.greaterThan(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalGreaterThanMethodOptionalTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorOptionalTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{0}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{10}, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs{0}, .expectedResult{false}},
        {.lhs{0}, .rhs{10}, .expectedResult{false}},
        {.lhs{0}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{10}, .rhs{10}, .expectedResult{false}},
        {.lhs{10}, .rhs{0}, .expectedResult{true}},
        {.lhs{10}, .rhs{std::nullopt}, .expectedResult{std::nullopt}}
    })
));

class OptionalGreaterThanMethodValueTest : public testing::TestWithParam<OptionalComparisonOperatorValueTestParam> {
};

TEST_P(OptionalGreaterThanMethodValueTest, GreaterThan) {
    EXPECT_EQ(GetParam().lhs.greaterThan(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalGreaterThanMethodValueTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorValueTestParam>({
        {.lhs{std::nullopt}, .rhs = 0, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs = 10, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs = 0, .expectedResult{false}},
        {.lhs{0}, .rhs = 10, .expectedResult{false}},
        {.lhs{10}, .rhs = 10, .expectedResult{false}},
        {.lhs{10}, .rhs = 0, .expectedResult{true}}
    })
));

class OptionalLessEqualMethodOptionalTest : public testing::TestWithParam<OptionalComparisonOperatorOptionalTestParam> {
};

TEST_P(OptionalLessEqualMethodOptionalTest, LessEqual) {
    EXPECT_EQ(GetParam().lhs.lessEqual(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalLessEqualMethodOptionalTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorOptionalTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{0}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{10}, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs{0}, .expectedResult{true}},
        {.lhs{0}, .rhs{10}, .expectedResult{true}},
        {.lhs{0}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{10}, .rhs{10}, .expectedResult{true}},
        {.lhs{10}, .rhs{0}, .expectedResult{false}},
        {.lhs{10}, .rhs{std::nullopt}, .expectedResult{std::nullopt}}
    })
));

class OptionalLessEqualMethodValueTest : public testing::TestWithParam<OptionalComparisonOperatorValueTestParam> {
};

TEST_P(OptionalLessEqualMethodValueTest, LessEqual) {
    EXPECT_EQ(GetParam().lhs.lessEqual(GetParam().rhs), GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalLessEqualMethodValueTest, testing::ValuesIn(
    std::to_array<OptionalComparisonOperatorValueTestParam>({
        {.lhs{std::nullopt}, .rhs = 0, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs = 10, .expectedResult{std::nullopt}},
        {.lhs{0}, .rhs = 0, .expectedResult{true}},
        {.lhs{0}, .rhs = 10, .expectedResult{true}},
        {.lhs{10}, .rhs = 10, .expectedResult{true}},
        {.lhs{10}, .rhs = 0, .expectedResult{false}}
    })
));

struct OptionalBinaryOperatorOptionalTestParam {
    Optional<int> lhs{};
    Optional<int> rhs{};
    Optional<int> expectedResult{};
};

struct OptionalBinaryOperatorValueTestParam {
    Optional<int> lhs{};
    int rhs{};
    Optional<int> expectedResult{};
};

class OptionalMinusOperatorOptionalTest : public testing::TestWithParam<OptionalBinaryOperatorOptionalTestParam> {
};

TEST_P(OptionalMinusOperatorOptionalTest, MinusOperator) {
    EXPECT_EQ(GetParam().lhs - GetParam().rhs, GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalMinusOperatorOptionalTest, testing::ValuesIn(
    std::to_array<OptionalBinaryOperatorOptionalTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{1}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{10}, .expectedResult{std::nullopt}},
        {.lhs{1}, .rhs{1}, .expectedResult{0}},
        {.lhs{1}, .rhs{10}, .expectedResult{-9}},
        {.lhs{1}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{10}, .rhs{10}, .expectedResult{0}},
        {.lhs{10}, .rhs{1}, .expectedResult{9}},
        {.lhs{10}, .rhs{std::nullopt}, .expectedResult{std::nullopt}}
    })
));

class OptionalMinusOperatorValueTest : public testing::TestWithParam<OptionalBinaryOperatorValueTestParam> {
};

TEST_P(OptionalMinusOperatorValueTest, MinusOperator) {
    EXPECT_EQ(GetParam().lhs - GetParam().rhs, GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalMinusOperatorValueTest, testing::ValuesIn(
    std::to_array<OptionalBinaryOperatorValueTestParam>({
        {.lhs{std::nullopt}, .rhs = 1, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs = 10, .expectedResult{std::nullopt}},
        {.lhs{1}, .rhs = 1, .expectedResult{0}},
        {.lhs{1}, .rhs = 10, .expectedResult{-9}},
        {.lhs{10}, .rhs = 10, .expectedResult{0}},
        {.lhs{10}, .rhs = 1, .expectedResult{9}}
    })
));

class OptionalBitandOperatorOptionalTest : public testing::TestWithParam<OptionalBinaryOperatorOptionalTestParam> {
};

TEST_P(OptionalBitandOperatorOptionalTest, BitandOperator) {
    EXPECT_EQ(GetParam().lhs & GetParam().rhs, GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalBitandOperatorOptionalTest, testing::ValuesIn(
    std::to_array<OptionalBinaryOperatorOptionalTestParam>({
        {.lhs{std::nullopt}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{3}, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs{10}, .expectedResult{std::nullopt}},
        {.lhs{3}, .rhs{3}, .expectedResult{3}},
        {.lhs{3}, .rhs{4}, .expectedResult{0}},
        {.lhs{3}, .rhs{10}, .expectedResult{2}},
        {.lhs{10}, .rhs{10}, .expectedResult{10}},
        {.lhs{4}, .rhs{3}, .expectedResult{0}},
        {.lhs{10}, .rhs{3}, .expectedResult{2}},
        {.lhs{3}, .rhs{std::nullopt}, .expectedResult{std::nullopt}},
        {.lhs{4}, .rhs{std::nullopt}, .expectedResult{std::nullopt}}
    })
));

class OptionalBitandOperatorValueTest : public testing::TestWithParam<OptionalBinaryOperatorValueTestParam> {
};

TEST_P(OptionalBitandOperatorValueTest, BitandOperator) {
    EXPECT_EQ(GetParam().lhs & GetParam().rhs, GetParam().expectedResult);
}

INSTANTIATE_TEST_SUITE_P(, OptionalBitandOperatorValueTest, testing::ValuesIn(
    std::to_array<OptionalBinaryOperatorValueTestParam>({
        {.lhs{std::nullopt}, .rhs = 3, .expectedResult{std::nullopt}},
        {.lhs{std::nullopt}, .rhs = 10, .expectedResult{std::nullopt}},
        {.lhs{3}, .rhs = 3, .expectedResult{3}},
        {.lhs{3}, .rhs = 4, .expectedResult{0}},
        {.lhs{3}, .rhs = 10, .expectedResult{2}},
        {.lhs{10}, .rhs = 10, .expectedResult{10}},
        {.lhs{4}, .rhs = 3, .expectedResult{0}},
        {.lhs{10}, .rhs = 3, .expectedResult{2}},
    })
));
