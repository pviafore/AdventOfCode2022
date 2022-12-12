#include <cassert>
#include <iostream>
#include <istream>
#include <map>
#include <numeric>
#include <span>
#include <variant>

#include "input.h"


struct Noop {};
struct Add { int delta; };

using Instruction = std::variant<Noop, Add>;

std::istream& operator>>(std::istream& stream, Instruction& instruction){
    std::string token;
    stream >> token;
    if(token == "noop"){
        instruction = Noop{};
    }
    else if (token == "addx"){
        Add add{};
        stream >> add.delta;
        instruction = add;
    }
    return stream;
}

std::vector<int> getSignalStrengthValues(std::span<Instruction> instructions){
     int xValue = 1;
    std::vector<int> valuesDuringCycle{1};
    for (const auto& instruction: instructions) {
        valuesDuringCycle.push_back(xValue);
        if(std::holds_alternative<Add>(instruction)){
            valuesDuringCycle.push_back(xValue);
            xValue += std::get<Add>(instruction).delta;
        }
    }
    return valuesDuringCycle;
}
size_t getSignalStrengthSum(std::span<int> valuesDuringCycle){
    auto cycles = {20, 60, 100, 140, 180, 220};
    return std::accumulate(cycles.begin(), cycles.end(), 0U, [&valuesDuringCycle](size_t sum, size_t cycle){
        return sum + cycle*valuesDuringCycle[cycle];
    });
}

void drawScreen(std::span<int> valuesDuringCycle){
    for(int tick=0; tick<=240; ++tick){
        if(tick % 40 == 0){
            std::cout << "\n";
        }
        auto index = tick%40;
        std::cout << ((valuesDuringCycle[tick] >= index-2 && valuesDuringCycle[tick] <= index  ) ? "#" : ".");

    }
    std::cout << "\n";
}

int main(){
    auto instructions = input::readLines<Instruction>("input/input10.txt");
    auto valuesDuringCycle = getSignalStrengthValues(instructions);
    std::cout << "Signal Strength Sum: " << getSignalStrengthSum(valuesDuringCycle) << "\n";
    drawScreen(valuesDuringCycle);

}