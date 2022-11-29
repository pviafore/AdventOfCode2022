#include <concepts>
#include <fstream>
#include <string>
#include <vector>

template<typename T>
concept InputStreamable = requires(T t, std::istream& is){
    is >> t;
};

template<typename T>
concept StreamInitializable = std::default_initializable<T> && std::copy_constructible<T> && InputStreamable<T>;

namespace input {
    template <StreamInitializable T=std::string>
    std::vector<T> readLines(const std::string& filename){
        std::vector<T> lines;
        std::ifstream in(filename);
        while (in.good()){
            T element;
            in >> element;
            if(in.good()){
                lines.push_back(element);
            }
        }
        return lines;
    }
} // namespace input