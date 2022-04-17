//
// Created by Keuin on 2022/4/16.
//

#ifndef RT_THREADING_H
#define RT_THREADING_H

#include <vector>
#include <thread>
#include <memory>
#include <deque>
#include <mutex>
#include <atomic>
#include <iostream>

// A simple once-usage thread pool and task queue.
// Using lock-free atomic counter to avoid expensive queue or synchronization mechanism.
// Tasks should be added into the queue before starting.
// Once the task queue is empty, threads quit.

// internal usage
template<typename T>
struct s_task {
    void (*f)(T &);

    T arg;
};

template<typename T>
class thread_pool {

    unsigned thread_count;
    std::vector<std::thread> workers;
    std::atomic<size_t> counter{0}; // index to the first available task in queue
    std::vector<s_task<T>> tasks;

    void worker_main();

public:
    explicit thread_pool(unsigned thread_count) : thread_count{thread_count} {
        std::cerr << "Using " << (counter.is_lock_free() ? "lock-free" : "locking") << " dispatcher." << std::endl;
    }

    // Thread unsafe!
    void submit_task(void (*f)(T &), T &&t);

    void start();

    // Wait the queue to become empty
    void wait();
};

template<typename T>
void thread_pool<T>::start() {
    if (workers.empty()) {
        for (typeof(thread_count) i = 0; i < thread_count; ++i) {
            workers.emplace_back(std::thread{&thread_pool<T>::worker_main, this});
        }
    } else {
        // TODO
    }
}

template<typename T>
void thread_pool<T>::worker_main() {
    const auto max_cnt = tasks.size();
    while (true) {
        const auto i = counter.fetch_add(1, std::memory_order_relaxed); // we only need atomicity
        if (i >= max_cnt) break; // all tasks are done
        auto &task = tasks[i];
        task.f(task.arg);
    }
}

// Do not submit after starting.
template<typename T>
void thread_pool<T>::submit_task(void (*f)(T &), T &&t) {
    tasks.push_back(s_task<T>{.f=f, .arg=std::move(t)});
}

template<typename T>
void thread_pool<T>::wait() {
    for (auto &th: workers) {
        th.join();
    }
}


#endif //RT_THREADING_H
