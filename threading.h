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

template<typename T_Args, typename T_ImmuCtx, typename T_MutCtx>
using task_func_t = void (*)(size_t, T_Args &, const T_ImmuCtx &, T_MutCtx &);

// internal usage
template<typename T, typename U, typename V>
struct s_task {
    task_func_t<T, U, V> f;
    T arg;
};

template<typename T, typename U, typename V>
class thread_pool {

    unsigned thread_count;
    std::vector<std::thread> workers;
    std::atomic<size_t> counter{0}; // index to the first available task in queue
    std::vector<s_task<T, U, V>> tasks;
    const U &shared_ctx; // reference to immutable shared context
    V &mut_shared_ctx; // mutable shared context

    void worker_main();

public:
    explicit thread_pool(unsigned thread_count, const U &shared_ctx, V &mut_shared_ctx) :
            thread_count{thread_count}, shared_ctx{shared_ctx}, mut_shared_ctx{mut_shared_ctx} {
        std::cerr << "Using " << (counter.is_lock_free() ? "lock-free" : "locking") << " dispatcher." << std::endl;
    }

    // Thread unsafe!
    void submit_task(task_func_t<T, U, V> f, T &&t);

    void start();

    // Wait the queue to become empty
    void wait();
};

template<typename T, typename U, typename V>
void thread_pool<T, U, V>::start() {
    if (workers.empty()) {
        for (typeof(thread_count) i = 0; i < thread_count; ++i) {
            workers.emplace_back(std::thread{&thread_pool<T, U, V>::worker_main, this});
        }
    } else {
        // TODO
    }
}

template<typename T, typename U, typename V>
void thread_pool<T, U, V>::worker_main() {
    const auto max_cnt = tasks.size();
    while (true) {
        const auto i = counter.fetch_add(1, std::memory_order_relaxed); // we only need atomicity
        if (i >= max_cnt) break; // all tasks are done
        auto &task = tasks[i];
        task.f(i, task.arg, shared_ctx, mut_shared_ctx);
    }
}

// Do not submit after starting.
template<typename T, typename U, typename V>
void thread_pool<T, U, V>::submit_task(task_func_t<T, U, V> f, T &&t) {
    tasks.push_back(s_task<T, U, V>{.f=f, .arg=std::move(t)});
}

template<typename T, typename U, typename V>
void thread_pool<T, U, V>::wait() {
    for (auto &th: workers) {
        th.join();
    }
}


#endif //RT_THREADING_H
