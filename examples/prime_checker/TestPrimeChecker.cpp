#define CATCH_CONFIG_MAIN

#include "libs/catch.hpp"
#include "PrimeChecker.hpp"

TEST_CASE("Test PrimeChecker 1.") {
  PrimeChecker pc;
  REQUIRE(pc.isPrime(5));
}

TEST_CASE("Test PrimeChecker 2.") {
  PrimeChecker pc;
  REQUIRE(pc.isPrime(2));
}

TEST_CASE("Test PrimeChecker 3.") {
  PrimeChecker pc;
  REQUIRE(pc.isPrime(9));
}

TEST_CASE("Test PrimeChecker 4.") {
  PrimeChecker pc;
  REQUIRE(pc.isPrime(13));
}
