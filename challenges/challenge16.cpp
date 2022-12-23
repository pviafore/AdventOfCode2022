#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <map>
#include <numeric>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>


#include "input.h"

struct Valve {
    unsigned long flowRate = 0;
    std::vector<std::string> tunnels;
};

using Room = std::pair<std::string, Valve>;
using RoomLookup = std::unordered_map<std::string, Valve>;

std::istream& operator>>(std::istream& stream, Room& room){
    std::regex roomRegex{"Valve ([A-Z]{2}) has flow rate=(\\d+); tunnel[s]? lead[s]? to valve[s]? (.*)"};
    std::smatch match;
    std::string line;
    getline(stream, line);
    auto result = std::regex_search(line, match, roomRegex);
    assert(result);
    assert(match.size() == 4);
    room.first = match[1];
    room.second.flowRate = std::stoul(match[2]);
    auto tunnels = input::split(match[3], ", ");
    std::swap(tunnels, room.second.tunnels);

    return stream;
}

struct Position {
    std::string valve;
    std::string elephantValve;
    std::set<std::string> openValves;

    std::strong_ordering operator<=>(const Position& position) const = default;

    bool isValveOpen(const std::string& valve) const {
        return openValves.find(valve) != openValves.end();
    }
};

void insertRate(auto& newPositions, const Position& key, unsigned int newRate){
    try {
        auto oldRate = newPositions.at(key);
        newPositions[key] = std::max(oldRate, newRate);
    }
    catch(...) {
        newPositions.emplace(std::make_pair(std::move(key), newRate));
    }
    
}


unsigned int getMaximumPressure(const RoomLookup& roomLookup) {
    const std::string starting = "AA";
    std::map<Position, unsigned int> positions {{{starting, "", std::set<std::string>{} }, 0U}};
    for(auto minutes = 1U; minutes <= 30; ++minutes){
        std::map<Position, unsigned int> newPositions{positions.begin(), positions.end()};
        for(const auto& current: positions){
            auto [position, totalFlow] = current;

            auto room = roomLookup.at(position.valve);
            if(room.flowRate != 0 && !position.isValveOpen(position.valve) ) {
                unsigned int newRate = totalFlow + (30 - minutes) * room.flowRate;
                auto openValves = position.openValves;
                openValves.insert(position.valve);
                // if the new rate is actually greater than the existing position, add it
                auto key = Position{position.valve, "", std::move(openValves)};
                insertRate(newPositions, key, newRate);
            }
            for(const auto& tunnel: room.tunnels){
                auto key = Position{tunnel, "", position.openValves};
                insertRate(newPositions, key, totalFlow);
            }
        }
        std::swap(positions, newPositions);
    }
    return std::ranges::max_element(positions, [](const auto& position1, const auto& position2){
        return position1.second < position2.second;
    })->second;
}

unsigned int getMaximumPressureWithElephant(const RoomLookup& roomLookup) {
    const std::string starting = "AA";
    const std::string elephant = "AA";
    std::map<Position, unsigned int> positions {{{starting, elephant, std::set<std::string>{} }, 0U}};
    for(auto minutes = 1U; minutes <= 26; ++minutes){
        std::map<Position, unsigned int> newPositions{positions.begin(), positions.end()};
        for(const auto& current: positions){
            auto [position, totalFlow] = current;

            auto room = roomLookup.at(position.valve);
            auto elephantRoom = roomLookup.at(position.elephantValve);
            // four things can happen - we both can open a valve
            // one can move and the other can open a valve (or vice versa)
            // or we both move
            bool isMyValveOpen = (room.flowRate != 0 && !position.isValveOpen(position.valve) );
            bool isElephantValveOpen = (elephantRoom.flowRate != 0 && !position.isValveOpen(position.elephantValve) );
            auto myTunnels = room.tunnels;
            auto elephantTunnels = elephantRoom.tunnels;

            if(isMyValveOpen && isElephantValveOpen && position.valve != position.elephantValve){
                unsigned int newRate = totalFlow + (26 - minutes) * (room.flowRate + elephantRoom.flowRate);
                auto openValves = position.openValves;
                openValves.insert(position.valve);
                openValves.insert(position.elephantValve);
                auto key = Position{position.valve, position.elephantValve, std::move(openValves)};
                insertRate(newPositions, key, newRate); 
            }
            else if(isMyValveOpen){
                unsigned int newRate = totalFlow + (26 - minutes) * (room.flowRate);
                auto openValves = position.openValves;
                openValves.insert(position.valve);
                for(const auto& tunnel: elephantRoom.tunnels){
                    auto key = Position{position.valve, tunnel, openValves};
                    insertRate(newPositions, key, newRate); 
                }
            }
            else if(isElephantValveOpen){
                unsigned int newRate = totalFlow + (26 - minutes) * (elephantRoom.flowRate);
                auto openValves = position.openValves;
                openValves.insert(position.elephantValve);
                for(const auto& tunnel: room.tunnels){
                    auto key = Position{tunnel, position.elephantValve, openValves};
                    insertRate(newPositions, key, newRate); 
                }
            }
            else {
                for(const auto& tunnel: room.tunnels){
                    for(const auto& elephantTunnel: elephantRoom.tunnels){
                        auto key = Position{tunnel, elephantTunnel, position.openValves};
                        insertRate(newPositions, key, totalFlow); 

                    }
                }
            }
        }
        std::swap(positions, newPositions);
    }
    return std::ranges::max_element(positions, [](const auto& position1, const auto& position2){
        return position1.second < position2.second;
    })->second;
}

int main() {
    auto rooms = input::readLines<Room>("input/input16.txt");
    RoomLookup roomLookup{rooms.begin(), rooms.end()};
    std::cout << "Maximum pressure " << getMaximumPressure(roomLookup) << "\n";
    std::cout << "Maximum pressure " << getMaximumPressureWithElephant(roomLookup) << "\n";
}