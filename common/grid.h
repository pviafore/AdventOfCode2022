#include <cassert>
#include <functional>
#include <istream>
#include <ostream>
#include <map>
#include <utility>

namespace Grid {


    enum class Direction {
        Up,
        Left,
        Down,
        Right
    };

    struct Coord {
        int xPos; 
        int yPos;

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

    template <typename T>
    class Grid{
    public:

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
                    grid.grid[{rowIndex, columnIndex++}] = grid.mapper(symbol);
                }
                rowIndex += 1;
                grid.maxY = columnIndex-1;
            }
            grid.maxX = rowIndex-1;
            return stream;
        }

        T operator[](Coord coord) const {
            return grid.at(coord);
        }

        ConstIterator begin() const {
            return grid.begin();
        }
        ConstIterator end() const { 
            return grid.end();
        }

        int getMaxY() const {
            return maxY;
        }
        
        int getMaxX() const {
            return maxX;
        }

    private:
        std::map<Coord, T> grid;
        std::function<T(char)> mapper;
        int maxX = 0;
        int maxY = 0;
    };
} // namespace grid