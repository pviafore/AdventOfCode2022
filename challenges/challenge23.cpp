#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <span>
#include <vector>

#include "grid.h"
#include "input.h"

using Offsets = std::vector<Grid::Coord>;
Offsets northOffsets = {Grid::Coord{-1, -1}, Grid::Coord{0, -1}, Grid::Coord{1,-1}}; 
Offsets southOffsets = {Grid::Coord{-1, 1}, Grid::Coord{0, 1}, Grid::Coord{1,1}}; 
Offsets westOffsets = {Grid::Coord{-1, -1}, Grid::Coord{-1, 0}, Grid::Coord{-1,1}}; 
Offsets eastOffsets = {Grid::Coord{1, -1}, Grid::Coord{1, 0}, Grid::Coord{1,1}}; 
Offsets allNeighbors = {Grid::Coord{1, -1}, Grid::Coord{1, 0}, Grid::Coord{1,1}, Grid::Coord{0, -1}, Grid::Coord{0,1}, Grid::Coord{-1, -1}, Grid::Coord{-1, 0}, Grid::Coord{-1,1}}; 

std::vector<std::pair<Offsets, Grid::Direction>> offsetDirectionPairs {
    {northOffsets, Grid::Direction::Up},
    {southOffsets, Grid::Direction::Down},
    {westOffsets, Grid::Direction::Left},
    {eastOffsets, Grid::Direction::Right},
};


class Elf {
public:
    explicit Elf(Grid::Coord pos) : coord(pos) {}
    Grid::Coord position() const { 
        return coord;
    }

    void setNewPosition(const Grid::Coord& position){
        coord = position;
    }

    std::strong_ordering operator<=>(const Elf& elf) const = default;


    Grid::Coord getProposedMove(std::span<Elf> elves) const {
        bool allAlone = std::ranges::all_of(allNeighbors, [&elves, this](const auto& neighbor){
            auto proposed = this->coord + neighbor;
            auto findElf = [proposed](const Elf& elf) { return
             elf.position() == proposed; };
            return std::ranges::find_if(elves, findElf) == elves.end();
        });
        if (allAlone) {
            return coord;
        }

        for(const auto& offsetDirectionPair: offsetDirectionPairs) {
            auto [offsets, direction] = offsetDirectionPair;
            bool isValid = std::ranges::all_of(offsets, [&elves, this](const auto& neighbor){
                auto proposed = this->coord + neighbor;
                auto findElf = [proposed](const Elf& elf) { return elf.position() == proposed; };
                return std::ranges::find_if(elves, findElf) == elves.end();
            });
            if (isValid) {
                return coord + direction;
            }
        }
        return coord;

    }
private:
    Grid::Coord coord;
};

std::vector<Elf> toElves(const Grid::Grid<char>& grid) {
    std::vector<Elf> elves;
    for(const auto& p: grid){
        if(p.second == '#'){
            elves.emplace_back(p.first);
        }
    }
    return elves;
}

bool runRound(std::span<Elf> elves) {
    std::map<Grid::Coord, unsigned int> proposedMoves;
    for(const auto& elf: elves){
        auto move = elf.getProposedMove(elves);
        proposedMoves.try_emplace(move, 0U);
        proposedMoves[move]++;
    }

    std::vector<Elf> oldElves(elves.begin(), elves.end());
    for(auto& elf: elves) {
        auto move = elf.getProposedMove(oldElves);
        if(proposedMoves.at(move) == 1){
            if(move != elf.position()){
                elf.setNewPosition(move);
            }
        }
    }
    std::rotate(offsetDirectionPairs.begin(), offsetDirectionPairs.begin() + 1, offsetDirectionPairs.end());
    std::vector<Elf> newElves(elves.begin(), elves.end());
    return oldElves != newElves;
}


size_t getEmptySpaces(std::span<Elf> elves) {
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for(const auto& elf: elves){
        minX = std::min(minX, elf.position().xPos);
        maxX = std::max(maxX, elf.position().xPos);
        minY = std::min(minY, elf.position().yPos);
        maxY = std::max(maxY, elf.position().yPos);
    }

    return (maxX - minX + 1) * (maxY - minY + 1) - elves.size();

}


size_t getEmptySpacesAfter(const Grid::Grid<char>& grid, unsigned int rounds) {
    auto elves = toElves(grid);
    for(unsigned int round = 0; round < rounds; ++round){
        runRound(elves);
    }
    return getEmptySpaces(elves);
}

size_t getEmptySpacesWhenStatic(const Grid::Grid<char>& grid) {
    size_t round = 0;
    auto elves = toElves(grid);
    while(true) {
        bool atLeastOneMoveMade = runRound(elves);
        ++round;
        if (!atLeastOneMoveMade) {
            return round;
        }
    }
    assert(false);
    return 0U;
}

int main(){
    Grid::Grid<char> grid(std::identity{});

    std::ifstream in("input/input23.txt");
    in >> grid;

    std::cout << "Empty Spaces: " << getEmptySpacesAfter(grid, 10) << "\n";


    offsetDirectionPairs = {
        {northOffsets, Grid::Direction::Up},
        {southOffsets, Grid::Direction::Down},
        {westOffsets, Grid::Direction::Left},
        {eastOffsets, Grid::Direction::Right},
    };
    std::cout << "Empty Spaces when static: " << getEmptySpacesWhenStatic(grid) << "\n";

}