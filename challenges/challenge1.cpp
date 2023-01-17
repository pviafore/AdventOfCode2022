#include <algorithm>
#include <iostream>
#include <string>

#include "input.h"

using namespace std::string_literals;
class Elf {
public:
	unsigned int getTotalCalories() const {
		return totalCalories;
	}

	friend std::istream& operator>>(std::istream& stream, Elf& elf) {
		while(stream.good()) {
			std::string calorieCount;
			std::getline(stream, calorieCount);
			if (calorieCount.empty()) {
				break; // hit a space
			}
			elf.totalCalories += std::stoi(calorieCount);
		}
		return stream;
	}

	friend auto operator<=>(const Elf& elf1, const Elf& elf2) = default;


private:
	unsigned int totalCalories = 0U;
};

int main() {
	auto elves = input::readLines<Elf>("input/input1.txt");

	std::ranges::sort(elves, std::ranges::greater());
	std::cout << "The elf with the most calories is: " << elves[0].getTotalCalories() << "\n";

	auto calorieTotal = elves[0].getTotalCalories() + elves[1].getTotalCalories() + elves[2].getTotalCalories();
	std::cout << "The three elves with the most calories is: " << calorieTotal << "\n";

	return 0;
}
