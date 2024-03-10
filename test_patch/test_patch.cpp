#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// test patch.cpp
#include "catch.hpp"
#include "..\post_fire_simulation\patch.h"
#include <vector>
#include <string>

TEST_CASE("Test N_seeds initialization of a patch object") {
    std::string patch_id = "0_0";                  // patch id formatted as "x_y"
    std::vector<int> x_y_cor = {0, 0};             // x and y coordinates of the patch, also used to register the seed landing location
    std::vector<int> N_seeds = {-1, 0};             // number of seeds
    patch p(patch_id, x_y_cor, {-5, 0});
    int i = 0;

    SECTION("Test N_seeds initialization") {
        REQUIRE(p.N_seeds[0] == 0);
        REQUIRE(p.N_seeds[1] == 0);
    }
//    SECTION("test2") {
//        REQUIRE(i > 1);
//    }
}

