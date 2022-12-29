#include <algorithm>
#include <cassert>
#include <functional>
#include <istream>
#include <ostream>
#include <map>
#include <optional>
#include <ranges>
#include <utility>

namespace Grid {


    enum class Direction {
        Up,
        Left,
        Down,
        Right
    };

    Direction turn(Direction facing, Direction toTurn){
        if(toTurn == Direction::Left){
            switch(facing){
                case Direction::Up: return Direction::Left;
                case Direction::Left: return Direction::Down;
                case Direction::Down: return Direction::Right;
                case Direction::Right: return Direction::Up;
                default: assert(false); return Direction::Up;
            }
        }
        else {
            assert(toTurn == Direction::Right);
            switch(facing){
                case Direction::Up: return Direction::Right;
                case Direction::Right: return Direction::Down;
                case Direction::Down: return Direction::Left;
                case Direction::Left: return Direction::Up;
                default: assert(false); return Direction::Up;
            }
        }

    }

    struct Coord {
        int xPos = 0; 
        int yPos = 0;

        std::strong_ordering operator<=>(const Coord& c2) const = default;

        friend Coord operator+(const Coord& lhs, const Coord& rhs){
            return {lhs.xPos + rhs.xPos, lhs.yPos + rhs.yPos};
        }

        friend Coord operator-(const Coord&lhs, const Coord& rhs){
            return lhs + Coord{-1*rhs.xPos, -1*rhs.yPos};
        }

        friend Coord operator+(const Coord& lhs, Direction direction){
            switch(direction){
                case Direction::Up: { return lhs + Coord{0,-1}; }
                case Direction::Down: { return lhs + Coord{0,1}; }
                case Direction::Left: { return lhs + Coord{-1,0}; }
                case Direction::Right: { return lhs + Coord{1,0}; }
            }
            assert(false);
            return {0,0};
        }

        friend std::ostream& operator<<(std::ostream& stream, const Coord& coord) {
            stream << "Coord{x=" << coord.xPos << ", y=" << coord.yPos << "}";
            return stream;
        }

    };

    unsigned int getManhattanDistance(const Coord& coord1, const Coord& coord2){
        return abs(coord1.xPos - coord2.xPos) + abs(coord1.yPos - coord2.yPos);
    }

    // inclusive
    std::vector<Coord> getPointsBetween(const Coord& coord1, const Coord& coord2){
        assert(coord1.xPos == coord2.xPos || coord1.yPos == coord2.yPos); // horizontal or vertical only
        std::vector<Coord> out;
        if(coord1.xPos == coord2.xPos){
            for(int yPos = std::min(coord1.yPos, coord2.yPos); yPos <= std::max(coord1.yPos, coord2.yPos); ++yPos){
                out.emplace_back(coord1.xPos, yPos);
            };
        }
        else {
            for(int xPos = std::min(coord1.xPos, coord2.xPos); xPos <= std::max(coord1.xPos, coord2.xPos); ++xPos){
                out.emplace_back(xPos, coord1.yPos);
            };
        }
        return out;
    }

    template <typename T>
    class Grid{
    public:
        explicit Grid() : mapper(std::identity{}) {}
        explicit Grid(std::function<T(char)> mapper) : mapper(mapper) {}

        using ConstIterator = typename std::map<Coord,T>::const_iterator;
        // the type of T should be able to take a char
        friend std::istream& operator>>(std::istream& stream, Grid &grid){
            int rowIndex = 0;
            while(stream.good()){
                std::string text;
                getline(stream, text);
                if(text == "") {
                    break;
                }
                int columnIndex = 0;
                for(auto symbol: text) {
                    grid.grid[{columnIndex++, rowIndex}] = grid.mapper(symbol);
                }
                rowIndex += 1;
                grid.maxX = columnIndex-1;
            }
            grid.maxY = rowIndex-1;
            return stream;
        }

        T operator[](Coord coord) const {
            return grid.at(coord);
        }

        T& operator[](Coord coord) {
            return grid[coord];
        }

        ConstIterator begin() const {
            return grid.begin();
        }
        ConstIterator end() const { 
            return grid.end();
        }

        size_t size() const { 
            return grid.size();
        }

        int getMaxY() const {
            return maxY;
        }
        
        int getMaxX() const {
            return maxX;
        }

        int getMaxXInRow(long row) const { 
            return std::ranges::max(grid 
                | std::views::keys 
                | std::views::filter([row](const auto& c){ return c.yPos == row; }) 
                | std::views::transform([](const auto& c) { return c.xPos;}));
        }

        int getMaxYInColumn(long column) const { 
            return std::ranges::max(grid 
                | std::views::keys 
                | std::views::filter([column](const auto& c){ return c.xPos == column; }) 
                | std::views::transform([](const auto& c) { return c.yPos;}));
        }

        std::optional<Coord> find(T value) const {
            auto match = std::ranges::find_if(grid, [value](const auto& coordValuePair){
                return coordValuePair.second == value;
            });
            return (match != grid.end()) ? match->first : std::optional<Coord>{};
        }

        ConstIterator find(Coord coord) const {
            return grid.find(coord);
        }

        std::vector<Coord> getNeighbors(Coord coord) const {
            auto neighborPoints = {coord + Coord{1,0}, coord+Coord{0,1}, coord+Coord{0,-1}, coord+Coord{-1,0}};
            auto neighborsInGrid = neighborPoints | std::views::filter([this](const Coord& c) {
                return this->grid.find(c) != this->grid.end();
            });
            return {neighborsInGrid.begin(), neighborsInGrid.end()};
        }

    private:
        std::map<Coord, T> grid;
        std::function<T(char)> mapper;
        int maxX = 0;
        int maxY = 0;
    };
}// namespace grid