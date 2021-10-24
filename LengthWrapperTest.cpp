#include <iostream>
#include <bitset>
#include "LengthWrapper.h"

int main() {
//    LengthWrapper test(16000000);
//    std::string t;
//    for(auto i = 1; i < test.Length(); ++i)
//        t += std::bitset<8>(*(test.data() + i)).to_string();
//
//    if(t != std::bitset<24>(16000000).to_string())
//        std::cout << "Incorrect" << std::endl;
//
//    std::cout << t << "    - Mine" << std::endl;
//    std::cout << std::bitset<24>(16000000).to_string();

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