#include <numeric>
#include <cassert>
#include <iostream>
#include <istream>
#include <numeric>
#include <ranges>
#include <set>
#include <span>
#include <unordered_map>
#include <vector>

#include "grid.h"
#include "input.h"


struct LineSegment {
    Grid::Coord point1;
    Grid::Coord point2;
};

using LineSegments = std::vector<LineSegment>;
const Grid::Coord ORIGIN{500, 0};

class Cavern{
public:
    explicit Cavern(std::span<LineSegments> formations) {
        for(const auto& formation: formations){
            for(const LineSegment& segment: formation){
                auto pointsBetween = Grid::getPointsBetween(segment.point1, segment.point2);
                for (const auto& point: pointsBetween){
                    blockedAreas.try_emplace(point.xPos, std::set<int>{});
                    blockedAreas[point.xPos].insert(point.yPos);
                }
            }
        }
    }

    void placeFloor() {
        auto biggestYElements = std::views::transform(blockedAreas, [](const auto& p){
            return *std::ranges::max_element(p.second);
        });
        floor = *std::ranges::max_element(biggestYElements) + 2;
    }

    bool placeSand() {
        if(sandAtOrigin) { 
            return false;
        }
        auto sand = ORIGIN;
        bool atRest = false;
        while(!atRest) {

            // check for drop straight down
            auto straightDown = blockedAreas[sand.xPos].lower_bound(sand.yPos+1);
            auto diagonalLeft = blockedAreas[sand.xPos-1].lower_bound(sand.yPos+1);
            auto diagonalRight = blockedAreas[sand.xPos+1].lower_bound(sand.yPos+1);
            assert(!blockedAreas[sand.xPos].contains(sand.yPos));
            if(straightDown == blockedAreas[sand.xPos].end() || diagonalLeft == blockedAreas[sand.xPos - 1].end() || diagonalRight == blockedAreas[sand.xPos +1].end()){
                if(floor){
                    if(straightDown == blockedAreas[sand.xPos].end()){
                        blockedAreas[sand.xPos].insert(floor.value() - 1);
                    }
                    else if(diagonalLeft == blockedAreas[sand.xPos-1].end()){
                        blockedAreas[sand.xPos-1].insert(floor.value() - 1);
                    }
                    else if(diagonalRight == blockedAreas[sand.xPos+1].end()){
                        blockedAreas[sand.xPos+1].insert(floor.value() - 1);
                    }
                    atRest = true;
                }
                else {
                    return false;
                }
            }
            else if(*straightDown == sand.yPos + 1 && *diagonalLeft == sand.yPos + 1 && *diagonalRight == sand.yPos+1){
                blockedAreas[sand.xPos].insert(sand.yPos);
                atRest = true;
            }
            else if(*straightDown != sand.yPos + 1) {
                sand.yPos = *straightDown-1;
            }
            else if(*diagonalLeft != sand.yPos + 1) {
                sand.yPos = *diagonalLeft-1;
                --sand.xPos;
            }
            else if(*diagonalRight != sand.yPos + 1) {
                sand.yPos = *diagonalRight-1;
                ++sand.xPos;
            }
            if(sand == ORIGIN) {
               sandAtOrigin = true; 
            }
        }
        return true;
    }


private:
    //xpos to y pos of blockages
    std::unordered_map<int, std::set<int>> blockedAreas;
    std::optional<int> floor;
    bool sandAtOrigin = false;
};

std::istream& operator>>(std::istream& stream, LineSegments& lineSegments){
    std::string text;
    getline(stream, text);
    auto points = input::split(text, " -> ") | std::views::transform([](const auto& s){
        auto pos = s.find(",");
        assert(pos != std::string::npos);
        std::string xPos{s.substr(0, pos)};
        std::string yPos{s.substr(pos+1, std::string::npos)};
        return Grid::Coord{std::stoi(xPos), std::stoi(yPos)};
    }); 
    auto lines = std::accumulate(points.begin()+2, points.end(), LineSegments{LineSegment{*points.begin(), *(points.begin() + 1)}}, [](LineSegments segments, const auto& coord){
        LineSegments out(std::move(segments));
        out.emplace_back(out.rbegin()->point2, coord);
        return out;
    });
    lineSegments.insert(lineSegments.end(), lines.begin(), lines.end());
    return stream;
}

unsigned int getNumberOfSandGrainsPlaced(std::span<LineSegments> lines) {
    Cavern cavern{lines};
    unsigned int sandPlaced = 0;
    while(cavern.placeSand()) {
        ++sandPlaced;
    }
    return sandPlaced;
}

unsigned int getNumberOfSandGrainsPlacedWithNewScan(std::span<LineSegments> lines) {
    Cavern cavern{lines};
    cavern.placeFloor();
    unsigned int sandPlaced = 0;
    while(cavern.placeSand()) {
        ++sandPlaced;
    }
    return sandPlaced;
}



int main() {
    auto lines = input::readLines<LineSegments>("input/input14.txt");
    std::cout << getNumberOfSandGrainsPlaced(lines) << "\n:";
    std::cout << getNumberOfSandGrainsPlacedWithNewScan(lines) << "\n:";
}