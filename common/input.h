#include <concepts>
#include <fstream>
#include <string>
#include <string_view>
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

    std::vector<std::string> split(const std::string& text, const std::string& delimiter) {
        std::vector<std::string> out;
        std::string::size_type pos;
        std::string::size_type start;
        for(start=0, pos = text.find(delimiter); pos != std::string::npos; start=pos+delimiter.length(), pos = text.find(delimiter, pos+delimiter.length())){
            if(start != pos){
                out.emplace_back(text.substr(start, pos));
            }
        }
        if (start != pos){
            out.emplace_back(text.substr(start, pos));
        }
        return out;
    }
} // namespace input