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

    SECTION("positive angle") {
        CHECK(Helpers::deg2rad(720.0f) == Approx(4*pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(540.0f) == Approx(3*pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(360.0f) == Approx(2*pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(180.0f) == Approx(1*pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(90.0f) == Approx(0.5f * pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(45.0f) == Approx(0.25f * pi).epsilon(floatEpsilon));
        CHECK(Helpers::deg2rad(9.0f) == Approx(0.05f * pi).epsilon(floatEpsilon));
    }
}

TEST_CASE("radians to degrees", "[Helpers]") {
    SECTION("negative angle") {
        CHECK(Helpers::rad2deg(-4*pi) == Approx(-720.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(-3*pi) == Approx(-540.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(-2*pi) == Approx(-360.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(-1*pi) == Approx(-180.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(-0.5f * pi) == Approx(-90.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(-0.25f * pi) == Approx(-45.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(-0.05f * pi) == Approx(-9.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(-0.0f * pi) == Approx(-0.0f).epsilon(floatEpsilon));
    }

    SECTION("zero angle") {
        CHECK(Helpers::rad2deg(0.0f) == Approx(0.0f).epsilon(floatEpsilon));
    }

    SECTION("positive angle") {
        CHECK(Helpers::rad2deg(4*pi) == Approx(720.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(3*pi) == Approx(540.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(2*pi) == Approx(360.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(1*pi) == Approx(180.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(0.5f * pi) == Approx(90.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(0.25f * pi) == Approx(45.0f).epsilon(floatEpsilon));
        CHECK(Helpers::rad2deg(0.05f * pi) == Approx(9.0f).epsilon(floatEpsilon));
    }
}
