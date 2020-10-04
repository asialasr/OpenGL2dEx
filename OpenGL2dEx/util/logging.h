#ifndef LOGGING_H
#define LOGGING_H

#include <cassert>

#define LOG_USE_FILE_IO
#ifdef LOG_USE_FILE_IO
#include <fstream>

// TODO(sasiala): better file logging implementation
static std::ofstream log_file_{ "log.txt" };

#define LOG(message) \
do {\
log_file_ << __FILE__ << ":" << __LINE__ << ": " << message << std::endl;\
} while (0)

#define ASSERT(bex, message) \
do {\
if (!(bex)) {\
log_file_ << "Assertion Failed: ";\
LOG(message);\
assert(false);\
}\
} while (0)

#else
#include <iostream>

#define LOG(message) \
do {\
std::cerr << __FILE__ << ":" << __LINE__ << ": " << message << std::endl;\
} while (0)

#define ASSERT(bex, message) \
do {\
if (!(bex)) {\
std::cerr << "Assertion Failed: ";\
LOG(message);\
assert(false);\
}\
} while (0)
#endif

#endif // LOGGING_H