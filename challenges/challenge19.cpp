#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <numeric>
#include <regex>
#include <span>
#include <map>
#include <ranges>

#include "input.h"

struct Robots { 
    unsigned int obsidian = 0;
    unsigned int clay = 0;
    unsigned int ore = 0;

    std::strong_ordering operator<=>(const Robots& state) const = default;
};


struct State { 
    unsigned int obsidianCollected = 0;
    unsigned int clayCollected = 0;
    unsigned int oreCollected = 0;
    Robots robots;

    State(unsigned int obsidian, unsigned int clay, unsigned int ore, Robots robots) :obsidianCollected(obsidian), clayCollected(clay), oreCollected(ore), robots(std::move(robots)) { }
    std::strong_ordering operator<=>(const State& state) const = default;
};
void addToStatesMap(auto& newStates, const auto& state, const auto& geodes)
{
    newStates.try_emplace(state, geodes);
    newStates[state] = std::max(newStates[state], geodes);
}

struct Blueprint {
    unsigned long id = 0;
    unsigned long oreRobotOreCost = 0;
    unsigned long clayRobotOreCost = 0;
    unsigned long obsidianRobotOreCost = 0;
    unsigned long obsidianRobotClayCost = 0;
    unsigned long geodeRobotOreCost = 0;
    unsigned long geodeRobotObsidianCost = 0;


    friend std::istream& operator>>(std::istream& stream, Blueprint & blueprint) {
        std::string text;
        getline(stream, text);
        std::regex blueprintRegex { "Blueprint (\\d+): Each ore robot costs (\\d+) ore. Each clay robot costs (\\d+) ore. Each obsidian robot costs (\\d+) ore and (\\d+) clay. Each geode robot costs (\\d+) ore and (\\d+) obsidian."};
        std::smatch match;
        auto result = std::regex_search(text, match, blueprintRegex);
        assert(result && match.size() == 8);
        blueprint.id = std::stoul(match[1]);
        blueprint.oreRobotOreCost = std::stoul(match[2]);
        blueprint.clayRobotOreCost = std::stoul(match[3]);
        blueprint.obsidianRobotOreCost = std::stoul(match[4]);
        blueprint.obsidianRobotClayCost = std::stoul(match[5]);
        blueprint.geodeRobotOreCost = std::stoul(match[6]);
        blueprint.geodeRobotObsidianCost = std::stoul(match[7]);
        return stream;
    }

    unsigned int getGeodes(unsigned int minutes) const {
        std::map<State, unsigned int> states{{State {0, 0, 0, { 0, 0, 1}}, 0U}};
        for(auto minute = 1U; minute <= minutes; ++minute){
            std::cout << minute << "\n";
            decltype(states) newStates;
            auto maxGeodes = 0U;
            for(const auto& statePair: states){
                auto& [state, geodes] = statePair;
                // if we produce a new robot every minute from hear on out, will we eclipse our max geodes?
                // if not, let's skip it
                auto minutesLeft = minutes - minute;
                if(geodes + (minutesLeft * (minutesLeft + 1))/2 < maxGeodes) {
                    continue;
                }
                State newState {
                    state.obsidianCollected + state.robots.obsidian,
                    state.clayCollected + state.robots.clay,
                    state.oreCollected + state.robots.ore,
                    state.robots
                };
                addToStatesMap(newStates, newState, geodes);
                //only produce ore if its not the last turn
                if(minute != minutes && state.oreCollected >= oreRobotOreCost){
                    State oreState(newState);
                    oreState.oreCollected -= oreRobotOreCost;
                    oreState.robots.ore++;
                    addToStatesMap(newStates, oreState, geodes);
                }
                // don't produce clay on the last turn, becasue it won't matter for geodes
                if(minute < minutes - 1 && state.oreCollected >= clayRobotOreCost){
                    State clayState(newState);
                    clayState.oreCollected -= clayRobotOreCost;
                    clayState.robots.clay++;
                    addToStatesMap(newStates, clayState, geodes);
                }
                // don't build an obsidian bot on the second to last turn, becasue it won't help with geode
                if(minute < minutes - 1 && state.oreCollected >= obsidianRobotOreCost && state.clayCollected >= obsidianRobotClayCost){
                    State obsidianState(newState);
                    obsidianState.oreCollected -= obsidianRobotOreCost;
                    obsidianState.clayCollected -= obsidianRobotClayCost;
                    obsidianState.robots.obsidian++;
                    addToStatesMap(newStates, obsidianState, geodes);
                }
                // don't produce a geode bot on the last turn
                if(minute != minutes && state.oreCollected >= geodeRobotOreCost && state.obsidianCollected >= geodeRobotObsidianCost){
                    State geodeState(newState);
                    geodeState.oreCollected -= geodeRobotOreCost;
                    geodeState.obsidianCollected -= geodeRobotObsidianCost;
                    addToStatesMap(newStates, geodeState, geodes+minutesLeft);
                    maxGeodes = std::max(maxGeodes, geodes + minutesLeft);
                }

            }
            std::swap(newStates, states);
        }
        return std::ranges::max(states | std::views::values);
    }
};

unsigned int getTotalBlueprintQuality(std::span<Blueprint> blueprints, unsigned int minutes) {
    return std::accumulate(blueprints.begin(), blueprints.end(), 0U, [minutes](unsigned int sum, const Blueprint& blueprint){ 
        std::cout << "Blueprint " << blueprint.id << "\n";
        return sum + blueprint.id * blueprint.getGeodes(minutes);
    });
}

unsigned int getTotalExtendedBlueprintQuality(std::span<Blueprint> blueprints, unsigned int minutes) {
    return std::accumulate(blueprints.begin(), blueprints.end(), 1U, [minutes](unsigned int sum, const Blueprint& blueprint){ 
        std::cout << "Blueprint " << blueprint.id << "\n";
        return sum * blueprint.getGeodes(minutes);
    });
}

int main() {
    auto blueprints = input::readLines<Blueprint>("input/input19.txt");
    std::cout << "Total blueprint quality: " << getTotalBlueprintQuality(blueprints, 24) << "\n";
    std::cout << "Total extended blueprint quality: " << getTotalExtendedBlueprintQuality(std::ranges::subrange(blueprints.begin(), blueprints.begin() + 3), 32) << "\n";
}
