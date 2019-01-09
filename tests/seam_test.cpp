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

TEST_CASE("Seam Removal") {
    CImg<unsigned char> img(5,3,1,3);
    Seam s(3);
    s.update(2,10.0);
    s.update(2,10.0);
    s.update(2,10.0);
    auto new_img = s.removeFrom(img);
    REQUIRE(new_img(0,0,0,0) == img(0,0,0,0));
    REQUIRE(new_img(0,0,0,1) == img(0,0,0,1));
    REQUIRE(new_img(0,0,0,2) == img(0,0,0,2));
    REQUIRE(new_img(0,1,0,0) == img(0,1,0,0));
    REQUIRE(new_img(0,1,0,1) == img(0,1,0,1));
    REQUIRE(new_img(0,1,0,2) == img(0,1,0,2));
    REQUIRE(new_img(0,2,0,0) == img(0,2,0,0));
    REQUIRE(new_img(0,2,0,1) == img(0,2,0,1));
    REQUIRE(new_img(0,2,0,2) == img(0,2,0,2));

    REQUIRE(new_img(1,0,0,0) == img(1,0,0,0));
    REQUIRE(new_img(1,0,0,1) == img(1,0,0,1));
    REQUIRE(new_img(1,0,0,2) == img(1,0,0,2));
    REQUIRE(new_img(1,1,0,0) == img(1,1,0,0));
    REQUIRE(new_img(1,1,0,1) == img(1,1,0,1));
    REQUIRE(new_img(1,1,0,2) == img(1,1,0,2));
    REQUIRE(new_img(1,2,0,0) == img(1,2,0,0));
    REQUIRE(new_img(1,2,0,1) == img(1,2,0,1));
    REQUIRE(new_img(1,2,0,2) == img(1,2,0,2));

    REQUIRE(new_img(2,0,0,0) == img(3,0,0,0));
    REQUIRE(new_img(2,0,0,1) == img(3,0,0,1));
    REQUIRE(new_img(2,0,0,2) == img(3,0,0,2));
    REQUIRE(new_img(2,1,0,0) == img(3,1,0,0));
    REQUIRE(new_img(2,1,0,1) == img(3,1,0,1));
    REQUIRE(new_img(2,1,0,2) == img(3,1,0,2));
    REQUIRE(new_img(2,2,0,0) == img(3,2,0,0));
    REQUIRE(new_img(2,2,0,1) == img(3,2,0,1));
    REQUIRE(new_img(2,2,0,2) == img(3,2,0,2));

    REQUIRE(new_img(3,0,0,0) == img(4,0,0,0));
    REQUIRE(new_img(3,0,0,1) == img(4,0,0,1));
    REQUIRE(new_img(3,0,0,2) == img(4,0,0,2));
    REQUIRE(new_img(3,1,0,0) == img(4,1,0,0));
    REQUIRE(new_img(3,1,0,1) == img(4,1,0,1));
    REQUIRE(new_img(3,1,0,2) == img(4,1,0,2));
    REQUIRE(new_img(3,2,0,0) == img(4,2,0,0));
    REQUIRE(new_img(3,2,0,1) == img(4,2,0,1));
    REQUIRE(new_img(3,2,0,2) == img(4,2,0,2));
}