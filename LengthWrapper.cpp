#include "LengthWrapper.h"

LengthWrapper::LengthWrapper(long long length) {
    if(length < 0)
        throw std::invalid_argument("Length can not be negative.");

    if(length < 128) {
        bites = new unsigned char;
        *bites = length;
        ptrLen = 1;
    } else {
        ptrLen = biteLength(length) + 1;
        bites = new unsigned char[ptrLen];

        *bites = 0b10000000;
        *bites |= static_cast<unsigned char>(ptrLen - 1);

        for(auto i = ptrLen - 1; i > 0; --i) {
            bites[i] = length & 0b11111111;
            length = length >> 8;
        }
    }
}
LengthWrapper::LengthWrapper(unsigned char* ptr, size_t size) : bites(ptr), ptrLen(size){}

LengthWrapper& LengthWrapper::operator=(const LengthWrapper& copy) {
    if(this != &copy){
        ptrLen = copy.ptrLen;
        bites = new unsigned char[ptrLen];
        for (auto i = 0; i < ptrLen; ++i)
            bites[i] = copy.bites[i];
    }
    return *this;
}
LengthWrapper::~LengthWrapper() {
    delete [] bites;
}

long long LengthWrapper::value() const {
    if (ptrLen == 1)
        return *bites;  // Short
    else
        return decodeL();   // Long
}
long long LengthWrapper::decodeL() const {
    long long retVal = 0;
    for(auto i = 1; i < ptrLen; ++i) {
        retVal = retVal << 8;
        retVal += static_cast<unsigned char>(bites[i]);
    }
    return retVal;
}

template<typename T>
size_t biteLength(T value) {
    size_t ind = 0;
    for(int i = 1; i <= value; i *= 2, ++ind);
    return (ind + 7) / 8; // Round up division
}

char* LengthWrapper::data() const {
    return reinterpret_cast<char*>(bites);
}
size_t LengthWrapper::Length() const {
    return ptrLen;
}