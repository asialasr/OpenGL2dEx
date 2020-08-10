#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <cassert>

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


#endif // LOGGING_H