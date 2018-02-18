#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "DummyTestClass.hpp"


// Verification test case to see catch is working.
TEST_CASE("Arbitrary test 1.") {
    DummyTestClass dtc;
    REQUIRE(dtc.check_true());
}
