#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <span>
#include <tuple>

#include "input.h"

struct Shape {
    std::array<unsigned int, 4> fromBottom;
    std::array<unsigned int, 4> fromLeft;
    std::array<unsigned int, 4> fromRight;
    std::array<unsigned int, 4> height;
    unsigned int maxWidth;
};

const Shape line = {
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1},
    4
};

const Shape tee = {
    {2, 1, 2, 0},
    {2, 1, 2, 0},
    {2, 1, 2, 0},
    {1, 3, 1, 0},
    3
};

const Shape ell = {
    {1, 1, 1, 0},
    {1, 3, 3, 0},
    {1, 1, 1, 0},
    {1, 1, 3, 0},
    3
};

const Shape eye = {
    {1, 0, 0, 0},
    {1, 1, 1, 1},
    {1, 1, 1, 1},
    {4, 0, 0, 0},
    1
};

const Shape oh = {
    {1, 1, 0, 0},
    {1, 1, 0, 0},
    {1, 1, 0, 0},
    {2, 2, 0, 0},
    2
};

const std::array<Shape, 5> shapes{ line, tee, ell, eye, oh};

enum class Direction {
    Left,
    Right
};

auto shiftLeftIfAble(auto leftEdge, auto bottomEdge, const auto& shape, const auto& spaces){
    // above the ground
    bool shouldMove = false;
    if(bottomEdge >= spaces.size() && leftEdge > 0){
        shouldMove = true;
    }
    // check if 
    else if(leftEdge > 0) {
        // make sure we can move into the left
        shouldMove = true;
        // if any areas are blocked
        for(unsigned int row = 0; row < 4; ++row){
            if(shape.fromLeft[row] > 0 && bottomEdge + row < spaces.size()){
                assert(!spaces[bottomEdge + row][leftEdge + (shape.fromLeft[row] - 1)]);
                auto newLeft = leftEdge + (shape.fromLeft[row] - 1) - 1;
                if(spaces[bottomEdge + row][newLeft]){
                    shouldMove = false;
                    break;
                }
            }
        }
    }

    return shouldMove ? (leftEdge - 1) : leftEdge;
}

auto shiftRightIfAble(auto leftEdge, auto bottomEdge, const auto& shape, const auto& spaces){
    // above the ground
    auto rightEdge = leftEdge+shape.maxWidth - 1;
    bool shouldMove = false;
    if(bottomEdge >= spaces.size() && rightEdge < 6){
        shouldMove = true;
    }
    else if(rightEdge < 6) {
        // make sure we can move into the right
        shouldMove = true;
        // if any areas are blocked
        for(unsigned int row = 0; row < 4; ++row){
            if(shape.fromRight[row] > 0 && bottomEdge + row < spaces.size()){
                assert(!spaces[bottomEdge + row][rightEdge - (shape.fromRight[row] - 1)]);
                auto newRight = rightEdge - (shape.fromRight[row] - 1) + 1;
                if(spaces[bottomEdge + row][newRight]){
                    shouldMove = false;
                    break;
                }
            }
        }
    }
    return shouldMove ? (leftEdge + 1) : leftEdge;
}

auto shiftDownIfAble(auto leftEdge, auto bottomEdge, const auto& shape, const auto& spaces){
    //process the fall down
    bool shouldMove = bottomEdge > spaces.size();
    if(bottomEdge <= spaces.size() ) {
        shouldMove = true;
        // if any areas are blocked
        for(unsigned int column = 0; column < 4; ++column){
            if(shape.fromBottom[column] > 0){
                auto currentBottom = bottomEdge + (shape.fromBottom[column] - 1);
                if(currentBottom <= spaces.size()){
                    assert(currentBottom == spaces.size() || !spaces[currentBottom][leftEdge+column]);
                    auto newBottom = currentBottom - 1;
                    if(spaces[newBottom][leftEdge+column]){
                        shouldMove = false;
                        break;
                    }
                }
           }
        }
    }

    return shouldMove ? (bottomEdge - 1) : bottomEdge;
}

std::array<unsigned int, 7> getContours(const auto& spaces) {
    std::array<unsigned int, 7> out = {0,0,0,0,0,0,0};
    std::set<unsigned int> numbersToCheck = {0,1,2,3,4,5,6};
    auto iter = spaces.rbegin();
    unsigned int depth = 0;
    while(!numbersToCheck.empty()){
        std::vector<unsigned int> toDelete;
        for(auto num: numbersToCheck){
            if((*iter)[num]) {
               out[num] = depth;
               toDelete.push_back(num);
            }
        }
        for(auto num: toDelete){
            numbersToCheck.erase(num);
        }
        ++depth;
        ++iter;
    }
    return out;
}

void placeShape(auto leftEdge, auto bottomEdge, const auto& shape, auto& spaces){
    // put it in shapes
    for(unsigned int column = 0; column < 4; ++column){
        if(shape.fromBottom[column] > 0) {
            auto currentBottom = bottomEdge + (shape.fromBottom[column] - 1);
            for(unsigned space = currentBottom; space < currentBottom + shape.height[column]; ++space){
                while(space >= spaces.size()){
                    spaces.push_back({false, false, false, false, false, false, false});
                }
                assert(!spaces[space][leftEdge + column]);
                spaces[space][leftEdge + column] = true;
            }
        }
    }
}

unsigned long getHeightAfterTurns(const std::vector<Direction>& directions, unsigned long turns){

    unsigned long offsetHeight = 0UL;
    auto shapeIter = shapes.begin();
    auto directionIter = directions.begin();
    std::vector<std::array<bool, 7>> spaces { {true, true, true, true, true, true, true} };
    std::map<std::tuple<Shape*, const Direction*, std::array<unsigned int, 7>>, std::pair<unsigned long, unsigned long>> seen;
    for(unsigned long turn = 0; turn < turns; ++turn){
        auto shape = *shapeIter++;

        auto bottomEdge = spaces.size() + 3;
        auto leftEdge = 2;

        bool shapeStillFalling = true;
        while(shapeStillFalling){
            auto direction = *directionIter++;
            if(directionIter == directions.end()){
                directionIter = directions.begin();
            }
            if(direction == Direction::Left) {
                leftEdge = shiftLeftIfAble(leftEdge, bottomEdge, shape, spaces);
            } 
            if(direction == Direction::Right) {
                leftEdge = shiftRightIfAble(leftEdge, bottomEdge, shape, spaces); 
            }

            auto newBottom = shiftDownIfAble(leftEdge, bottomEdge, shape, spaces);
            if(newBottom != bottomEdge){
                bottomEdge = newBottom;
            }
            else {
                shapeStillFalling = false;
                placeShape(leftEdge, bottomEdge, shape, spaces);
            }
        }
        auto seenKey = std::make_tuple(&shape, &(*directionIter), getContours(spaces));
        auto match = seen.find(seenKey);
        if(match != seen.end()){
            auto difference = turn - match->second.first;
            auto differenceOfHeight = (spaces.size() - 1) - match->second.second;
            auto numberOfRounds = (turns - difference) / difference;
            turn += numberOfRounds * difference;
            offsetHeight = numberOfRounds * differenceOfHeight; 
            
            seen.clear();
        }

        seen[seenKey] = {turn, spaces.size() - 1}; 

        if(shapeIter == shapes.end()){
            shapeIter = shapes.begin();
        }

    } 
    return offsetHeight + spaces.size() - 1;
}

int main() {

    std::vector<std::array<bool, 7>> spaces{ 
        {true, true, true, true, true, true, true},
        {false, false, false, true, true, true, true},
    };

    auto directions = input::readSingleLine("input/input17.txt") | std::views::transform([](char c){
        assert(c == '<' || c == '>');
        return (c == '<') ? Direction::Left : Direction::Right;
    });

    std::cout << getHeightAfterTurns(std::vector<Direction>{directions.begin(), directions.end()}, 2022) << "\n";
    std::cout << getHeightAfterTurns(std::vector<Direction>{directions.begin(), directions.end()}, 1'000'000'000'000) << "\n";
    return 0;
}