#include <functional>
#include <istream>
#include <map>
#include <utility>

namespace Grid {
    struct Coord {
        int xPos; 
        int yPos;

        std::strong_ordering operator<=>(const Coord& c2) const = default;
    };

    unsigned int getManhattanDistance(const Coord& coord1, const Coord& coord2){
        return abs(coord1.xPos - coord2.xPos) + abs(coord1.yPos - coord2.yPos);
    }

    template <typename T>
    class Grid{
    public:

        Grid(std::function<T(char)> mapper) : mapper(mapper) {}

        using ConstIterator = std::map<Coord,T>::const_iterator;
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