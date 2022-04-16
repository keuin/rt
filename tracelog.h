//
// Created by Keuin on 2022/4/16.
//

#ifndef RT_TRACELOG_H
#define RT_TRACELOG_H

// log ray traces to stderr
//#define LOG_TRACE

#ifdef LOG_TRACE
#define TRACELOG(...) (fprintf(stderr, __VA_ARGS__))
#else
#define TRACELOG(...)
#endif

#endif //RT_TRACELOG_H
