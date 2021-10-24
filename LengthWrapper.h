#ifndef PROJECT_LENGTHWRAPPER_H
#define PROJECT_LENGTHWRAPPER_H

#include <iostream>
#include <exception>
#include <cstddef>

class LengthWrapper {
    unsigned char* bites;
    size_t ptrLen;

    long long decodeL() const;

public:
    LengthWrapper(long long length);
    LengthWrapper(unsigned char* ptr, size_t size);

    LengthWrapper& operator=(const LengthWrapper& copy);

    long long value() const;
    char* data() const;
    size_t Length() const;

    ~LengthWrapper();
};

template <typename T>
size_t biteLength(T value);
#endif //PROJECT_LENGTHWRAPPER_H