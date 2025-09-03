#pragma once

#include <random>
#include <type_traits>

#include "SameTypeTraits.h"
#include "ArithmeticTraits.h"

template<typename T>
concept RandomEngineConcept = requires {
    std::is_same_v<T, std::default_random_engine>;
    std::is_same_v<T, std::knuth_b>;
    std::is_same_v<T, std::minstd_rand0>;
    std::is_same_v<T, std::minstd_rand>;
    std::is_same_v<T, std::mt19937>;
    std::is_same_v<T, std::mt19937_64>;
    std::is_same_v<T, std::ranlux24>;
    std::is_same_v<T, std::ranlux24_base>;
    std::is_same_v<T, std::ranlux48>;
    std::is_same_v<T, std::ranlux48_base>;
};

template<typename Distribute, typename Value>
concept RandomDistributeConcept = requires {
    requires ArithmeticConcept<Value>;
    // TODO: Add more distributions as needed
    std::is_same_v<Distribute, std::uniform_int_distribution<Value>>;
    std::is_same_v<Distribute, std::uniform_real_distribution<Value>>;
    std::is_same_v<Distribute, std::binomial_distribution<Value>>;
    std::is_same_v<Distribute, std::geometric_distribution<Value>>;
    std::is_same_v<Distribute, std::negative_binomial_distribution<Value>>;
};

template <ArithmeticConcept T,
    RandomEngineConcept Engine, RandomDistributeConcept<T> Distribute>
struct RandomBase {
    std::random_device rd;
    Engine eng{ rd() };
    Distribute dist;

    template<typename T, typename... Ts>
        requires SameTypeCVRef<T, Ts...> // allows e.g., int & const int
    RandomBase(T&& first, Ts&&... rest) {
        if constexpr (sizeof...(rest) == 0) {
            dist = Distribute(first);
        }
        else {
            dist = Distribute(first, rest...);
        }
    }

    T Generate() { return dist(eng); }
};