#pragma once
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <vector>
#include <utility>

namespace neopic_capture {
struct TimingStats {
    std::size_t count = 0;
    double mean = 0, minimum = 0, median = 0, p95 = 0, maximum = 0;
};
inline TimingStats Summarize(std::vector<double> values) {
    TimingStats s;
    s.count = values.size();
    if (values.empty()) return s;
    std::sort(values.begin(), values.end());
    s.minimum = values.front(); s.maximum = values.back();
    s.mean = std::accumulate(values.begin(), values.end(), 0.0) / static_cast<double>(values.size());
    s.median = values.size() % 2 ? values[values.size() / 2]
        : (values[values.size() / 2 - 1] + values[values.size() / 2]) / 2;
    s.p95 = values[(values.size() * 95 + 99) / 100 - 1]; // nearest rank
    return s;
}
struct BenchmarkResult {
    TimingStats success, failure;
    std::size_t warmupSuccess = 0, warmupFailure = 0;
    double firstMs = 0, totalMs = 0;
    bool firstSuccess = false, noReadySource = false;
};

// The callback is the whole Capture + _TempCapture handoff. The clock supplies
// monotonic ticks; per-iteration output and file I/O belong outside this runner.
template<class Capture, class Clock>
BenchmarkResult RunBenchmark(std::size_t iterations, std::size_t warmup,
    Capture&& capture, Clock&& now, std::int64_t ticksPerSecond) {
    if (!iterations || ticksPerSecond <= 0) throw std::invalid_argument("benchmark count/timer");
    BenchmarkResult r;
    std::vector<double> good, bad;
    good.reserve(iterations); bad.reserve(iterations);
    auto ms = [ticksPerSecond](auto delta) { return static_cast<double>(delta) * 1000.0 / static_cast<double>(ticksPerSecond); };
    for (std::size_t i = 0; i < warmup; ++i) {
        const auto start = now();
        const bool ok = capture();
        const auto stop = now();
        if (i == 0) { r.firstMs = ms(stop - start); r.firstSuccess = ok; }
        if (ok) ++r.warmupSuccess; else ++r.warmupFailure;
    }
    // E_PENDING must not become an impressive-looking fast benchmark result.
    if (warmup && !r.warmupSuccess) { r.noReadySource = true; return r; }
    const auto batchStart = now();
    for (std::size_t i = 0; i < iterations; ++i) {
        const auto start = now();
        const bool ok = capture();
        const auto stop = now();
        const double elapsed = ms(stop - start);
        if (!warmup && i == 0) { r.firstMs = elapsed; r.firstSuccess = ok; }
        (ok ? good : bad).push_back(elapsed);
    }
    r.totalMs = ms(now() - batchStart);
    r.success = Summarize(std::move(good));
    r.failure = Summarize(std::move(bad));
    return r;
}
}
