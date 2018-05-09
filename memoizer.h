#pragma once
#include <map>
#include <tuple>
#include <type_traits>
#include <boost/optional.hpp>

/**
 * \ingroup Evolve
 *
 * This file provides a simple unbounded memoizing cache. It is completely generic
 * and can be used to memoize any function.
 *
 * The cache uses a std::map to store the computed results. The key_type is a tuple
 * of the argument types of the function and the mapped_type is the type of the result
 * computed by the memozied function
 *
 * See the score() functions in knights_tour.h and nqueens.h to see usage examples
 */
namespace Memoizer {

template<typename F, typename... Args>
struct Cache {
    using key_t = std::tuple<std::decay_t<Args>...>;
    using val_t = std::result_of_t<F&& (Args&&...)>;
    std::map<key_t,val_t> cache_;

    void store(val_t v, Args... args) {
        cache_[key_t{args...}] = v;
    }

    boost::optional<val_t> lookup(Args... args) const {
        boost::optional<val_t> result = boost::none;
        auto itr = cache_.find({args...});
        if(itr != cache_.end()) {
            result = itr->second;
        }
        return result;
    }
};

template<typename CacheT, typename CallableT>
struct Memoizer{
    mutable CacheT cache_;
    CallableT fn_;

    Memoizer(const CallableT& callable) :
        fn_{callable}
    {}

    template<typename... Args>
    auto operator()(Args... args) const {
        auto res = cache_.lookup(args...);
        if(!res) {
            res = fn_(args...);
            cache_.store(*res,args...);
        }
        return *res;
    }
};

}
