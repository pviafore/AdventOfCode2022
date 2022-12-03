#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <memory>
#include <numeric>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

#include "input.h"

struct ChangeDirectory { 
    std::string directoryName; 
};

struct List {};

struct Dir {
    std::string name;
};

struct File { 
    std::string name;
    size_t size;
};

using CommandOrOutput = std::variant<ChangeDirectory, List, Dir, File>;
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
class FileTree {  // NOLINT(misc-no-recursion)
public:
    using Leaf = std::variant<FileTree, size_t>;

    explicit FileTree(FileTree* fileTree) : parent(fileTree) {
        
    }

    FileTree() = default;

    FileTree* cd(const std::string& subdirectory){
        if (subdirectory == "..") {
            if(!parent){
                assert(false);
                return nullptr;
            }
            return parent.value();
        }
        addSubdirectoryIfMissing(subdirectory);
        return &std::get<FileTree>(leaves[subdirectory]);
    }

    void addSubdirectoryIfMissing(const std::string& subdirectory) {
        if(leaves.find(subdirectory) == leaves.end()) {
            leaves.try_emplace(subdirectory, FileTree{this});
        }
    }

    void addFile(std::string filename, size_t size) {
        leaves.emplace(std::move(filename), size);
    }

    size_t getSmallFileSum() const {
        auto mySize = getSize();
        auto current = (mySize < 100'000) ? mySize : 0U;
        return current + std::accumulate(leaves.begin(), leaves.end(), static_cast<size_t>(0), [](size_t sum, const auto& keyvalue){
            auto [_, leaf] = keyvalue;
            return sum + std::visit(overloaded {  // NOLINT(clang-diagnostic-error)
                [](const FileTree& fileTree) { return fileTree.getSmallFileSum(); },
                [](size_t ) { return static_cast<size_t>(0);}
            }, leaf);
        });
    }
    
    size_t findSmallestDirectorySizeToDelete(size_t target) const {
        auto mySize = this->getSize();
        if(mySize == 0) {
            return UINT64_MAX;
        }
        std::vector<size_t> sizes;
        std::transform(leaves.begin(), leaves.end(), std::back_inserter(sizes), [target](const auto& keyvalue){
            auto [_, leaf] = keyvalue;
            return std::visit(overloaded { // NOLINT(clang-diagnostic-error)
                [target](const FileTree& fileTree) { return fileTree.findSmallestDirectorySizeToDelete(target); },
                [](size_t ) { return static_cast<size_t>(UINT64_MAX);}
            }, leaf);
        });
        return std::min(*std::min_element(sizes.begin(), sizes.end()), (mySize > target) ? mySize : UINT64_MAX);
    }

    size_t getSize() const { 
        return std::accumulate(leaves.begin(), leaves.end(), static_cast<size_t>(0), [](size_t sum, const auto& keyvalue){
            auto [_, leaf] = keyvalue;
            return sum + std::visit(overloaded { // NOLINT(clang-diagnostic-error)
                [](const FileTree& fileTree) { return fileTree.getSize(); },
                [](size_t size ) { return size;}
            }, leaf);
        });
    }



private:
    std::unordered_map<std::string, Leaf> leaves;
    std::optional<FileTree*> parent;
    
    };

std::unique_ptr<FileTree> buildFileTree(std::span<CommandOrOutput> commands) {
    auto root = std::make_unique<FileTree>();
    auto* currentContext = root.get();
    bool isListing = false;
    for(const auto& command: commands) {
        std::visit(overloaded { // NOLINT(clang-diagnostic-error)
            [&root, &currentContext, &isListing](const ChangeDirectory& change){ 
                isListing = false; 
                currentContext = (change.directoryName == "/") ? root.get() : currentContext->cd(change.directoryName);},
            [&isListing](const List&){ isListing = true; },
            [&currentContext, &isListing](const Dir& dir){ assert(isListing) ; currentContext->addSubdirectoryIfMissing(dir.name);},
            [&currentContext, &isListing](const File& file){ assert(isListing) ; currentContext->addFile(file.name, file.size);},
        }, command);
    } 
    return root;
}


std::istream& operator>>(std::istream& stream, CommandOrOutput& command){
    std::string token;
    std::string token2;
    stream >> token;
    if(token == "$") {
        stream >> token;
    }
    if (token == "ls"){
        command = List{};
        return stream;
    }
    stream >> token2;
    if (token == "cd"){
        command = ChangeDirectory { token2 };
    }
    else if(token == "dir") { 
        command = Dir { "directoryName" };
    }
    else {
        command = File { token2, std::stoul(token)};
    }
    return stream;
}


int main(){
    auto commands = input::readLines<CommandOrOutput>("input/input7.txt");
    auto fileTree = buildFileTree(commands);
    std::cout << "Total small files: " << fileTree->getSmallFileSum() << "\n";
    auto neededSpace = 30'000'000 - (70'000'000 - fileTree->getSize());
    std::cout << "Smallest directory to delete: " << fileTree->findSmallestDirectorySizeToDelete(neededSpace) << "\n";

}


