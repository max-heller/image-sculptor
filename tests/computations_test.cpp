#include "catch.hpp"
#include "computations.hpp"

TEST_CASE("Brightness Computation") {
    CImg<unsigned char> im(3, 3, 1, 3);
    im.fill(100, 50);
    auto brightness = get_brightness(im);
    cimg_forXY(brightness, x, y) {
        if ((x + y) % 2 == 0)
            REQUIRE(brightness(x,y) == 83);
        else
            REQUIRE(brightness(x,y) == 66);
    }
}

TEST_CASE("Energy Computation") {
    SECTION("Uniform") {
        CImg<unsigned char> brightness(3,3,1,1);
        brightness.fill(100);
        auto energy = calculate_energy(brightness);
        cimg_forXY(energy, x, y) {
            REQUIRE(energy(x,y) == 0);
        }
    }
    SECTION("3x4 Gradient") {
        CImg<unsigned char> brightness(3,4,1,1);
        brightness.fill(100,50,0);
        auto energy = calculate_energy(brightness);
        REQUIRE(energy(1,1) == 400.0);
        REQUIRE(energy(1,2) == 400.0);
    }
    SECTION("4x4 Gradient") {
        CImg<unsigned char> brightness(4,4,1,1);
        brightness.fill(100,50,0,100);
        auto energy = calculate_energy(brightness);
        REQUIRE(energy(1,1) == 400.0);
        REQUIRE(energy(1,2) == 400.0);
        REQUIRE(energy(2,1) == 200.0);
        REQUIRE(energy(2,2) == 200.0);
    }
    SECTION("Staggered grid") {
        CImg<unsigned char> brightness(4,4,1,1);
        brightness.fill(100,0,100,0,100,0,100);
        auto energy = calculate_energy(brightness);
        REQUIRE(energy(1,1) == 0.0);
        REQUIRE(energy(1,2) == (float)sqrt(20000));
        REQUIRE(energy(2,1) == 200.0);
        REQUIRE(energy(2,2) == 0.0);
    }
}