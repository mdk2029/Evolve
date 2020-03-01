#pragma once

#include <array>
#include <tuple>
#include <random>
#include <cmath>
#include <ostream>
#include "memoizer.h"
#include <optional>

extern std::default_random_engine& randomEngine();

/**
 * \ingroup Evolve
 *
 * This file defines the Knights tour problem as a Specimen that can be used
 * to instantiate the Evolve::Generation class template
 *
 * We currently hardcode the chess board to be of size 8x8
 *
 * The problem is modeled as a sequence of 63 moves. The tour always start from
 * position (4,4) (e5 in chess notation). Each move represents the row and col delta
 * from the current position
 */

namespace KnightsTour {

/// Represents a move by storing its row and column deltas from the current position
struct Mov {

    int rdelta_;
    int cdelta_;

    constexpr Mov(int rd, int cd) :
        rdelta_{rd},
        cdelta_{cd}
    {}

    constexpr Mov() : Mov(1,2){}
    constexpr Mov(const Mov&) = default;
    constexpr Mov& operator=(const Mov&) = default;

    constexpr bool operator<(const Mov& rhs) const {
        return rdelta_ == rhs.rdelta_ ? cdelta_ < rhs.cdelta_ : rdelta_ < rhs.rdelta_;
    }

    constexpr bool operator==(const Mov& rhs) const {
        return rdelta_ == rhs.rdelta_ && cdelta_ == rhs.cdelta_;
    }

};

/// These are the possible moves from a position. Depending on the position
/// and the tour so far, not all of these moves will be applicable
constexpr auto moves = {Mov{1,2},Mov{1,-2},Mov{2,1},Mov{2,-1},
                        Mov{-1,2},Mov{-1,-2},Mov{-2,1},Mov{-2,-1}
                       };

/// Represents a position on a chess board
struct Pos {
    int row_;
    int col_;

    constexpr Pos(int row, int col) :
        row_{row}, col_{col}
    {}
};

/// Helper function to construct an array of N items obtained by the generating function
/// f
template<typename F, size_t... Is>
inline auto make_array(F f, std::index_sequence<Is...>) -> std::array<decltype(f()),sizeof...(Is)> {
    return {((void)Is, f())...};
}

/// Represents a sequence of 63 moves
struct Tour {
    static constexpr auto length = 63;
    static constexpr auto numRows = 8;
    static constexpr auto numCols = 8;
    static constexpr auto startPos = Pos{4,4};

    std::array<Mov,length> tour_;

    /// An inner helper class that tracks the tour so far
    struct Board {
        unsigned board_[numRows][numCols];
        unsigned nextMovIdx { 2 };

        Board() {
            for(auto r = 0; r < numRows; r++) {
                for(auto c = 0; c < numCols; c++) {
                    board_[r][c] = 0;
                }
            }
            board_[startPos.row_][startPos.col_] = 1;
        }

        /// Given the board so far, can mov be applied? If so, apply
        /// mov and update the board
        std::optional<Pos> maybeApplyMove(const Pos& old, const Mov& mov) {
            auto newRow = old.row_ + mov.rdelta_;
            auto newCol = old.col_ + mov.cdelta_;
            if (newRow >= 0 && newRow <= 7 && newCol >= 0 && newCol <=7
                    && !board_[newRow][newCol]) {
                board_[newRow][newCol] = nextMovIdx++;
                return Pos{newRow, newCol};
            } else {
                return std::nullopt;
            }
        }

        /// Apply the tour for as long as possible. Note that not all
        /// tours are valid so in general only a prefix of the tour
        /// will be applicable before we run into a deadend
        void applyTour(const Tour& tour) {
            Pos pos = startPos;
            for(const auto& mov : tour.tour_) {
                auto newPos = maybeApplyMove(pos,mov);
                if(newPos) {
                    pos = *newPos;
                } else {
                    break;
                }
            }
        }

        unsigned numMoves() const {
            return nextMovIdx - 2;
        }
    };

    Tour(const std::array<Mov,length>& other) : tour_{other}
    {}

    unsigned numValidSteps() const {
        Board board;
        board.applyTour(*this);
        return board.numMoves();
    }

    operator Board() const {
        Board board;
        board.applyTour(*this);
        return board;
    }

    bool solved() const {
        return numValidSteps() == Tour::length;
    }

    /// Ordering function needed because we store tours in a memoizing cache
    bool operator< (const Tour& rhs) const {
        for(size_t i = 0; i < rhs.tour_.size(); i++) {
            if(!(tour_[i] == rhs.tour_[i])) {
                return tour_[i] < rhs.tour_[i];
            }
        }
        return false;
    }

    static Tour random() {
        auto randomMove = []() {
            static std::uniform_int_distribution<uint8_t> distribution(0,7);
            return *(std::cbegin(moves) + distribution(randomEngine()));
        };

        Tour tour{make_array(randomMove,std::make_index_sequence<Tour::length>())};
        return tour;
    }
};

inline
std::ostream& operator<<(std::ostream& os, const Tour& t) {
    Tour::Board board{t};
    std::string line{"---------------------------------"};
    os << line << '\n';
    for(int r : {7,6,5,4,3,2,1,0}) {
        for(int c : {0,1,2,3,4,5,6,7})
        {
            if(board.board_[r][c] >= 10) {
                os << " ";
            } else {
                os << "  ";
            }
            os << board.board_[r][c] << " ";
            if(c == 7) {
                os << '\n' << line << '\n';
            }
        }
    }
    return os;
}

/// The fitness function of the specimen
/// The fittest specimen will have a score of 63
/// corresponding to a solved Tour
inline
unsigned score(const Tour& t) {

    /// This is the actual fitness function
    auto realScore = [](const Tour& t) {
        return t.numValidSteps();
    };

    /// We memoize the call since we might be evaulating the same tour multiple
    /// times
    using CacheT = Memoizer::Cache<decltype(realScore), std::decay_t<decltype(t)>>;
    static Memoizer::Memoizer<CacheT, decltype(realScore)> memoizer_s{realScore};

    return memoizer_s(t);
}

inline
std::tuple<Tour, Tour> cross(const Tour& first, const Tour& second, size_t crossPoint) {

    Tour child1{first}, child2{second};
    std::copy(std::begin(second.tour_)+crossPoint, std::end(second.tour_), std::begin(child1.tour_)+crossPoint);
    std::copy(std::begin(first.tour_)+crossPoint, std::end(first.tour_), std::begin(child2.tour_)+crossPoint);

    return {child1,child2};

}

inline
Tour mutate(const Tour& tour) {
    static std::uniform_int_distribution<uint8_t> distribution1(0,7);
    static std::uniform_int_distribution<unsigned> distribution2(0,63);

    //select a random point and mutate it
    Tour mutated{tour};
    unsigned step = distribution2(randomEngine());
    Mov mov = *(std::cbegin(moves) + distribution1(randomEngine()));
    mutated.tour_[step] = mov;
    return mutated;
}

/// It can be seen from the Genetics Algo literature that just crossover and mutation
/// are not enough to generate valid solutions to this problem when using discretized
/// evolution. A common approach suggested is to extend the semantics of mutation
/// with "nurture" - i.e. increase the fitness of a child. To do this
/// we fix up a child specimen so that it has a longer valid prefix tour.
inline
Tour extend(const Tour& t) {

    Tour tour{t};
    Tour::Board board;

    Pos pos{Tour::startPos};
    for(size_t idx = 0; idx < Tour::length; idx++) {
        auto newPos = board.maybeApplyMove(pos,tour.tour_[idx]);
        if(newPos) {
            pos = *newPos;
        } else {
            bool extended = false;
            /// Todo: Apply Warnsdorff's heuristic to select a move from
            /// multiple applicable moves
            for(const auto& mov : moves) {
                newPos = board.maybeApplyMove(pos,mov);
                if(newPos) {
                    pos = *newPos;
                    tour.tour_[idx] = mov;
                    extended = true;
                    break;
                }
            }
            if(!extended) {
                break;
            }
        }
    }

    return tour;
}

/// Mating involves selecting parents, creating offsprings, mutating children
/// and then extending the children
inline
std::tuple<Tour, Tour> mate(const Tour& first, const Tour& second) {

    //Select a random crossover point
    auto crossover = []() {
        static std::uniform_int_distribution<unsigned> distribution(0,Tour::length);
        return distribution(randomEngine());
    };

    auto crossPoint = crossover();
    auto children = cross(first, second, crossPoint);
    return {extend(mutate(std::get<0>(children))), extend(mutate(std::get<1>(children)))};
}

inline
bool solved(const Tour& t) {
    return t.solved();
}

}
