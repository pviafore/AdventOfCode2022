#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <ranges>
#include <span>
#include <set>
#include <vector>

#include "input.h"

using namespace std::literals;

struct Cube {
    long xPos = 0;
    long yPos = 0;
    long zPos = 0;

    friend std::istream& operator>>(std::istream& stream, Cube& cube) {
        std::string text;
        getline(stream, text);

        auto numbers = text | std::views::split(","s) | std::views::transform([](const auto& s){
            return std::stol(std::string(s.begin(), s.end()));
        });
        std::vector<unsigned long> collectedNumbers(numbers.begin(), numbers.end());
        assert(collectedNumbers.size() == 3);
        cube.xPos = collectedNumbers[0];
        cube.yPos = collectedNumbers[1];
        cube.zPos = collectedNumbers[2];

        return stream;
    }

    bool isAdjacentTo(const Cube& rhs) const {
        return (
            (abs(xPos - rhs.xPos) == 1 && yPos == rhs.yPos && zPos == rhs.zPos) ||
            (abs(yPos - rhs.yPos) == 1 && xPos == rhs.xPos && zPos == rhs.zPos) ||
            (abs(zPos - rhs.zPos) == 1 && yPos == rhs.yPos && xPos == rhs.xPos)
        );
    }

    std::strong_ordering operator<=>(const Cube& cube) const = default;

};

unsigned int getExposedFaces(std::span<Cube> cubes){
    std::vector<Cube> seenCubes;
    unsigned int exposedFaces = 0;
    for(const auto& cube1: cubes) {
        for (const auto& cube2: seenCubes){
            if (cube1.isAdjacentTo(cube2)){
                exposedFaces -= 2;
            }
        }
        seenCubes.push_back(cube1);
        exposedFaces += 6;
    }
    return exposedFaces;
}


std::vector<Cube> getNeighbors(const Cube& position){
    std::vector<Cube> out {
        {position.xPos + 1, position.yPos, position.zPos},
        {position.xPos, position.yPos + 1, position.zPos},
        {position.xPos, position.yPos, position.zPos + 1},
        {position.xPos - 1, position.yPos, position.zPos},
        {position.xPos, position.yPos - 1, position.zPos},
        {position.xPos, position.yPos, position.zPos - 1},
    };
    return out;
}

unsigned int getExternallyExposedFaces(std::span<Cube> cubes){ 
    long minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
    for(const auto& cube: cubes) {
        minX = std::min(minX, cube.xPos);
        minY = std::min(minY, cube.yPos);
        minZ = std::min(minZ, cube.zPos);
        maxX = std::max(maxX, cube.xPos);
        maxY = std::max(maxY, cube.yPos);
        maxZ = std::max(maxZ, cube.zPos);
    }
    std::set<Cube> cubeLookup { cubes.begin(), cubes.end() };
    // check from all 8 corners in case we're segmented
    std::vector<Cube> spacesToCheck{
        {minX, minY, minZ},
        {minX, minY, maxZ},
        {minX, maxY, minZ},
        {minX, maxY, maxZ},
        
        {maxX, minY, minZ},
        {maxX, minY, maxZ},
        {maxX, maxY, minZ},
        {maxX, maxY, maxZ},
    
    };
    std::set<Cube> seen;
    unsigned int externallyExposedFaces = 0;
    while(!spacesToCheck.empty()){
        Cube current = *spacesToCheck.rbegin();
        spacesToCheck.pop_back();
        if (seen.find(current) != seen.end() || cubeLookup.find(current) != cubeLookup.end()){
            continue;
        }
        seen.insert(current);
        for(const auto& neighbor: getNeighbors(current)) {
            if(neighbor.xPos < minX - 1 || neighbor.xPos > maxX + 1 || neighbor.yPos < minY - 1 || neighbor.yPos > maxY + 1 || neighbor.zPos < minZ - 1 || neighbor.zPos > maxZ + 1) {
                continue;
            }
            if(cubeLookup.find(neighbor) == cubeLookup.end()){
                spacesToCheck.push_back(neighbor);
            }
            else{
                externallyExposedFaces += 1;
            }
        }
    }
    return externallyExposedFaces;
}

int main() {
    auto cubes = input::readLines<Cube>("input/input18.txt");
    std::cout << "Exposed Faces: " << getExposedFaces(cubes) << "\n";
    std::cout << "Externally Exposed Faces: " << getExternallyExposedFaces(cubes) << "\n";
}
