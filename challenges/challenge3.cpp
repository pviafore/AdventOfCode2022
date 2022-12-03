#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <numeric>
#include <set>
#include <span>
#include <string>

#include "input.h"

unsigned int getPriority(char item) {
    // getting priority a-z = 1-26 or A-Z = 27 - 52
    return islower(item) != 0 ? item - 96 : item - 64 + 26;
}

class Group {
public:
    friend std::istream& operator>>(std::istream& stream, Group& group){
        stream >> group.bag1 >> group.bag2 >> group.bag3;
        // sort so we can do set operations on them
        std::ranges::sort(group.bag1);
        std::ranges::sort(group.bag2);
        std::ranges::sort(group.bag3);
        return stream;
    }

    unsigned int getPriority() const {
        std::set<char> firstMatch;
        std::ranges::set_intersection(bag1, bag2, std::inserter(firstMatch, firstMatch.begin()));
        assert(!firstMatch.empty());

        std::set<char> commonLetter;
        std::ranges::set_intersection(firstMatch, bag3, std::inserter(commonLetter, commonLetter.begin()));
        assert(commonLetter.size() == 1);

        return ::getPriority(*commonLetter.begin());
    }


private:
    std::string bag1, bag2, bag3;
};

unsigned int getRucksackPriority(const std::string& text) {
    assert(text.length() <= INT32_MAX);
    const auto midpoint = static_cast<int32_t>(text.length() / 2);
    auto match = std::find_first_of(text.begin(), text.begin()+midpoint, text.begin()+midpoint, text.end());
    assert(match != text.end());
    return getPriority(*match);
}

unsigned int getTotalPriority(std::span<std::string> rucksacks) {
    return std::accumulate(rucksacks.begin(), rucksacks.end(), 0U, [](unsigned int sum, const std::string& text){
        return sum + getRucksackPriority(text);
    });
}

unsigned int getTotalGroupPriority(std::span<Group> groups) {
    return std::accumulate(groups.begin(), groups.end(), 0U, [](unsigned int sum, const Group& group){
        return sum + group.getPriority();
    });
}


int main(){ 
    auto rucksacks = input::readLines("input/input3.txt");
    std::cout << "Total priority is: " << getTotalPriority(rucksacks) << "\n";

    auto groups = input::readLines<Group>("input/input3.txt");
    std::cout << "Total group priority is: " << getTotalGroupPriority(groups) << "\n";

}