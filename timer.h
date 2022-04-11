//
// Created by Keuin on 2022/4/11.
//

#ifndef RT_TIMER_H
#define RT_TIMER_H

#include <iostream>
#include <chrono>

class timer {
private:
    typeof(std::chrono::system_clock::now()) start_time;
public:
    void start_measure() {
        fprintf(stderr, "Start timing...\n");
        start_time = std::chrono::system_clock::now();
    }
    void stop_measure() {
        const auto end = std::chrono::system_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_time);
        const auto secs = 1e-3 * duration.count() * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
        fprintf(stderr, "Stop timing. Duration: %fs\n", secs);
    }
};

#endif //RT_TIMER_H
