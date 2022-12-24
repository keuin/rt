//
// Created by Keuin on 2022/4/11.
//

#ifndef RT_TIMER_H
#define RT_TIMER_H

#include <iostream>
#include <chrono>

class timer {
private:
    decltype(std::chrono::system_clock::now()) start_time;
    decltype(std::chrono::system_clock::now()) end_time;
    bool silent;
public:
    timer() : silent{false} {}

    explicit timer(bool silent) : silent{silent} {}

    void start_measure() {
        if (!silent) {
            fprintf(stderr, "Start timing...\n");
        }
        start_time = std::chrono::system_clock::now();
    }
    void stop_measure() {
        end_time = std::chrono::system_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        const auto secs = 1e-3 * duration.count() * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
        if (!silent) {
            fprintf(stderr, "Stop timing. Duration: %fs\n", secs);
        }
    }

    // Get seconds elapsed.
    std::chrono::duration<double> duration() {
        return end_time - start_time;
    }
};

#endif //RT_TIMER_H
