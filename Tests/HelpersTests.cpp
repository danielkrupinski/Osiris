#include <numbers>

#include "catch.hpp"

#include "../Osiris/Helpers.h"

constexpr auto pi = std::numbers::pi_v<float>;
constexpr auto floatEpsilon = std::numeric_limits<float>::epsilon();

TEST_CASE("degrees to radians", "[Helpers]") {
    SECTION("negative angle") {
        CHECK(Helpers::deg2rad(-720.0f) == Approx(-4*pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(-540.0f) == Approx(-3*pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(-360.0f) == Approx(-2*pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(-180.0f) == Approx(-1*pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(-90.0f) == Approx(-0.5f * pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(-45.0f) == Approx(-0.25f * pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(-9.0f) == Approx(-0.05f * pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(-0.0f) == Approx(-0.0f * pi).epsilon(floatEpsilon));
    }

    SECTION("zero angle") {
        CHECK(Helpers::deg2rad(0.0f) == Approx(0.0f).epsilon(floatEpsilon));
    }
}
