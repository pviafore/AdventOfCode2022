#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <istream>
#include <numeric>
#include <ranges>
#include <span>
#include <unordered_map>
#include <variant>
#include <vector>

#include "input.h"

const std::unordered_map<char, std::function<long(long, long)>> operationLookup = {
    {'+', std::plus()},
    {'-', std::minus()},
    {'*', std::multiplies()},
    {'/', std::divides()}
};

struct Operation {
    char operation;
    std::string arg1;
    std::string arg2;

};

struct Fraction {
    long numerator = 0;
    long denominator = 1;

    explicit Fraction() = default;
    explicit Fraction(long numerator, long denominator = 1) {
        long divisor = std::gcd(numerator, denominator);
        this->numerator = (divisor == 1) ? numerator : numerator / divisor;
        this->denominator = (divisor == 1) ? denominator : denominator / divisor;
    } 

    friend Fraction operator+(const Fraction& f1, const Fraction& f2) {
        if(f1.denominator == f2.denominator) {
            return Fraction { f1.numerator + f2.numerator, f1.denominator};
        }
        else {
            return Fraction { f1.numerator * f2.denominator + f2.numerator * f1.denominator, f1.denominator * f2.denominator};
        }
    }

    friend Fraction operator-(const Fraction& f1, const Fraction& f2) {
        return f1 + Fraction{-1 * f2.numerator, f2.denominator};
    }

    friend Fraction operator*(const Fraction& f1, const Fraction& f2) {
        return Fraction { f1.numerator * f2.numerator, f1.denominator * f2.denominator};
    }
    
    friend Fraction operator/(const Fraction& f1, const Fraction& f2) {
        return Fraction { f1.numerator * f2.denominator, f1.denominator * f2.numerator};
    }
};

using OperationOrNumber = std::variant<Operation, long>;
using namespace std::literals;
struct Monkey {
    std::string name;
    OperationOrNumber opOrNumber;
    
    friend std::istream& operator>>(std::istream& stream, Monkey& monkey){
        std::string text;
        getline(stream, text);
        std::vector<std::string> words;
        std::ranges::transform(text | std::views::split(" "s),  std::back_inserter(words), [](const auto& s){
            return std::string{s.begin(), s.end()};
        });
        
        assert(words.size() >= 2);
        monkey.name = words[0].substr(0,4);
        if (words.size() == 2){
            monkey.opOrNumber = std::stol(words[1]);
        }
        else {
            assert(words.size() == 4);
            monkey.opOrNumber =  Operation{ words[2][0], words[1], words[3]};
        }
        return stream;
    }

};

long getValue(const std::string& name, const auto& monkeyLookup, auto& answerLookup) {
    if(answerLookup.find(name) == answerLookup.end()){
        auto& monkey = monkeyLookup.at(name);
        if(std::holds_alternative<long>(monkey.opOrNumber)) {
            answerLookup[name] = std::get<long>(monkey.opOrNumber);
        }
        else {
            Operation op = std::get<Operation>(monkey.opOrNumber);
            answerLookup[name] = operationLookup.at(op.operation)(getValue(op.arg1, monkeyLookup, answerLookup), getValue(op.arg2, monkeyLookup, answerLookup));
        }
    }
    return answerLookup[name];
}

long getRootYell(const std::unordered_map<std::string, Monkey>& monkeyLookup){
    std::unordered_map<std::string, long> answerLookup;

    return getValue("root", monkeyLookup, answerLookup);
}

using Coefficients = std::unordered_map<int, Fraction>;

Coefficients subtractCoefficients(const Coefficients& c1, const Coefficients& c2) {
    Coefficients out { c1.begin(), c1.end() };
    for(auto [exponent, term]: c2) {
        if (c1.find(exponent) == c1.end()) {
            out[exponent] = Fraction{-1} * term;
        }
        else {
            out[exponent] = out[exponent] - term;
        }
    }
    return out;
}

Coefficients addCoefficients(const Coefficients& c1, const Coefficients& c2) {
    Coefficients out { c1.begin(), c1.end() };
    for(auto [exponent, term]: c2) {
        if (c1.find(exponent) == c1.end()) {
            out[exponent] = term;
        }
        else {
            out[exponent] = out[exponent] + term;
        }
    }
    return out;
}

Coefficients multiplyCoefficients(const Coefficients& c1, const Coefficients& c2) {
    Coefficients out;
    for(auto [c1Exponent, c1Term]: c1) {
        for(auto [c2Exponent, c2Term]: c2) {
            auto newExponent = c1Exponent + c2Exponent;
            if(out.find(newExponent) == out.end()){
                out[newExponent] = c1Term * c2Term;
            }
            else {
                out[newExponent] = out[newExponent] + c1Term * c2Term;
            }
        }
    }
    return out;
}

Coefficients divideCoefficients(const Coefficients& c1, const Coefficients& c2) {
    Coefficients out;
    for(auto [c1Exponent, c1Term]: c1) {
        for(auto [c2Exponent, c2Term]: c2) {
            auto newExponent = c1Exponent - c2Exponent;
            if(out.find(newExponent) == out.end()){
                out[newExponent] = c1Term / c2Term;
            }
            else {
                out[newExponent] = out[newExponent] - c1Term / c2Term;
            }
        }
    }
    return out;
}

const std::unordered_map<char, std::function<Coefficients(const Coefficients&, const Coefficients&)>> coefficientOperationLookup = {
    {'+', addCoefficients},
    {'-', subtractCoefficients},
    {'*', multiplyCoefficients},
    {'/', divideCoefficients}
};

Coefficients resolve(const std::string& name, const auto& monkeyLookup, auto& answerLookup) {
    if(name == "humn") {
        return Coefficients { { 1, Fraction { 1 } } };
    }
    if(answerLookup.find(name) == answerLookup.end()){
        auto& monkey = monkeyLookup.at(name);
        if(std::holds_alternative<long>(monkey.opOrNumber)) {
            answerLookup[name] = Coefficients{ {0, Fraction { std::get<long>( monkey.opOrNumber  )}} };
        }
        else {
            Operation op = std::get<Operation>(monkey.opOrNumber);
            Coefficients c1 = resolve(op.arg1, monkeyLookup, answerLookup);
            Coefficients c2 = resolve(op.arg2, monkeyLookup, answerLookup);
            answerLookup[name] = coefficientOperationLookup.at(op.operation)(c1, c2);
        }
    }
    return answerLookup[name];
}

long getNumberNeededForRoot(const std::unordered_map<std::string, Monkey>& monkeyLookup) { 

    auto monkey = monkeyLookup.at("root");
    auto operation = std::get<Operation>(monkey.opOrNumber);
    std::unordered_map<std::string, Coefficients> answerLookup;
    auto coefficients = subtractCoefficients(resolve(operation.arg1, monkeyLookup, answerLookup), resolve(operation.arg2, monkeyLookup, answerLookup));
    assert(coefficients.size() == 2 && coefficients.find(1) != coefficients.end() && coefficients.find(0) != coefficients.end());

    auto fraction =  Fraction { -1 } * coefficients[0] / coefficients[1];
    assert(abs(fraction.denominator) == 1);
    return fraction.numerator / fraction.denominator;
}

int main(){
    std::unordered_map<std::string, Monkey> monkeyLookup;
    std::ranges::transform(input::readLines<Monkey>("input/input21.txt"), std::inserter(monkeyLookup, monkeyLookup.begin()), [](auto monkey){
        return std::pair(monkey.name, std::move(monkey));
    });
    std::cout << "Root Yell: " << getRootYell(monkeyLookup) << "\n";
    std::cout << "Number Needed: " << getNumberNeededForRoot(monkeyLookup) << "\n";
    return 0;

}