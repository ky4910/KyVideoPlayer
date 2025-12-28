#ifndef PLAYCONTEXT_H
#define PLAYCONTEXT_H

#pragma once

#include <atomic>

struct AVClock
{
    void reset() { pts.store(0.0); }
    void set(double v) { pts.store(v, std::memory_order_release); }
    double get() const { return pts.load(std::memory_order_acquire); }

private:
    std::atomic<double> pts{0.0};
};

struct PlayContext
{
    AVClock clock;
    std::atomic<bool> running{false};
    std::atomic<bool> paused{false};
    double speed = 1.0;
};

#endif // PLAYCONTEXT_H