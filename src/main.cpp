
#include <format>
#include <iostream>
#include <string>
#include <vector>

#define BQQ_CMV_DEBUG
#include "../include/cmv/cmv.hpp"

void run(u64 size, u64 maxRand) {
    std::cout << std::format("Data init, random range [0, {})\n", maxRand);
    std::vector<u64> data;
    data.reserve(size);

    std::mt19937 gen(std::random_device {}());
    std::uniform_int_distribution<u64> dist(0, maxRand);
    for (u64 i = 0; i < size; ++i) {
        data.push_back(dist(gen));
    }

    u64 elementCount = data.size();
    double epsilon   = 0.05;
    double delta     = 0.05;
    std::cout << "Data init done... Counting...\n";
    u64 result = approximateDistinct(data.cbegin(), data.cend(), data.size(),
                                     epsilon, delta);
    std::cout << std::format("Approximate number of distinct elements: {}\n",
                             result);

    std::cout << "Creating exact set...\n";
    std::set<u64> s(data.cbegin(), data.cend());
    std::cout << std::format("Actual number of distinct elements: {}\n",
                             s.size());
    auto diff = result > s.size() ? result - s.size() : s.size() - result;
    std::cout << std::format(
        "Diff: {}\nError: {}%\n\n", diff,
        (static_cast<double>(diff) / static_cast<double>(s.size())) * 100.0);
}

void runConstrained(u64 size, u64 maxRand, u64 constraint) {
    std::cout << std::format("Data init, random range [0, {})\n", maxRand);
    std::vector<u64> data;
    data.reserve(size);

    std::mt19937 gen(std::random_device {}());
    std::uniform_int_distribution<u64> dist(0, maxRand);
    for (u64 i = 0; i < size; ++i) {
        data.push_back(dist(gen));
    }

    u64 elementCount = data.size();
    std::cout << std::format(
        "Data init done... Counting with constrained memory to {} elements... "
        "\n",
        constraint);
    u64 result = approximateDistinctSpace(data.cbegin(), data.cend(),
                                          data.size(), constraint);
    std::cout << std::format("Approximate number of distinct elements: {}\n",
                             result);

    std::cout << "Creating exact set...\n";
    std::set<u64> s(data.cbegin(), data.cend());
    std::cout << std::format("Actual number of distinct elements: {}\n",
                             s.size());
    auto diff = result > s.size() ? result - s.size() : s.size() - result;
    std::cout << std::format(
        "Diff: {}\nError: {}%\n\n", diff,
        (static_cast<double>(diff) / static_cast<double>(s.size())) * 100.0);
}

int main() {
    run(1'000'000, 1'000'000);
    run(10'000'000, 1'000'000);
    runConstrained(1'000'000, 1'000'000, 1000);
    runConstrained(10'000'000, 10'000'000, 1000);
    runConstrained(1'000'000, 1'000'000, 10'000);
    runConstrained(10'000'000, 10'000'000, 10'000);
    runConstrained(10'000'000, 1, 10'000);
}