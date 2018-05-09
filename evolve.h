#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <iostream>
#include <array>
#include <tuple>
#include <cassert>
#include "nqueens.h"

namespace Evolve {

template<typename Specimen>
class Generation {

private:

    std::vector<Specimen> specimens_;
    std::vector<unsigned> fitnessScores_;
    std::vector<std::tuple<size_t,size_t>> parents_;
    std::vector<Specimen> children_;

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

    size_t numChildren() const {
        return children_.size();
    }

    size_t numSpecimen() const {
        return specimens_.size();
    }

    unsigned maxScore() const {
        return *(std::max_element(std::begin(fitnessScores_), std::end(fitnessScores_)));
    }

    Generation& scoreSpecimens() {
        std::for_each(std::begin(specimens_), std::end(specimens_),
                      [this](auto&& specimen) {
            fitnessScores_.push_back(score(specimen));
        });
        return *this;
    }

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
                    children_.clear();
                    return *this;
                }
                children_.push_back(std::move(child));
            }
        }
        return *this;
    }

};

template<typename Specimen>
void evolve(Generation<Specimen>& curr) {
    int idx{0};
    while(curr.numSpecimen() > 2) {
        curr.scoreSpecimens().selectPairs().makeOffSprings();
        if(++idx % 1000 == 0) {
            unsigned maxScore = curr.maxScore();
            std::cout << "Generation : " << idx << ", maxScore = " << maxScore << std::endl;
        }
        curr.promote();
    };
}

}
