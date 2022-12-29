#include <cassert>
#include <ranges>
#include <iostream>
#include <istream>
#include <numeric>
#include <ranges>

#include "input.h"


std::string toSnafuNumber(int64_t number) {
    std::string out = "";
    while(number != 0) {
        auto remainder = number % 5;
        if(remainder < 3){
            out += std::to_string(remainder);
            number -= remainder;
        }
        else if(remainder == 3) {
            out += "=";
            number += 2;
        }
        else {
            out += "-";
            number += 1;
        }
        number /= 5LL;
    }
    return std::string(out.rbegin(), out.rend());
}

int64_t fromSnafuNumber(const std::string& str) {
    int64_t sum = 0U;
    int64_t factor = 1;
    for(char c: std::views::reverse(str)) {
        switch(c) {
            case '=': sum -= 2LL*factor; break;
            case '-': sum -= factor; break;
            case '1': sum += factor; break;
            case '2': sum += 2LL*factor; break;
            default: break;
        }
        factor *= 5;
    }
    assert(toSnafuNumber(sum) == str);
    assert(sum > 0);
    return sum;
}

int main() {
    auto numbers = input::readLines("input/input25.txt") | std::views::transform(fromSnafuNumber);
    int64_t total = std::accumulate(numbers.begin(), numbers.end(), 0LL);
    std::cout <<  fromSnafuNumber("1=10-0001=--==2") << " " << total << " " << toSnafuNumber(total) << "\n";
}
