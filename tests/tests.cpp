#include "catch.hpp"
#include "seam.hpp"

TEST_CASE("Seam Instantiation") {
    int height = 1080;
    Seam s(height);
    REQUIRE(s.getPositions().size() == 0);
    REQUIRE(s.getPositions().capacity() >= height);
    REQUIRE(s.getEnergy() == 0);
}

TEST_CASE("Seam Updating") {
    Seam s(10);
    s.update(1, 1);
    REQUIRE(s.getPositions().size() == 1);
    REQUIRE(s.getEnergy() == 1);
    s.update(2, 2);
    REQUIRE(s.getPositions().size() == 2);
    REQUIRE(s.getEnergy() == 3);
}