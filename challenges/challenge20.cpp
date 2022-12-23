#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <unordered_map>

#include "input.h"

// will not ever deallocate due to circular reference, that's okay for AoC
struct Node {
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    long value = 0;
    int index = 0;
};


std::shared_ptr<Node> findValue(std::shared_ptr<Node> node, int value){
    assert(node);
    while(node->value != value) {
        node = node->right;
    }
    return node;
}

long getValue(std::shared_ptr<Node> node, size_t value) {
    for(size_t index = 0; index < value; ++index) {
        node = node->right;
    }
    return node->value;
}

long getGroveCoordinates(std::span<long> numbers, long multiplier, unsigned int times) {

    std::unordered_map<size_t, std::shared_ptr<Node>> indexToNodeLookup;
    std::shared_ptr<Node> head = std::make_shared<Node>();
    auto left = head;
    bool firstNumber = true;
    for(size_t index=0; index < numbers.size(); ++index){
        auto number = numbers[index];
        if(firstNumber){
            firstNumber = false;
            left->value = number * multiplier; 
        }
        else {
            left->right = std::make_shared<Node>(left, nullptr, number*multiplier);
            left = left->right;
        }
        left->index = index;
        indexToNodeLookup[index] = left;
    }
    left->right = head;
    head->left = left;

    for(size_t time = 0; time < times; ++time){
        for(size_t index = 0; index < numbers.size(); ++index){
            auto number = numbers[index] * multiplier;
            auto node = indexToNodeLookup[index];
            auto shifts = number % static_cast<int>(numbers.size() - 1);
            for(unsigned int shift = 0; shift < abs(shifts); ++shift){
                if(number < 0){
                    std::swap(node->left->value, node->value);
                    std::swap(node->left->index, node->index);
                    std::swap(indexToNodeLookup[index], indexToNodeLookup[node->index]);
                    node = node->left;
                }
                if(number > 0){
                    std::swap(node->right->value, node->value);
                    std::swap(node->right->index, node->index);
                    std::swap(indexToNodeLookup[index], indexToNodeLookup[node->index]);
                    node = node->right;
                }
            }
        }
    }

    auto foundZero = findValue(head, 0);
    return getValue(foundZero, 1000 % numbers.size() ) + getValue(foundZero,2000 % numbers.size()) + getValue(foundZero, 3000 % numbers.size());
}

int main() {
    auto numbers = input::readLines<long>("input/input20.txt");
    std::cout << "Grove Coordinates: " << getGroveCoordinates(numbers, 1, 1) << "\n";
    std::cout << "Grove Coordinates: " << getGroveCoordinates(numbers, 811589153, 10) << "\n";
}