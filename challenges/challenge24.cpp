#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

#include "grid.h"
#include "input.h"

Grid::Coord projectedPosition(size_t maxX, size_t maxY, Grid::Coord coord, Grid::Direction direction, unsigned int tick) {
    if(direction == Grid::Direction::Right) {
        int value = coord.xPos - 1 + tick;
        return Grid::Coord{static_cast<int>(value % (maxX - 1) + 1), coord.yPos};
    }
    if(direction == Grid::Direction::Left) {
        int value = coord.xPos - 1 - tick ;
        while(value < 0) {
            value += (maxX-1);
        }
        return Grid::Coord{static_cast<int>(value % (maxX - 1) + 1), coord.yPos};
    }
    if(direction == Grid::Direction::Up) {
        int value = coord.yPos - 1 - tick ;
        while(value < 0) {
            value += (maxY-1);
        }
        return Grid::Coord{coord.xPos, static_cast<int>(value % (maxY-1) + 1)};
    }
    if(direction == Grid::Direction::Down) {
        int value = coord.yPos - 1 + tick ;
        return Grid::Coord{coord.xPos, static_cast<int>(value % (maxY-1) + 1)};
    }

    assert(false);
    return Grid::Coord{0,0};
}


char getOppositeBlizzard(Grid::Direction direction){
    if(direction == Grid::Direction::Up) { return 'v'; }
    if(direction == Grid::Direction::Down) { return '^'; }
    if(direction == Grid::Direction::Left) { return '>'; }
    if(direction == Grid::Direction::Right) { return '<'; }
    assert(false);
    return '\0';
}


unsigned int getMinutesUntilPathCompleted(const Grid::Grid<char>& grid, Grid::Coord startingPosition, Grid::Coord endingPosition, size_t startingTicks){
    std::set<Grid::Coord> possibilities {startingPosition};
    auto ticks = startingTicks;
    auto maxY = grid.getMaxY();
    auto maxX = grid.getMaxX();
    while(!possibilities.empty()){
        std::set<Grid::Coord> newPossibilities;
        for(const auto& coord: possibilities) {
            if(coord == endingPosition) {
                return ticks;
            }
            auto candidates = grid.getNeighbors(coord);
            candidates.push_back(coord); // staying put is an option
            for(const auto& candidate: candidates){
                bool willBlizzardsCollide = false;
                if(grid[candidate] == '#'){
                    continue;
                }
                for(auto direction: {Grid::Direction::Up, Grid::Direction::Down, Grid::Direction::Left, Grid::Direction::Right}){
                    auto projected = projectedPosition(maxX, maxY, candidate, direction, ticks+1 );
                    if(grid[projected] == getOppositeBlizzard(direction)) {
                        willBlizzardsCollide = true;
                        break;
                    }
                }
                if(!willBlizzardsCollide){
                    newPossibilities.insert(candidate);
                }
            }
        }
        std::swap(possibilities, newPossibilities);
        ++ticks;
    }
    assert(false); // should not reeach here
    return 0;
}

unsigned int getMinutesUntilExit(const Grid::Grid<char>& grid) {
    auto startingPosition = std::ranges::find_if(grid, [](const auto& p) {
        auto [coord, character] = p;
        return coord.yPos == 0 && character == '.';
    });
    auto endingPosition = std::ranges::find_if(grid, [&grid](const auto& p){
        auto [coord, character] = p;
        return coord.yPos == grid.getMaxY() && character == '.';
    });
    return getMinutesUntilPathCompleted(grid, startingPosition->first, endingPosition->first, 0);
}

unsigned int getMinutesUntilThereAndBackAgainAndBack(const Grid::Grid<char>& grid) {
    auto startingPosition = std::ranges::find_if(grid, [](const auto& p) {
        auto [coord, character] = p;
        return coord.yPos == 0 && character == '.';
    });
    auto endingPosition = std::ranges::find_if(grid, [&grid](const auto& p){
        auto [coord, character] = p;
        return coord.yPos == grid.getMaxY() && character == '.';
    });
    auto untilEnd = getMinutesUntilPathCompleted(grid, startingPosition->first, endingPosition->first, 0);
    auto untilStart = getMinutesUntilPathCompleted(grid, endingPosition->first, startingPosition->first, untilEnd+1);
    return getMinutesUntilPathCompleted(grid, startingPosition->first, endingPosition->first, untilStart + 1);
}


int main() {
    Grid::Grid<char> grid{std::identity{}};

    std::ifstream in("input/input24.txt");
    in >> grid;

    std::cout << "Minutes until end: " << getMinutesUntilExit(grid) << "\n";
    std::cout << "Minutes until end-start-end: " << getMinutesUntilThereAndBackAgainAndBack(grid) << "\n";
}