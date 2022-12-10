#include <cassert>
#include <istream>
#include <iostream>
#include <set>
#include <span>
#include <string>
#include <unordered_map>

#include "grid.h"
#include "input.h"

Grid::Direction toDirection(char directionChar){
    switch(directionChar){
        case 'U': return Grid::Direction::Up;
        case 'L': return Grid::Direction::Left;
        case 'R': return Grid::Direction::Right;
        case 'D': return Grid::Direction::Down;
        default: assert(false); return Grid::Direction::Up;
    }
}

struct Move {
    Grid::Direction direction = Grid::Direction::Up;
    unsigned long moves = 0;

};

std::istream& operator>>(std::istream& stream, Move& move) {
    char directionChar;
    std::string number;
    stream >> directionChar >> number;
    
    move.direction = toDirection(directionChar);
    move.moves = std::stoul(number);
    return stream;
}

Grid::Coord getMoveTowards(const Grid::Coord& origin, const Grid::Coord& destination) {
    assert(Grid::getManhattanDistance(origin, destination) <= 4);
    auto difference = origin - destination;
    
    std::map<Grid::Coord, Grid::Coord> moveLookup = {
        { {2,0}, {-1, 0}},
        { {-2,0}, {1, 0}},
        { {2,1}, {-1, -1}},
        { {2,2}, {-1, -1}},
        { {-2,-2}, {1, 1}},
        { {-2,2}, {1, -1}},
        { {2,-2}, {-1, 1}},
        { {-2,1}, {1, -1}},
        { {2,-1}, {-1, 1}},
        { {-2,-1}, {1, 1}},
        { {0,2}, {0, -1}},
        { {0,-2}, {0, 1}},
        { {-1,2}, {1, -1}},
        { {-1,-2}, {1, 1}},
        { {1,2}, {-1, -1}},
        { {1,-2}, {-1, 1}},
    };

    auto nextMove = moveLookup.find(difference);
    return nextMove != moveLookup.end() ? nextMove->second : Grid::Coord{0, 0};
}


size_t getPositionsVisitedByTail(std::span<Move> moves, size_t numberOfKnots){
    Grid::Coord initial { 0, 0 };
    std::vector<Grid::Coord> positions {numberOfKnots, initial};
    std::set<Grid::Coord> tailPositions{ initial };
    for(const auto& move: moves){
        for(unsigned int counter = 0; counter < move.moves; ++counter){
            positions[0] = positions[0] + move.direction;
            for (size_t knot = 1; knot < numberOfKnots; ++knot){
                positions[knot] = positions[knot] + getMoveTowards(positions[knot], positions[knot - 1]);
            }
            tailPositions.insert(positions[numberOfKnots - 1]);
        }
    }
    return tailPositions.size();
}

int main() {
    auto moves = input::readLines<Move>("input/input9.txt");
    std::cout << "Total Positions Visited By Tail: " << getPositionsVisitedByTail(moves, 2) << "\n";
    std::cout << "Total Positions Visited By Tail(10): " << getPositionsVisitedByTail(moves, 10) << "\n";

}

