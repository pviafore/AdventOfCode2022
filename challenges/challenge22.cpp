#include <cassert>
#include <functional>
#include <iostream>
#include <istream>
#include <iterator>
#include <optional>
#include <vector>

#include "input.h"
#include "grid.h"

struct Instruction { 
    size_t steps;
    std::optional<Grid::Direction> direction;

    friend std::ostream& operator<<(std::ostream& stream, const Instruction& instruction){
        stream << instruction.steps;
        if(instruction.direction){
            stream << ((instruction.direction.value() == Grid::Direction::Left) ? "L" : "R");
        } 
        return stream;
    }
};

Grid::Direction toDirection(char c) {
    switch (c) {
        case 'L': return Grid::Direction::Left;
        case 'R': return Grid::Direction::Right;
        default: assert(false); return Grid::Direction::Up;
    }
}

std::istream& operator>>(std::istream& stream, std::vector<Instruction>& instructions) {
    std::string text;
    getline(stream, text);
    auto start = 0U;
    auto nextPos = text.find_first_of("RL");
    for(; nextPos != std::string::npos; nextPos = text.find_first_of("RL", nextPos + 1)){
        instructions.emplace_back(std::stoul(text.substr(start, nextPos - start)), toDirection(text[nextPos]) );
        start = nextPos + 1;
    }
    instructions.emplace_back(std::stoul(text.substr(start, (nextPos - start))), std::nullopt);
    return stream;
}

std::pair<Grid::Coord, Grid::Direction> getFinalPosition(const auto& grid, const auto& instructions) {
    auto position = Grid::Coord{0,0};
    auto direction = Grid::Direction::Right;
    while(grid[position] == ' ') {
        position = position + Grid::Direction::Right;
    }
    for(const Instruction& instruction: instructions){
        for(size_t step = 0; step < instruction.steps; ++step){

            auto nextPosition = position + direction;
            if (direction == Grid::Direction::Left && (position.xPos == 0 || grid[nextPosition] == ' ')){
                nextPosition.xPos = grid.getMaxXInRow(position.yPos);
            }
            if(direction == Grid::Direction::Right && (position.xPos == grid.getMaxXInRow(position.yPos) || grid[nextPosition] == ' ' )) {
                nextPosition.xPos = 0;
                while(grid[nextPosition] == ' '){
                    ++nextPosition.xPos;
                }
            }
            if(direction == Grid::Direction::Up && (position.yPos == 0 || grid[nextPosition] == ' ')){
                nextPosition.yPos = grid.getMaxYInColumn(position.xPos);
                while(grid[nextPosition] == ' '){
                    --nextPosition.yPos;
                }
            }
            if(direction == Grid::Direction::Down && (position.yPos == grid.getMaxYInColumn(position.xPos) || grid[nextPosition] == ' ')){
                nextPosition.yPos = 0;
                while(grid[nextPosition] == ' '){
                    ++nextPosition.yPos;
                }
            }
            if(grid[nextPosition] == '#') {
                break;
            }
            position = nextPosition;
        }
        direction = (instruction.direction) ?  Grid::turn(direction, instruction.direction.value()) : direction;
    }
    return std::pair(position, direction);
}

/*
  A B
  C
E D
F
    
    F

E   A   B
    
    C
    
    
    
    
    C
    
E   D   B

    F
    
    
    
A is right side up
A -> C is C top (down)
A -> B is B left (right)
A -> E is E left (left)
A -> F is F left (up)
B -> F is F bottom (up)
B -> C is C right (down)
B -> D is D right (right)
C -> D is D top ( down)
C -> E is E top (left)
E -> F is F top ( bottom )


*/

class Cube {

public:
    // assuming layout above
    explicit Cube(const Grid::Grid<char>& grid) {
        for(const auto& coordPair: grid) {
            auto [coord, value] = coordPair;
            if(coord.yPos < 50) {
                if (coord.xPos >= 50 && coord.xPos < 100){
                    auto xPos = coord.xPos - 50;
                    faceA[Grid::Coord{xPos, coord.yPos}] = value;
                }
                else if(coord.xPos >= 100){
                    auto xPos = coord.xPos - 100;
                    faceB[Grid::Coord{xPos, coord.yPos}] = value;
                }
            }
            if(coord.yPos >= 50 && coord.yPos < 100){
                if(coord.xPos >= 50 && coord.xPos < 100) {
                    faceC[Grid::Coord{coord.xPos - 50, coord.yPos - 50}] = value;
                }
            }
            if(coord.yPos >= 100 && coord.yPos < 150){
                auto yPos = coord.yPos - 100;
                if(coord.xPos < 50) {
                    faceE[Grid::Coord{coord.xPos, yPos}] = value;
                }
                else if(coord.xPos < 100) {
                    auto xPos = coord.xPos - 50;
                    faceD[Grid::Coord{xPos, yPos}] = value;
                }
            }
            if(coord.yPos >= 150 && coord.yPos < 200 && coord.xPos < 50) {
                auto yPos = coord.yPos - 150;
                faceF[Grid::Coord{coord.xPos, yPos}] = value;
            }
        }
        assert(faceA.size() == 2500 && faceB.size() == 2500 && faceC.size() == 2500 && faceD.size() == 2500 && faceE.size() == 2500 && faceF.size() == 2500);
        
        currentGrid = &faceA;
        position = Grid::Coord{0,0};
        direction = Grid::Direction::Right;
    }

    void makeMove(const Instruction& instruction) {
        for(size_t step = 0; step < instruction.steps; ++step){
            
            auto nextPosition = position + direction;
            Grid::Grid<char>* nextGrid = currentGrid;
            auto nextDirection = direction;
            if(direction == Grid::Direction::Left && position.xPos == 0) {
                if(currentGrid == &faceA){
                    nextGrid = &faceE;
                    nextDirection = Grid::Direction::Right;
                    nextPosition = Grid::Coord{0, 49-position.yPos};
                }
                else if(currentGrid == &faceB) {
                    nextGrid = &faceA;
                    nextPosition = Grid::Coord { 49, position.yPos};
                }
                else if(currentGrid == &faceC) {
                    nextGrid = &faceE;
                    nextDirection = Grid::Direction::Down;
                    nextPosition = Grid::Coord { position.yPos,0};
                }
                else if(currentGrid == &faceD) {
                    nextGrid = &faceE;
                    nextPosition = Grid::Coord { 49, position.yPos};
                }
                else if(currentGrid == &faceE) {
                    nextGrid = &faceA;
                    nextDirection = Grid::Direction::Right;
                    nextPosition = Grid::Coord { 0, 49 - position.yPos};
                }
                else {
                    assert(currentGrid == &faceF);
                    nextGrid = &faceA;
                    nextDirection = Grid::Direction::Down;
                    nextPosition = { position.yPos, 0};
                }
            }
            else if (direction == Grid::Direction::Right && position.xPos == 49){
                if(currentGrid == &faceA){
                    nextGrid = &faceB;
                    nextPosition = Grid::Coord{0, position.yPos};
                }
                else if(currentGrid == &faceB){
                    nextGrid = &faceD;
                    nextDirection = Grid::Direction::Left;
                    nextPosition = Grid::Coord{49, 49 - position.yPos};
                }
                else if(currentGrid == &faceC){
                    nextGrid = &faceB;
                    nextDirection = Grid::Direction::Up;
                    nextPosition = Grid::Coord{position.yPos, 49};
                }
                else if(currentGrid == &faceD) {
                    nextGrid = &faceB;
                    nextDirection = Grid::Direction::Left;
                    nextPosition = Grid::Coord { 49, 49 - position.yPos};
                }
                else if(currentGrid == &faceE) {
                    nextGrid = &faceD;
                    nextPosition = Grid::Coord { 0, position.yPos};
                }
                else {
                    assert(currentGrid == &faceF);
                    nextGrid = &faceD;
                    nextDirection = Grid::Direction::Up;
                    nextPosition = Grid::Coord{position.yPos, 49};
                }
            }
            else if(direction == Grid::Direction::Up && position.yPos == 0) { 
                if(currentGrid == &faceA){
                    nextGrid = &faceF;
                    nextDirection = Grid::Direction::Right;
                    nextPosition = Grid::Coord{0, position.xPos};
                }
                else if(currentGrid == &faceB){
                    nextGrid = &faceF;
                    nextPosition = Grid::Coord{position.xPos, 49};
                }
                else if(currentGrid == &faceC){
                    nextGrid = &faceA;
                    nextPosition = Grid::Coord{position.xPos, 49};
                }
                else if(currentGrid == &faceD){
                    nextGrid = &faceC;
                    nextPosition = Grid::Coord{position.xPos, 49};
                }
                else if(currentGrid == &faceE) {
                    nextGrid = &faceC;
                    nextDirection = Grid::Direction::Right;
                    nextPosition = Grid::Coord { 0, position.xPos};
                }
                else {
                    assert(currentGrid == &faceF);
                    nextGrid = &faceE;
                    nextPosition = Grid::Coord{position.xPos, 49};
                }

            }
            else if(direction == Grid::Direction::Down && position.yPos == 49) { 
                if(currentGrid == &faceA){
                    nextGrid = &faceC;
                    nextPosition = Grid::Coord{position.xPos, 0};
                }
                else if(currentGrid == &faceB){
                    nextGrid = &faceC;
                    nextDirection = Grid::Direction::Left;
                    nextPosition = Grid::Coord{49, position.xPos};
                }
                else if(currentGrid == &faceC){
                    nextGrid = &faceD;
                    nextPosition = Grid::Coord{position.xPos, 0};
                }
                else if(currentGrid == &faceD) {
                    nextGrid = &faceF;
                    nextDirection = Grid::Direction::Left;
                    nextPosition = Grid::Coord {49, position.xPos};
                }
                else if(currentGrid == &faceE) {
                    nextGrid = &faceF;
                    nextPosition = Grid::Coord {position.xPos, 0};
                }
                else {
                    assert(currentGrid == &faceF);
                    nextGrid = &faceB;
                    nextPosition = Grid::Coord{position.xPos, 0};
                }

            }

            assert(nextGrid->find(nextPosition) != nextGrid->end());

            if((*nextGrid)[nextPosition] == '#') {
                break;
            }
            currentGrid = nextGrid;
            direction = nextDirection;
            position = nextPosition;
            assert(currentGrid->find(position) != currentGrid->end() && (*currentGrid)[position] == '.');
        }
        direction = (instruction.direction) ? Grid::turn(direction, instruction.direction.value()) : direction;

    }


    std::pair<Grid::Coord, Grid::Direction> getUnwrappedPositionAndDirection() const{
        auto unwrappedPosition = position;

        if(currentGrid == &faceA){
            unwrappedPosition = Grid::Coord { position.xPos + 50, position.yPos};
        }
        else if(currentGrid == &faceB){
            unwrappedPosition = Grid::Coord{position.xPos + 100,  position.yPos};
        }
        else if(currentGrid == &faceC){
            unwrappedPosition = Grid::Coord{position.xPos + 50, position.yPos + 50};
        }
        else if(currentGrid == &faceD){
            unwrappedPosition = Grid::Coord{position.xPos + 50, position.yPos + 100};
        }
        else if(currentGrid == &faceE){
            unwrappedPosition = Grid::Coord{position.xPos,  position.yPos + 100};
        }
        else if(currentGrid == &faceF){
            unwrappedPosition = Grid::Coord{position.xPos,  position.yPos + 150};
        }

        return std::pair{unwrappedPosition, direction};
    }

    // void setPositionDirectionAndFace(Grid::Coord coord, Grid::Direction direction, char face){
    //     this->position = coord;
    //     this->direction = direction;
    //     switch(face) {
    //         case 'A': currentGrid=&faceA; break;
    //         case 'B': currentGrid=&faceB; break;
    //         case 'C': currentGrid=&faceC; break;
    //         case 'D': currentGrid=&faceD; break;
    //         case 'E': currentGrid=&faceE; break;
    //         case 'F': currentGrid=&faceF; break;
    //         default: assert(false); break;
    //     }
    // }


private:
    Grid::Grid<char> faceA, faceB, faceC, faceD, faceE, faceF;
    Grid::Coord position = Grid::Coord{0,0};
    Grid::Direction direction = Grid::Direction::Right;
    Grid::Grid<char>* currentGrid = &faceA;
};

std::pair<Grid::Coord, Grid::Direction> getCubeFinalPosition(const auto& grid, const auto& instructions) {
    //assuming 50x50 in the above format (in the comment)
    Cube cube(grid);
    
    for(const Instruction& instruction: instructions){
        cube.makeMove(instruction);
    }
    return cube.getUnwrappedPositionAndDirection();
}

unsigned int toScore(Grid::Direction direction){
    switch(direction) {
        case Grid::Direction::Right: return 0;
        case Grid::Direction::Down: return 1;
        case Grid::Direction::Left: return 2;
        case Grid::Direction::Up: return 3;
        default: assert(false); return 0;
    }
}

int main() {
    std::ifstream in("input/input22.txt");
    Grid::Grid<char> grid(std::identity{});
    std::vector<Instruction> instructions;
    in >> grid >> instructions;
    auto [coord, direction] = getFinalPosition(grid, instructions); 
    std::cout << "Password: " << ((coord.yPos+1) * 1000 + (coord.xPos+1) * 4 + toScore(direction)) << "\n";


    auto [cubeCoord, cubeDirection] = getCubeFinalPosition(grid, instructions); 
    std::cout << "Password: " << ((cubeCoord.yPos+1) * 1000 + (cubeCoord.xPos+1) * 4 + toScore(cubeDirection)) << "\n";
    
    return 0;
}