#include <algorithm>
#include <deque>
#include <iostream>
#include <ranges>
#include <set>
#include <string>
#include <string_view>

#include "input.h"

bool isUnique(const std::deque<char>& str){
    const std::set<char> set{ str.begin(), str.end()};
    return str.size() == set.size();

}

std::string::size_type getStartOfPacketPosition(std::string_view str, std::string::size_type num){
    std::deque<char> candidate{str.begin(), str.begin()+num};
    if (isUnique(candidate)){
        return 0;
    }
    const auto* charIter = str.begin() + num;
    for(; charIter != str.end() && !isUnique(candidate); ++charIter) {  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        candidate.pop_front();
        candidate.push_back(*charIter);
    }
    return std::distance(str.begin(), charIter);
}

int main() {
    auto str = input::readSingleLine("input/input6.txt");
    std::cout << "Start of packet " << getStartOfPacketPosition(str, 4) << "\n";
    std::cout << "Start of message " << getStartOfPacketPosition(str, 14) << "\n";
}
