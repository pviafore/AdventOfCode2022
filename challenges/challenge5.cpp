#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <regex>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
#include "input.h"

struct Move {
    uint64_t amount = 0;
    size_t from = 0;
    size_t to = 0;
};

Move toMove(const std::string& text) {
    const std::regex moveRegex { "move (\\d+) from (\\d+) to (\\d+)" };
    std::smatch match;

    if (std::regex_search(text, match, moveRegex)){
        return Move {
            std::stoul(match[1]),
            std::stoul(match[2]),
            std::stoul(match[3])
        };
    }
    assert(false);
    return Move{};
}

using Stack = std::vector<char>;
using Stacks = std::vector<Stack>;

Stacks createStacks(std::span<std::string> lines) {
    Stacks stacks {{}}; // empty spot in zero since stacks start at 1

    std::unordered_map<std::string::size_type, size_t> textIndexToStackMapping;
    auto numbers = *lines.begin();
    auto position = numbers.find_first_of("123456789");
    while(position != std::string::npos){
        // only supports up to 9 stacks
        textIndexToStackMapping[position] = stacks.size();
        stacks.emplace_back();
        position = (position >= numbers.length() - 1) ? std::string::npos : position + 1;
        position = numbers.find_first_of("1234567789", position);
    }

    std::for_each(lines.begin()+1, lines.end(), [&stacks, &textIndexToStackMapping](const auto& line){
        for(auto [textIndex, stack]: textIndexToStackMapping) {
            if(line[textIndex] != ' '){ 
                stacks[stack].push_back(line[textIndex]);
            }
        }
    });
    return stacks;
}

using Moves = std::vector<Move>;


std::pair<Stacks, Moves> parseInput(std::span<std::string> lines){
    auto separator = std::ranges::find(lines, "");
    assert(separator != lines.end() && separator > lines.begin());

    auto stacksText = std::vector<std::string>{std::make_reverse_iterator(separator), lines.rend()};
    const Stacks stacks = createStacks(stacksText);
    
    Moves moves;
    std::transform(separator+1, lines.end(), std::back_inserter(moves), toMove);

    return {stacks, moves};
}

Stacks applyMoves(const Moves& moves, Stacks stacks) {
    for(const auto& move: moves){
        for(unsigned int counter = 0; counter < move.amount; ++counter){
            stacks[move.to].push_back(stacks[move.from].back());
            stacks[move.from].pop_back();
        }
    }
    return stacks;
}

Stacks apply9001StyleMoves(const Moves& moves, Stacks stacks) {
    for(const auto& move: moves){
        auto& toStack = stacks[move.to];
        auto& from = stacks[move.from];
        toStack.insert(toStack.end(), from.end() - move.amount, from.end()); //NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
        from.erase(from.end() - move.amount, from.end()); //NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
    }
    return stacks;
}



void printTopOfStacks(const Stacks& stacks) {
    std::transform(stacks.begin() + 1, stacks.end(), std::ostream_iterator<char>(std::cout, ""), [](const auto& stack){
        return stack.back();
    });
    std::cout << "\n";
}

int main() {
    auto lines = input::readLines("input/input5.txt");
    auto [stacks, moves] = parseInput(lines);
    auto newStacks = applyMoves(moves, stacks);
    printTopOfStacks(newStacks);

    auto newStacks9001 = apply9001StyleMoves(moves, stacks);
    printTopOfStacks(newStacks9001);
    return 0;
}

