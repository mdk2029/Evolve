#pragma once
#include <map>
#include <tuple>
#include <type_traits>
#include <boost/optional.hpp>

namespace Memoizer {

template<typename F, typename... Args>
struct Cache {
    using key_t = std::tuple<Args...>;
    using val_t = std::result_of_t<F&& (Args&&...)>;
    std::map<key_t,val_t> cache_;

    void store(val_t v, Args... args) {
        cache_[key_t{args...}] = v;
    }

    boost::optional<val_t> lookup(Args... args) const {
        boost::optional<val_t> result = boost::none;
        auto itr = cache_.find(key_t{args...});
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
