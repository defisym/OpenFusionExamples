#pragma once

#include <atomic>

class SpinLock {
public:
    SpinLock() : flag(false) {}
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock) = delete;

    void lock() {
        bool expect = false;
        while (!flag.compare_exchange_weak(expect, true)) {
            expect = false;
        }
    }

    void unlock() {
        flag.store(false);
    }
private:
    std::atomic<bool> flag;
};

class SpinLocker {
public:
    SpinLocker(SpinLock& spinLock) :lock(spinLock) {
        lock.lock();
    }
    ~SpinLocker() {
        lock.unlock();
    }

    SpinLocker(const SpinLocker&) = delete;
    SpinLocker& operator=(const SpinLocker) = delete;
private:
    SpinLock& lock;
};