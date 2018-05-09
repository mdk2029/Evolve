#include <random>
#include "gtest/gtest.h"
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

std::default_random_engine& randomEngine() {
    static std::default_random_engine dre{std::random_device()()};
    return dre;
}


GTEST_TEST(Evolve, generationConstructionTest) {

    std::vector<MySpecimen> specimens;
    specimens.push_back(std::move(MySpecimen()));
    specimens.push_back(std::move(MySpecimen()));
    specimens.push_back(std::move(MySpecimen()));
    Evolve::Generation<MySpecimen> generation(std::make_move_iterator(std::begin(specimens)),
                                              std::make_move_iterator(std::end(specimens)));
}

GTEST_TEST(NQueens, BoardTest) {
    std::array<std::uint8_t, 8> defaultArray = {};
    NQueens::Board boarda{defaultArray}, boardb{defaultArray}, boardc{defaultArray};
    for(uint8_t idx = 0; idx < boarda.board_.size(); idx++) {
        boarda.board_[idx] = idx;
        boardb.board_[idx] = idx+1;
        boardc.board_[idx] = idx;
    }

    ASSERT_LT(boarda, boardb);
    ASSERT_FALSE(boardb < boarda);

    ASSERT_FALSE(boarda < boardc);
    ASSERT_FALSE(boardc < boarda);
}

GTEST_TEST(Memoizer, memoizerTest) {

    int global{0};

    auto dummyScore = [&global](int a, int b) {
        global++;
        return a + b;
    };

    using CacheT = Memoizer::Cache<decltype(dummyScore), int, int>;
    static Memoizer::Memoizer<CacheT, decltype(dummyScore)> memoizer_s{dummyScore};

    ASSERT_EQ(memoizer_s(5,10), 15);
    ASSERT_EQ(global, 1);

    ASSERT_EQ(memoizer_s(5,10), 15);
    ASSERT_EQ(global, 1);
}

