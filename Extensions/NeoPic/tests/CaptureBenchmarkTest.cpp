#include "../CaptureBenchmark.h"
#include <cassert>
#include <cmath>
#include <iostream>

static bool Near(double a, double b) { return std::abs(a - b) < 1e-9; }

int main() {
    using namespace neopic_capture;
    assert(Summarize({}).count == 0);
    const auto single = Summarize({7});
    assert(single.count == 1 && single.mean == 7 && single.minimum == 7
        && single.median == 7 && single.p95 == 7 && single.maximum == 7);
    const auto even = Summarize({4, 1, 3, 2});
    assert(even.count == 4 && even.mean == 2.5 && even.median == 2.5
        && even.minimum == 1 && even.maximum == 4 && even.p95 == 4);
    assert(Summarize({5, 1, 3}).median == 3);
    std::vector<double> twenty;
    for (int i = 20; i > 0; --i) twenty.push_back(i);
    assert(Summarize(twenty).p95 == 19); // nearest-rank boundary

    std::int64_t ticks = 0;
    std::size_t calls = 0;
    const int durations[] = {100, 50, 1, 1000, 3, 5};
    auto capture = [&]() { ticks += durations[calls]; return calls++ != 3; };
    auto clock = [&]() { return ticks; };
    const auto mixed = RunBenchmark(4, 2, capture, clock, 1000);
    assert(calls == 6 && mixed.warmupSuccess == 2 && mixed.warmupFailure == 0);
    assert(mixed.firstSuccess && mixed.firstMs == 100 && mixed.totalMs == 1009);
    assert(mixed.success.count == 3 && mixed.success.mean == 3
        && mixed.success.minimum == 1 && mixed.success.median == 3
        && mixed.success.p95 == 5 && mixed.success.maximum == 5);
    assert(mixed.failure.count == 1 && mixed.failure.mean == 1000 && !mixed.noReadySource);

    calls = 0;
    auto fail = [&]() { ++calls; ticks += 2; return false; };
    const auto unavailable = RunBenchmark(100, 3, fail, clock, 1000);
    assert(calls == 3 && unavailable.noReadySource && unavailable.warmupFailure == 3
        && unavailable.success.count == 0 && unavailable.failure.count == 0
        && unavailable.totalMs == 0 && unavailable.firstMs == 2 && !unavailable.firstSuccess);
    const auto failed = RunBenchmark(4, 0, fail, clock, 1000);
    assert(calls == 7 && !failed.noReadySource && failed.success.count == 0
        && failed.failure.count == 4 && failed.totalMs == 8 && failed.firstMs == 2);

    calls = 0;
    auto success = [&]() { ++calls; ticks += 1; return true; };
    const auto noWarmup = RunBenchmark(2, 0, success, clock, 2000);
    assert(calls == 2 && noWarmup.firstSuccess && Near(noWarmup.firstMs, 0.5)
        && Near(noWarmup.totalMs, 1) && noWarmup.success.count == 2);
    for (int which = 0; which < 3; ++which) {
        bool rejected = false;
        try { RunBenchmark(which == 0 ? 0 : 1, 0, success, clock, which == 0 ? 1000 : 1 - which); }
        catch (const std::invalid_argument&) { rejected = true; }
        assert(rejected && calls == 2);
    }
    std::cout << "PASS: benchmark warmup, timing, percentiles, failures and unavailable source\n";
}
