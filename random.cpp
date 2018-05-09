#include <random>

std::default_random_engine& randomEngine() {
    /// std::random_device() can be a syscall (On linux, it reads /dev/urandom)
    /// so do not call it directly. Instead we use it to seed std::default_random_engine
    /// which is a PRNG
    static std::default_random_engine dre{std::random_device()()};
    return dre;
}
