#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <istream>
#include <iostream>
#include <ranges>
#include <regex>
#include <vector>

#include "input.h"


class Monkey {
public:

    virtual ~Monkey() = default; 

    bool hasItems() const { 
        return !items.empty();
    }

    virtual std::pair<unsigned long, unsigned long> inspectAndThrow(){
        ++numberOfItemsInspected;
        auto item = items.front();
        items.pop_front();
        item = operation(item);
        item /= 3;
        auto to = (item % test == 0) ? ifTrue : ifFalse;
        return {item, to};
    }

    void addItem(unsigned long item) {
        items.push_back(item);
    }

    size_t getNumberOfInspections() const {
        return numberOfItemsInspected;
    }

    unsigned long getTest() const {
        return test;
    }

    friend std::istream& operator>>(std::istream& stream, Monkey& monkey){
        std:: string line;
        getline(stream, line); // Monkey #
   
        getline(stream, line); // Starting items: xx,yy,zz...
        auto itemsPos = line.find(": ");
        assert(itemsPos != std::string::npos);
        std::string startingItems{line.substr(itemsPos+2, std::string::npos)};
        std::ranges::transform(input::split(startingItems, ", "), std::back_inserter(monkey.items), [](const auto& s){ return std::stoul(s);});
        
        getline(stream, line); //  Operation: old +|* (old|\d)
        std::regex operationRegex { "\\s+Operation: new = old ([\\+|\\*]) (old|\\d+)"};
        std::smatch match;
        auto result = std::regex_search(line, match, operationRegex);
        assert(result);
        std::function<unsigned long(unsigned long, unsigned long)> func = std::plus<>();
        if (match[1] == "*") {
            func = std::multiplies<>();
        }

        if(match[2] == "old") {
            monkey.operation = [func](unsigned long item) { return func(item, item); };
        }
        else {
            monkey.operation = [func, operand=std::stoul(match[2])](unsigned long item){ return func(item, operand); };
        }
        
        getline(stream, line); //  Test: divisible by \d+
        std::regex testRegex { "\\s+Test: divisible by (\\d+)"};
        result = std::regex_search(line, match, testRegex);
        assert(result);
        monkey.test = std::stoul(match[1]);
        
        getline(stream, line); //    If true: throw to monkey \d+ 
        std::regex trueRegex { "\\s+If true: throw to monkey (\\d+)"};
        result = std::regex_search(line, match, trueRegex);
        assert(result);
        monkey.ifTrue = std::stoul(match[1]);

        getline(stream, line); //    If true: throw to monkey \d+ 
        std::regex falseRegex { "\\s+If false: throw to monkey (\\d+)"};
        result = std::regex_search(line, match, falseRegex);
        assert(result);
        monkey.ifFalse = std::stoul(match[1]);

        getline(stream, line); // empty line

        return stream;
    }
    
    std::function<unsigned long(unsigned long)> operation;
    unsigned int test = 0;
    size_t ifTrue = 0;
    size_t ifFalse = 0;
    std::deque<unsigned long long> items;

private:
    size_t numberOfItemsInspected = 0;
};

class AdvancedMonkey {

public:
    using Item = std::unordered_map<unsigned long, unsigned long>;
    AdvancedMonkey(const Monkey& monkey, std::vector<unsigned int> primes) : monkey(monkey){
        for(auto item: monkey.items) {
            Item primeOffsetPairing;
            for (auto prime: primes) {
                primeOffsetPairing[prime] = item%prime;
            }
            items.push_back(primeOffsetPairing);
        }
    }
    
    std::pair<Item, unsigned long> inspectAndThrow() {
        ++numberOfItemsInspected;

        auto item = items.front();
        items.pop_front();

        for(auto [prime, offset]: item){
            item[prime] = monkey.operation(offset) % prime;
        }
        auto to = (item[monkey.test] == 0) ? monkey.ifTrue : monkey.ifFalse;
        return {item, to};
    }
    
    void addItem(const Item& item){
        items.push_back(item);
    }

    bool hasItems() const { 
        return !items.empty();
    }

    size_t getNumberOfInspections() const {
        return numberOfItemsInspected;
    }

private:

    std::deque<Item> items;
    Monkey monkey;
    size_t numberOfItemsInspected = 0;
};

template<typename Monkeys>
unsigned long getTwoMostActiveMonkeyProduct(Monkeys monkeys, unsigned int numberOfRounds) {
    for(unsigned int round = 0; round < numberOfRounds; ++round) {
        //std::cout << "Round #" << round << "\n";
        for(auto& monkey: monkeys) {
            while(monkey.hasItems()) {
                auto [item, to] = monkey.inspectAndThrow();
                monkeys[to].addItem(item);
            }
        }
    }

    Monkeys monkeyCopy{monkeys.begin(), monkeys.end()};
    using MonkeyType = typename Monkeys::value_type;
    std::partial_sort(monkeyCopy.begin(), monkeyCopy.begin() + 2, monkeyCopy.end(), [](const MonkeyType& monkey1, const MonkeyType& monkey2){
        return monkey1.getNumberOfInspections() > monkey2.getNumberOfInspections();
    });
    return monkeyCopy[0].getNumberOfInspections() * monkeyCopy[1].getNumberOfInspections();
}

int main() { 
    auto monkeys = input::readLines<Monkey>("input/input11.txt");
    std::vector<Monkey> monkeyCopy(monkeys);

    auto primes = monkeys | std::views::transform([](const Monkey& monkey) { return monkey.getTest(); });
    
    std::cout << "Two most active monkey product: " << getTwoMostActiveMonkeyProduct(monkeys, 20) << "\n";
    
    std::vector<AdvancedMonkey> advancedMonkeys;
    std::transform(monkeyCopy.begin(), monkeyCopy.end(), std::back_inserter(advancedMonkeys), [&primes](const Monkey& monkey) {
        return AdvancedMonkey(monkey, {primes.begin(), primes.end()});
    });
    
    std::cout << "Two most active monkey product: " << getTwoMostActiveMonkeyProduct(advancedMonkeys, 10'000) << "\n";
    return 0;
}