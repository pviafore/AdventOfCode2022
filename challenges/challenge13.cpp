#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <memory>
#include <ranges>
#include <span>
#include <variant>
#include <vector>

#include "input.h"

enum class Result {
    LessThan,
    GreaterThan,
    KeepChecking
};
class Packet {

public:
    Packet() = default;
    Packet(std::string::iterator& begin, std::string::iterator end) {
        assert(begin != end && *begin == '[');
        bool lastBraceFound = false;
        while(!lastBraceFound) {
            std::string braces = "[]";
            auto nextBrace = std::find_first_of(begin+1, end, braces.begin(), braces.end());
            assert(nextBrace != end);
            auto numbers = input::split(std::string{begin+1, nextBrace}, ",");
            if(numbers.size() > 1 || numbers[0] != ""){
                auto unsignedLongs = numbers | 
                    std::views::filter([](const auto& s){ return !s.empty(); }) |
                    std::views::transform([](const auto& s){
                        return ValueOrPacket{std::stoul(s)};       
                    });
                std::copy(unsignedLongs.begin(), unsignedLongs.end(), std::back_inserter(subValues));
            }
            begin = nextBrace;
            if(*begin == '['){
                subValues.emplace_back(Packet(begin, end));

            }
            if(*begin == ']') {
                ++begin;
                lastBraceFound = true;
            }
        }

    }

    friend bool operator==(const Packet& packet1, const Packet& packet2) = default;

    friend std::istream& operator>>(std::istream& stream, Packet& packet){
        std::string packetText;
        getline(stream, packetText);
        auto start = packetText.begin();
        packet = Packet(start, packetText.end());
        return stream;

    }

    Result compare(const Packet& p2) const {
        auto iterator1 = subValues.begin();
        auto iterator2 = p2.subValues.begin();
        auto end1 = subValues.end();
        auto end2 = p2.subValues.end();
        while(iterator1 != end1 && iterator2 != end2){
            auto value1 = *iterator1;
            auto value2 = *iterator2;
            bool isInt1 = std::holds_alternative<unsigned long>(value1);
            bool isInt2 = std::holds_alternative<unsigned long>(value2);
            if(isInt1 && isInt2) {
                if(std::get<unsigned long>(value1) < std::get<unsigned long>(value2)){
                    return Result::LessThan;
                }
                if(std::get<unsigned long>(value1) > std::get<unsigned long>(value2)){
                    return Result::GreaterThan;
                }
            }
            else if(!isInt1 && !isInt2) {
                auto result = std::get<Packet>(value1).compare(std::get<Packet>(value2));
                switch(result){
                    case Result::KeepChecking: break;
                    default: return result;
                }
            }
            else if(isInt1 != isInt2) {
                if(isInt1) {
                    Packet packet1;
                    packet1.subValues.push_back(value1);
                    auto result = packet1.compare(std::get<Packet>(value2));
                    switch(result){
                        case Result::KeepChecking: break;
                        default: return result;
                    }
                }
                else {
                    Packet packet2;
                    packet2.subValues.push_back(value2);
                    auto result = std::get<Packet>(value1).compare(packet2);
                    switch(result){
                        case Result::KeepChecking: break;
                        default: return result;
                    }
                }
            }

            ++iterator1;
            ++iterator2;
        }

        if(iterator1 == end1 && iterator2 == end2) { 
            return Result::KeepChecking;
        }
        return (iterator1 == end1) ? Result::LessThan : Result::GreaterThan;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Packet& packet) {
        stream << "[";
        bool first = true;
        for(const auto& value: packet.subValues) {
            if(!first) {
                stream << ",";
            }
            first = false;
            if(std::holds_alternative<Packet>(value)) {
                stream << std::get<Packet>(value);
            }
            else {
                stream << std::get<unsigned long>(value);
            } 
        }
        stream << "]";
        return stream;
    }

private:
    using ValueOrPacket = std::variant<Packet, unsigned long>;
    std::vector<ValueOrPacket> subValues;
};

struct PacketPair {
    Packet p1;
    Packet p2;
    
    friend std::istream& operator>>(std::istream& stream, PacketPair& packetPair) {
        stream >> packetPair.p1;
        stream >> packetPair.p2;
        std::string empty;
        getline(stream, empty);
        return stream;
    }
};

unsigned int getSumOfOrderedPairIndices(std::span<PacketPair> packetPairs){
    unsigned int sum = 0;
    for(unsigned int i=0; i< packetPairs.size(); ++i) {
        if(packetPairs[i].p1.compare(packetPairs[i].p2) == Result::LessThan) {
            sum += (i+1);
        }
    }
    return sum;
}

unsigned int getDecoderIndicesProduct(std::span<PacketPair> packetPairs) {
    std::vector<Packet> packets;
    for(const auto& packetPair:  packetPairs){
        packets.push_back(packetPair.p1);
        packets.push_back(packetPair.p2);
    }
    std::string decoder1 = "[[2]]";
    std::string decoder2 = "[[6]]";

    auto start1 = decoder1.begin();
    Packet decoderPacket1(start1, decoder2.end());
    auto start2 = decoder2.begin();
    Packet decoderPacket2(start2, decoder2.end());

    packets.push_back(decoderPacket1);
    packets.push_back(decoderPacket2);

    std::sort(packets.begin(), packets.end(), [](const auto& p1, const auto& p2){
        return p1.compare(p2) == Result::LessThan;
    });

    return (1+std::distance(packets.begin(), std::find(packets.begin(), packets.end(), decoderPacket1))) * (1+std::distance(packets.begin(), std::find(packets.begin(), packets.end(), decoderPacket2)));
}

int main() {
    auto packetPairs = input::readLines<PacketPair>("input/input13.txt");
    std::cout << "Sum of ordered packet pairs: " << getSumOfOrderedPairIndices(packetPairs) << "\n";
    std::cout << "Decoder Indices Product: " << getDecoderIndicesProduct(packetPairs) << "\n";
    return 0;
}