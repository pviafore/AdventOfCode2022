#include <algorithm>
#include <cassert>
#include <functional>
#include <istream>
#include <iostream>
#include <ranges>
#include <set>

#include "grid.h"
#include "input.h"


unsigned int findShortestPathFrom(const auto& grid, Grid::Coord start, Grid::Coord end){
    std::set<Grid::Coord> seen {start};
    std::set<Grid::Coord> currentPositions {start};

    unsigned int steps = 0;
    while(currentPositions.find(end) == currentPositions.end()){
        std::set<Grid::Coord> newPositions;
        for(auto position: currentPositions) {
            auto neighbors = grid.getNeighbors(position);
            for(auto& neighbor: neighbors){
                if (grid[neighbor] <= grid[position]+1 && seen.find(neighbor) == seen.end()){ 
                    newPositions.insert(neighbor);
                }
            }
        }
        ++steps;
        if(newPositions.empty()) {
            // no way forward, return a large amount
            return UINT32_MAX;
        }
        currentPositions = newPositions;
        seen.insert(currentPositions.begin(), currentPositions.end());
    }

    return steps;

}

unsigned int findShortestPathFrom(const auto& grid, std::vector<Grid::Coord> startCoords, Grid::Coord end){
    auto shortestPaths = std::views::transform(startCoords, [&grid, end](const auto& start){
        return findShortestPathFrom(grid, start, end);
    }); 
    return *std::ranges::min_element(shortestPaths);
}

unsigned int getShortestPath(const auto& oldGrid){
    Grid::Grid grid{oldGrid};
    auto start = grid.find('S');
    auto end = grid.find('E');
    grid[start.value()] = 'a';
    grid[end.value()] = 'z';
    assert(start && end);

    return findShortestPathFrom(grid, start.value(), end.value());
}

// I could Floyd-Warshall it, but I don't feel like it
unsigned int getShortestPathFromAnyA(const auto& oldGrid){
    Grid::Grid grid{oldGrid};
    auto start = grid.find('S');
    auto end = grid.find('E');
    grid[start.value()] = 'a';
    grid[end.value()] = 'z';
    auto aValues = std::ranges::views::filter(grid, [](const auto& coordValuePair){
        return coordValuePair.second == 'a';
    }) | std::views::transform([](const auto& coordValuePair){
        return coordValuePair.first;
    });
    return findShortestPathFrom(grid, std::vector<Grid::Coord>{aValues.begin(), aValues.end()}, end.value());
}

int main() { 
    Grid::Grid<char> grid{ std::identity{}};
    std::ifstream stream("input/input12.txt");
    stream >> grid;

    std::cout << "Shortest path: " << getShortestPath(grid) << "\n";
    std::cout << "Shortest path from any a: " << getShortestPathFromAnyA(grid) << "\n";
}