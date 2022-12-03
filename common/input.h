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
            if constexpr (std::is_same<T, std::string>()){
                getline(in, element);
            }
            else {
                in >> element;
            }
            lines.push_back(element);
        }
        return lines;
    }

    std::string readSingleLine(const std::string& filename) {
        std::ifstream in(filename);
        std::string text;
        getline(in, text);
        return text;

    }
} // namespace input