#include <algorithm>
#include <iostream>
#include <istream>
#include <ranges>
#include <regex>
#include <span>

#include <grid.h>
#include <input.h>

struct Sensor {
    Grid::Coord sensor;
    Grid::Coord nearestBeacon;

    friend std::istream& operator>>(std::istream& stream, Sensor& sensor){
        std::string text;
        getline(stream, text);
        std::regex sensorRegex{"Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at x=(-?\\d+), y=(-?\\d+)"};
        std::smatch match;
        auto result = std::regex_search(text, match, sensorRegex);
        assert(result);

        sensor.sensor = Grid::Coord{std::stoi(match[1]), std::stoi(match[2])};
        sensor.nearestBeacon = Grid::Coord{std::stoi(match[3]), std::stoi(match[4])};
        return stream;
    }

    unsigned int getDistanceToBeacon() const {
        return Grid::getManhattanDistance(sensor, nearestBeacon);
    }

    unsigned int getDistance(const Grid::Coord& coord) const{
        return Grid::getManhattanDistance(sensor, coord);
    }

   long getMinXInRangeAtRow(unsigned int row) const{
        auto distance = getDistanceToBeacon();
        auto yDistance = static_cast<long>(abs(sensor.yPos - row));
        if(yDistance > distance){
            return INT64_MAX;
        }
        return sensor.xPos - (distance - yDistance);
    }

    long getMaxXInRangeAtRow(unsigned int row) const{
        auto distance = getDistanceToBeacon();
        auto yDistance = static_cast<long>(abs(sensor.yPos - row));
        if(yDistance > distance){
            return INT64_MIN;
        }
        return sensor.xPos + (distance - yDistance);
    }

    bool canReach(const Grid::Coord& coord) const {
        return getManhattanDistance(sensor, coord) <= getDistanceToBeacon();
    }

};

unsigned int getNumberOfInvalidPositions(std::span<Sensor> sensors, int row) {
    auto minX = std::ranges::min_element(sensors, [row](const auto& sensor1, const auto& sensor2){
        return sensor1.getMinXInRangeAtRow(row) < sensor2.getMinXInRangeAtRow(row);
    })->getMinXInRangeAtRow(row);
    auto maxX = std::ranges::max_element(sensors, [row](const auto& sensor1, const auto& sensor2){
        return sensor1.getMaxXInRangeAtRow(row) < sensor2.getMaxXInRangeAtRow(row);
    })->getMaxXInRangeAtRow(row);

    unsigned int count = 0;
    for(int xPos = minX; xPos <= maxX; ++xPos){
        bool isUndetectable = std::ranges::all_of(sensors, [xPos, row](const auto& sensor){
            Grid::Coord coord{xPos, row};
            return !sensor.canReach(coord) || (sensor.getDistance(coord) <= sensor.getDistanceToBeacon() && coord != sensor.nearestBeacon);
        });
        if( isUndetectable){
            ++count;
        }
    }
    return count;
}

unsigned long findSensorTuningFrequency(std::vector<Sensor> sensors){
    std::ranges::sort(sensors, [](const auto& s1, const auto& s2) { return s1.sensor.xPos < s2.sensor.xPos; });
    for(int row = 0; row < 4'000'000; ++row){
        std::vector<std::pair<long, long>> sortedRanges;
        auto sensorPair = [row](const auto& sensor){ return std::make_pair(sensor.getMinXInRangeAtRow(row), sensor.getMaxXInRangeAtRow(row)); };
        std::ranges::copy(std::views::transform(sensors, sensorPair), std::back_inserter(sortedRanges));
        std::ranges::sort(sortedRanges);

        long min = 0;
        for(auto& range: sortedRanges) { 
            if (range.second < 0){
                continue;
            }
            
            if(range.first > 4'000'000){
               break;  
            }
            if(range.first <= min + 1){
                min = std::max(range.second, min);
            }
            else {
                // there is a gap
                assert(range.first == min + 2);
                return static_cast<unsigned long>(range.first-1) * 4'000'000 + row;

            }
        }

    }
    assert(false);
    return 0;

}


int main(){
    auto sensors = input::readLines<Sensor>("input/input15.txt");
    std::cout << getNumberOfInvalidPositions(sensors, 2'000'000) << "\n";
    std::cout << findSensorTuningFrequency(sensors) << "\n";
    return 0;
}