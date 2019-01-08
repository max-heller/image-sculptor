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
    s.update(4, 1);
    REQUIRE(s.getPositions().size() == 1);
    REQUIRE(s.getPositions().at(0) == 4);
    REQUIRE(s.getEnergy() == 1);
    s.update(2, 2);
    REQUIRE(s.getPositions().size() == 2);
    REQUIRE(s.getPositions().at(1) == 2);
    REQUIRE(s.getEnergy() == 3);
}
TEST_CASE("Seam Comparison") {
    Seam s1(10), s2(10);
    s1.update(5, 10.2);
    s2.update(4, 4.5);
    REQUIRE(s2 < s1);
    REQUIRE(!(s1 < s2));
}