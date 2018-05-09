#include <random>

std::default_random_engine& randomEngine() {
    static std::default_random_engine dre{std::random_device()()};
    return dre;
}
