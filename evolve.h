#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <iostream>
#include <array>
#include <tuple>
#include <cassert>

/**
 * \defgroup Evolve A simple class to solve problems using evolution
 *
 */

namespace Evolve {

/**
 * \ingroup Evolve
 *
 * @brief Generation
 *
 * This class represents a generation during evolution. We use a discretized
 * evolution scheme where a generation of specimens mate as per their fitness functions,
 * create offsprings and then the offsprings constitute the next generation.
 *
 * This class takes in a Specimen type as a template parameter. The semantics
 * of the fitness functions, mating etc are delegated to the specimen class. Each
 * specimen class needs to define these functions. The operations on the specimen
 * type are found via ADL lookup so the Specimen types should ideally be in their
 * own namespaces
 *
 * We solve two classic chess problems using this approach - knight's tour and NQueens
 * knights_tour.h and nqueens.h provide the specimen class for these two problems
 * respectively
 *
 * TODO - Use concepts to specify the requirements of the Specimen type(s)
 * TODO - Use continuous evolution where there are not discrete _generation_ units.
 *
 */

template<typename Specimen>
class Generation {

private:

    /// These are the specimens of the current generation. They
    /// will mate as per their fitness functions and create offsprings
    /// The offsprings will constitute the next generation
    std::vector<Specimen> specimens_;

    /// Each Specimen type defines a scoring function to determine
    /// how fit a specimen is
    std::vector<unsigned> fitnessScores_;

    /// Parents will be chosen as per the specimen's fitness. More fit
    /// specimen will have a higher liklihood of being chosen.
    std::vector<std::tuple<size_t,size_t>> parents_;

    /// Two parents will be combined to form two children. The combination
    /// happens by selecting a random crossover point and then creating hybrid
    /// specimen by combing parts of the parents from different sides of the
    /// crossover point. Each child is further mutated at a random point.
    std::vector<Specimen> children_;

    /// After a generation has produced children, we see if there are any solutions
    /// and add them here.
    std::vector<Specimen> solutions_;

public:
    template<typename Iterator>
    Generation(Iterator begin, Iterator end) :
        specimens_{begin,end}
    {
        assert(specimens_.size() >=2 );
        fitnessScores_.reserve(specimens_.size());
    }

    template<typename SpecimenCont>
    Generation(SpecimenCont&& cont) : specimens_{std::move(cont)}
    {
        assert(specimens_.size() >=2 );
        fitnessScores_.reserve(specimens_.size());
    }

    void promote() {
        specimens_ = children_;
        fitnessScores_.clear();
        parents_.clear();
        children_.clear();
    }

    bool hasSolutions() const {
        return !solutions_.empty();
    }

    unsigned maxScore() const {
        return *(std::max_element(std::begin(fitnessScores_), std::end(fitnessScores_)));
    }

    void circleOfLife() {
        scoreSpecimens().selectPairs().makeOffSprings().promote();
    }

private:

    Generation& scoreSpecimens() {
        std::for_each(std::begin(specimens_), std::end(specimens_),
                      [this](auto&& specimen) {
            fitnessScores_.push_back(score(specimen));
        });
        return *this;
    }

    /// Select parents. More fit specimen will have a higher liklihood of
    /// being selected as a pair. The same specimen may mate with itself
    Generation& selectPairs() {
        std::default_random_engine generator;
        std::discrete_distribution<int> distribution{std::begin(fitnessScores_), std::end(fitnessScores_)};
        for(int i = 0; i < specimens_.size(); i+=2) {
            parents_.emplace_back(distribution(generator), distribution(generator));
        }
        return *this;
    }

    Generation& makeOffSprings() {
        for(const auto& mateIds : parents_) {
            const Specimen& parent1 = specimens_[std::get<0>(mateIds)];
            const Specimen& parent2 = specimens_[std::get<1>(mateIds)];
            Specimen child1{parent1}, child2{parent2};
            std::tie(child1,child2) = mate(parent1,parent2);
            for(const auto& child : {child1,child2}) {
                if(solved(child)) {
                    std::cout << "Found a solution : \n" << child << std::endl;
                    solutions_.push_back(child);
                    //Insert a random child to compensate for the specimen
                    //that has evolved to perfection and has escaped
                    children_.push_back(Specimen::random());
                } else {
                    children_.push_back(std::move(child));
                }
            }
        }
        return *this;
    }


};

/// We stop when we have atleast one solution
template<typename Specimen>
void evolve(Generation<Specimen>& curr) {
    int idx{0};
    while(!curr.hasSolutions()) {
        curr.circleOfLife();
        if(++idx % 1000 == 0) {
            unsigned maxScore = curr.maxScore();
            std::cout << "Generation : " << idx << ", maxScore = " << maxScore << std::endl;
        }
    }
}

}
