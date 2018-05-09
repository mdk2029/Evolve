#pragma once

#include <array>
#include <tuple>
#include <random>
#include <cmath>
#include <ostream>
#include "memoizer.h"

extern std::default_random_engine& randomEngine();

/**
 * \ingroup Evolve
 *
 * This file defines the NQueens problem as a Specimen that can be used
 * to instantiate the Evolve::Generation class template
 *
 * We currently hardcode N to 8.
 *
 * The problem is modeled as a sequence of 8 numbers which represent the
 * position of a queen on each column of the chess board.
 *
 * See below for definitions of the fitness functions and mating
 */

namespace NQueens {

/// Helper function to construct a std::array<> of N elements using
/// the function F to generate the elements
template<typename F, size_t... Is>
inline
auto make_array(F&& f, std::index_sequence<Is...>) -> std::array<decltype(f()),sizeof...(Is)> {
    return {((void)Is,f())...};
}

/// A sequence of 8 numbers, each representing the position of a queen on a chessboard
/// TODO remove hard-coded N=8
struct Board {
    std::array<std::uint8_t, 8> board_;

    Board(const Board& board) : board_{board.board_}
    {}

    Board(const std::array<std::uint8_t, 8>& b) : board_{b}
    {}

    unsigned numAttackingPairs() const {
        unsigned count{0};
        for(int i = 0; i < 8; i++)
            for(int j = i+1; j < 8; j++)  {
                if( (board_[i] == board_[j]) || (std::abs(board_[j] - board_[i]) == j-i) ) {
                    ++count;
                }
            }
        return count;
    }

    bool solved() const {
        return numAttackingPairs() == 0;
    }

    /// Ordering function needed because we store boards in a memoizing cache
    bool operator< (const Board& rhs) const {
        for(size_t i = 0; i < board_.size(); i++) {
            if(board_[i] != rhs.board_[i]) {
                return board_[i] < rhs.board_[i];
            }
        }
        return false;
    }

    static Board random() {
        auto randomPos = []() {
            static std::uniform_int_distribution<uint8_t> distribution(0,7);
            return distribution(randomEngine());
        };
        Board board{make_array(randomPos,std::make_index_sequence<8>())};
        return board;
    }

};


inline
std::ostream& operator<<(std::ostream& os, const Board& b) {
    os << "[";
    for(const auto& pos : b.board_) {
        os << (unsigned)pos << ',';
    }
    os << "]\n";
    return os;
}


/// The fitness function of the specimen.
/// We have 8C2 = 28 possible attacking pairs
/// The fittest specimen will have 0 attacking pairs
inline
unsigned score(const Board& b) {

    /// This is the actual fitness function
    auto realScore = [](const Board& b) {
        return 28 - b.numAttackingPairs();
    };

    /// We memoize the call since we might be evaulating the same board multiple
    /// times
    using CacheT =  Memoizer::Cache<decltype(realScore), std::decay_t<decltype(b)>>;
    static Memoizer::Memoizer<CacheT, decltype(realScore)> memoizer_s{realScore};

    return memoizer_s(b);
}

/// Given a crossing point, we create two children from two parents
inline
std::tuple<Board, Board> cross(const Board& first, const Board& second, uint8_t crossPoint) {

    Board child1{first}, child2{second};
    std::copy(std::begin(second.board_)+crossPoint, std::end(second.board_), std::begin(child1.board_)+crossPoint);
    std::copy(std::begin(first.board_)+crossPoint, std::end(first.board_), std::begin(child2.board_)+crossPoint);

    return {child1,child2};

}

/// After a child is created, we further mutate it at a random point
inline
Board mutate(const Board& board) {
    static std::uniform_int_distribution<uint8_t> distribution(0,7);

    //select a random point and mutate it
    Board mutated{board};
    unsigned col = distribution(randomEngine());
    mutated.board_[col] = distribution(randomEngine());
    return mutated;
}

/// Mating consists of crossing over followed by a mutation
std::tuple<Board, Board> mate(const Board& first, const Board& second) {

    //Select a random crossover point
    auto crossover = []() {
        static std::uniform_int_distribution<uint8_t> distribution(0,7);
        return distribution(randomEngine());
    };

    auto crossPoint = crossover();
    auto children = cross(first, second, crossPoint);
    return {mutate(std::get<0>(children)), mutate(std::get<1>(children))};
}

bool solved(const Board& b) {
    return b.solved();
}

}
