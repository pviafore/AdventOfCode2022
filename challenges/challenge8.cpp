#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>

#include <set>
#include "grid.h"
#include "input.h"

std::byte toByte(char symbol){
    return static_cast<std::byte>(symbol - 48);
}

enum class Direction{
    Top,
    Left,
    Bottom,
    Right
};

size_t getNumberOfTreesVisible(const auto& grid){

    const std::map<std::pair<Direction, int>, Grid::Coord> lineOfSightToTreeMapping;
    std::set<Grid::Coord> visible;
    for( const auto& [coord, tree]: grid){
        auto [xPos, yPos] = coord;
        if (yPos == 0) {
            lineOfSightToTreeMapping[{Direction::Top, xPos}] = coord;
            visible.insert(coord);
            continue;
        }
        if (xPos == 0) {
            lineOfSightToTreeMapping[{Direction::Left, yPos}] = coord;
            visible.insert(coord);
            continue;
        }

        auto tallestFromLeft = lineOfSightToTreeMapping[{Direction::Left, yPos}];
        auto tallestFromTop = lineOfSightToTreeMapping[{Direction::Top, xPos}];

        if (tree > grid[tallestFromLeft]){
            lineOfSightToTreeMapping[{Direction::Left, yPos}] = coord;
            visible.insert(coord);
        }
        if (tree > grid[tallestFromTop]){
            lineOfSightToTreeMapping[{Direction::Top, xPos}] = coord;
            visible.insert(coord);
        }
    }
    for( const auto& [coord, tree]: std::ranges::reverse_view(grid)){
        auto [xPos, yPos] = coord;
        if (yPos == grid.getMaxX()) {
            lineOfSightToTreeMapping[{Direction::Bottom, xPos}] = coord;
            visible.insert(coord);
            continue;
        }
        if (xPos == grid.getMaxY()) {
            lineOfSightToTreeMapping[{Direction::Right, yPos}] = coord;
            visible.insert(coord);
            continue;
        }

        auto tallestFromRight = lineOfSightToTreeMapping[{Direction::Right, yPos}];
        auto tallestFromBottom = lineOfSightToTreeMapping[{Direction::Bottom, xPos}];

        if (tree > grid[tallestFromRight]){
            lineOfSightToTreeMapping[{Direction::Right, yPos}] = coord;
            visible.insert(coord);
        }
        if (tree > grid[tallestFromBottom]){
            lineOfSightToTreeMapping[{Direction::Bottom, xPos}] = coord;
            visible.insert(coord);
        }
    }

    return visible.size();
}

unsigned int getScoreForMostScenicTree(const auto& grid){
    std::map<std::pair<Direction, Grid::Coord>, Grid::Coord> lineOfSightPerTree;
    auto getFurthestTree = [&grid, &lineOfSightPerTree](std::byte tree, Direction direction, Grid::Coord pos){

            while(tree > grid[pos] && pos != lineOfSightPerTree[{direction, pos}]){
                pos = lineOfSightPerTree[{direction, pos}];
            }
            return pos;
        };
    
    for( const auto& [coord, tree]: grid){
        auto [xPos, yPos] = coord;
        // mark yourself as the furthers that you can see
        if (yPos == 0) {
            lineOfSightPerTree[{Direction::Top, coord}] = coord;
            continue;
        }
        if (xPos == 0) {
            lineOfSightPerTree[{Direction::Left, coord}] = coord;
            continue;
        }
        
        lineOfSightPerTree[{Direction::Left, coord}] = getFurthestTree(tree, Direction::Left, {xPos - 1, yPos});
        lineOfSightPerTree[{Direction::Top, coord}] = getFurthestTree(tree, Direction::Top, {xPos , yPos - 1});
    }
    for( const auto& [coord, tree]: std::ranges::reverse_view(grid)){
        auto [xPos, yPos] = coord;
        // mark yourself as the furthers that you can see
        if (yPos == grid.getMaxY()) {
            lineOfSightPerTree[{Direction::Bottom, coord}] = coord;
            continue;
        }
        if (xPos == grid.getMaxX()) {
            lineOfSightPerTree[{Direction::Right, coord}] = coord;
            continue;
        }
        
        lineOfSightPerTree[{Direction::Right, coord}] = getFurthestTree(tree, Direction::Right, {xPos + 1, yPos});
        lineOfSightPerTree[{Direction::Bottom, coord}] = getFurthestTree(tree, Direction::Bottom, {xPos , yPos + 1});
    }

    auto getScenicScore = [&lineOfSightPerTree](const auto& coordTreePair){
        auto [coord, tree] = coordTreePair;
        return Grid::getManhattanDistance(coord, lineOfSightPerTree[{Direction::Top, coord}]) * 
               Grid::getManhattanDistance(coord, lineOfSightPerTree[{Direction::Bottom, coord}]) * 
               Grid::getManhattanDistance(coord, lineOfSightPerTree[{Direction::Right, coord}]) * 
               Grid::getManhattanDistance(coord, lineOfSightPerTree[{Direction::Left, coord}]);
    };
    auto scores = grid | std::views::transform(getScenicScore);
    return *std::ranges::max_element(scores);

}

int main() {
    Grid::Grid<std::byte> grid{toByte};
    std::ifstream stream("input/input8.txt");
    stream >> grid;

    std::cout << "Trees visible: " << getNumberOfTreesVisible(grid) << "\n";
    std::cout << "Most Scenic Score: " << getScoreForMostScenicTree(grid) << "\n";


    return 0;

}