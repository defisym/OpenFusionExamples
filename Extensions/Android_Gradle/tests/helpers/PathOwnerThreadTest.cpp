#include "GeneralDefinition.h"
#include <thread>
#include <atomic>
#include <iostream>

#ifndef _WIN32
// The same expansion site must produce a fresh owner for every argument and thread.
static auto makePath(const std::string& suffix) { return RELATIVE_PATH(suffix); }
int main() {
    constexpr int count = 8;
    constexpr int iterations = 200;
    std::atomic<int> ready{0};
    std::atomic<bool> go{false};
    std::atomic<bool> valid{true};
    std::vector<std::thread> workers;
    for (int t = 0; t < count; ++t) {
        workers.emplace_back([&,t] {
            ++ready;
            while (!go.load()) { std::this_thread::yield(); }
            for (int i = 0; i < iterations; ++i) {
                const std::string suffix = "worker-" + std::to_string(t) + "-" + std::to_string(i);
                auto owner = makePath(suffix);
                const std::string original(owner);
                if (!original.ends_with(suffix)) { valid=false; }
                auto copy = owner;
                char* buffer = copy;
                buffer[copy.size()-1] = '!';
                if (std::string(owner)!=original || std::string(copy)==original) { valid=false; }
                auto next = makePath("next");
                if (std::string(owner)!=original || !std::string(next).ends_with("next")) { valid=false; }
            }
        });
    }
    while (ready.load() != count) { std::this_thread::yield(); }
    go=true;
    for (auto& worker : workers) { worker.join(); }
    if (!valid.load()) { return 1; }
    std::cout << "PASS: 8 threads / 1600 independent path owners and mutable copies\n";
}
#else
int main() { return 0; } // Windows literal macros are unchanged.
#endif
