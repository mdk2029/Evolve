#define CATCH_CONFIG_MAIN

#include <random>
#include <catch2/catch.hpp>
#include "evolve.h"
#include "nqueens.h"
#include "memoizer.h"
#include <iostream>

class MySpecimen {
public:
    MySpecimen() {
    }

    MySpecimen(MySpecimen&& ) {
    }

};

TEST_CASE("generationConstructionTest", "[evolve]") {

    std::vector<MySpecimen> specimens;
    specimens.push_back(std::move(MySpecimen()));
    specimens.push_back(std::move(MySpecimen()));
    specimens.push_back(std::move(MySpecimen()));
    Evolve::Generation<MySpecimen> generation(std::make_move_iterator(std::begin(specimens)),
                                              std::make_move_iterator(std::end(specimens)));

    REQUIRE(!generation.hasSolutions());
}

TEST_CASE("nqueens") {
    std::array<std::uint8_t, 8> defaultArray = {};
    NQueens::Board boarda{defaultArray}, boardb{defaultArray}, boardc{defaultArray};
    for(uint8_t idx = 0; idx < boarda.board_.size(); idx++) {
        boarda.board_[idx] = idx;
        boardb.board_[idx] = idx+1;
        boardc.board_[idx] = idx;
    }

    REQUIRE(boarda < boardb);
    REQUIRE_FALSE(boardb < boarda);

    REQUIRE_FALSE(boarda < boardc);
    REQUIRE_FALSE(boardc < boarda);
}

TEST_CASE("memoizer") {

    int global{0};

    auto dummyScore = [&global](int a, int b) {
        global++;
        return a + b;
    };

    using CacheT = Memoizer::Cache<decltype(dummyScore), int, int>;
    static Memoizer::Memoizer<CacheT, decltype(dummyScore)> memoizer_s{dummyScore};

    REQUIRE(memoizer_s(5,10) == 15);
    REQUIRE(global == 1);

    REQUIRE(memoizer_s(5,10) == 15);
    REQUIRE(global == 1);
}

