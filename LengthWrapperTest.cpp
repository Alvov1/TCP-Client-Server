#include <iostream>
#include <bitset>
#include "LengthWrapper.h"

int main() {
    srand(100000);
    for(auto i = 0; i < 200; ++i) {
        auto num = (((unsigned) rand() + 256) * 256) % 16777216;
        LengthWrapper f(num);
        std::string t;

        for(auto j = 1; j < f.Length(); ++j)
            t += std::bitset<8>(*(f.data() + j)).to_string();

        if(t != std::bitset<24>(num).to_string())
            std::cout << "Incorrect bitness for " << num << std::endl;
        else
            std::cout << "OK for " << num << std::endl;

        if(num != f.value())
            std::cout << "Incorrect decimal for " << num << std::endl;
    }
    std::cout << "All is ok";
}
