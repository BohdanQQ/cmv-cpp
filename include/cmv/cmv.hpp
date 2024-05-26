#include <algorithm>
#include <cmath>
#include <concepts>
#include <ctime>
#include <iterator>
#include <random>
#include <set>
#include <span>

#ifdef BQQ_CMV_DEBUG
#include <format>
#include <iostream>
#endif

#include "../util/defs.hpp"

/**
 * @brief Approximates the number of distinct elements in a range of values with
 * explicit memory constraint.
 *
 * @tparam It Iterator type of the range.
 * @param begin start of the range
 * @param end end of the range (exclusive)
 * @param elementCount number of elements in the range
 * @param maxBuffSize maximum number of distinct elements to store in a buffer
 * @param seed seed for the random number generator
 * @return the approximate number of distinct elements
 */
template <std::forward_iterator It>
u64 approximateDistinctSpace(const It& begin, const It& end, u64 elementCount,
                             u64 maxBuffSize,
                             unsigned int seed = std::random_device()()) {
    double p = 1;
    std::set<typename std::iterator_traits<It>::value_type> s;

    const u64 threshold = std::min(elementCount, maxBuffSize);

#ifdef BQQ_CMV_DEBUG
    std::cout << std::format("Really using: {}\n", threshold);
#endif
    auto nextRand = [gen    = std::mt19937(seed),
                     random = std::uniform_real_distribution<double>(
                         0, 1)]() mutable { return random(gen); };

    for (auto it = begin; it != end; ++it) {
        s.erase(*it);
        if (nextRand() < p) {
            s.insert(*it);
        }

        if (s.size() >= threshold) {
            std::erase_if(s, [&](auto& elem) { return nextRand() >= 1 / 2.f; });
            p /= 2;
        }
    }

#ifdef BQQ_CMV_DEBUG
    std::cout << std::format("Number of random erasures: {}\n", -std::log2(p));
#endif
    return static_cast<u64>(s.size() / p);
}

/**
 * @brief Calculates the spaced used by the approximate distinct algorithm.
 *
 * @note The worst-case space scales with 1 / epsilon^2 and is capped at
 * elementCount.
 */
u64 calculateThreshold(u64 elementCount, double epsilon, double delta) {
    epsilon = std::max(epsilon, 0.0001);
    delta   = std::max(delta, 0.0001);

    u64 threshold = std::ceil(12 / std::pow(epsilon, 2) *
                              std::log(8 * elementCount / delta));
    return std::min(elementCount, threshold);
}

/**
 * @brief Approximates the number of distinct elements in a range of values.
 *
 * @tparam It Iterator type of the range.
 * @param begin start of the range
 * @param end end of the range (exclusive)
 * @param elementCount number of elements in the range
 * @param epsilon bound to the approximation's value range, SPACE usage scales
 * with 1 / epsilon^2 in the worst-case!!! see note
 * @param delta bound to the approximation's error, see note
 * @param seed seed for the random number generator
 * @return the approximate number of distinct elements, see note
 *
 * @note Dennote C the return value of this alogrithm and D the actual number of
 * distinct elements. The probability of C being in the range [D * (1 -
 * epsilon), D * (1 + epsilon)] is at least (1 - delta).
 */
template <std::forward_iterator It>
u64 approximateDistinct(const It& begin, const It& end, u64 elementCount,
                        double epsilon = 0.05, double delta = 0.05,
                        unsigned int seed = std::random_device()()) {
    u64 threshold = calculateThreshold(elementCount, epsilon, delta);
#ifdef BQQ_CMV_DEBUG
    std::cout << std::format(
        "Calculated threshold given epsilon = {} and delta = {}: {}\n", epsilon,
        delta, threshold);
#endif
    return approximateDistinctSpace(begin, end, elementCount, threshold, seed);
}