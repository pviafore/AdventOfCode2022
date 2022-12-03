#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <regex>
#include <span>
#include <utility>

#include "input.h"

using Range = std::pair<unsigned int, unsigned int>;

class RangePair {
public:
    friend std::istream& operator>>(std::istream& stream, RangePair& rangePair){
        std::string text;
        stream >> text;

        const std::regex rangePairRegex { "(\\d+)-(\\d+),(\\d+)-(\\d+)"};
        std::smatch match;
        auto result = std::regex_search(text, match, rangePairRegex);
        assert(result);

        rangePair.elf1 = Range{std::stoi(match[1]), std::stoi(match[2])};
        rangePair.elf2 = Range{std::stoi(match[3]), std::stoi(match[4])};

        return stream;
    }

    bool doesOnePairContainOther() const { 
        auto [firstElf,secondElf] = ((elf1.first < elf2.first) || (elf1.first == elf2.first && elf1.second > elf2.second)) ? std::pair{elf1, elf2} : std::pair{elf2, elf1};
        return secondElf.first >= firstElf.first && secondElf.second <= firstElf.second;
    }

    bool doesOverlap() const { 
        auto [firstElf,secondElf] = ((elf1.first < elf2.first) || (elf1.first == elf2.first && elf1.second > elf2.second)) ? std::pair{elf1, elf2} : std::pair{elf2, elf1};
        return firstElf.second >= secondElf.first;
    }
private:
    Range elf1, elf2;
};

unsigned int getFullyContainedPairs(std::span<RangePair> rangePairs){
    return std::ranges::count_if(rangePairs, [](const RangePair& pair){ return pair.doesOnePairContainOther(); }); 
}

unsigned int getOverlappingPairs(std::span<RangePair> rangePairs){
    return std::ranges::count_if(rangePairs, [](const RangePair& pair){ return pair.doesOverlap(); }); 
}


int main(){
    auto rangePairs = input::readLines<RangePair>("input/input4.txt");
    std::cout << "Fully contained pairs: " << getFullyContainedPairs(rangePairs) << "\n";
    std::cout << "Overlapping pairs: " << getOverlappingPairs(rangePairs) << "\n";
    return 0;
}