//
// Created by Keuin on 2022/4/16.
//

#ifndef RT_TRACELOG_H
#define RT_TRACELOG_H

// log ray traces to stderr
// Note: please do not enable antialiasing if you wish to get a readable log.
// These logs are simply printed to stderr. Using antialiasing will run
// multiple render thread at the same time, which will mess up the log.
//#define LOG_TRACE

#ifdef LOG_TRACE
#define TRACELOG(...) (fprintf(stderr, __VA_ARGS__))
#else
#define TRACELOG(...)
#endif

#endif //RT_TRACELOG_H
