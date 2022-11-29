#include <cassert>
#include <iostream>
#include <istream>
#include <numeric>
#include <span>
#include <unordered_map>

#include "input.h"
enum class Move {
    Rock,
    Paper,
    Scissors
};

enum class Outcome {
    Loss,
    Draw,
    Win
};

const std::unordered_map<char, Move> moveMapping = {
    {'A', Move::Rock},
    {'B', Move::Paper},
    {'C', Move::Scissors},
    {'X', Move::Rock},
    {'Y', Move::Paper},
    {'Z', Move::Scissors}
};

const std::unordered_map<Move, unsigned int> scoreMapping = {
    {Move::Rock, 1U},
    {Move::Paper, 2U},
    {Move::Scissors, 3U}
};

const std::unordered_map<Outcome, unsigned int> outcomeMapping = {
    {Outcome::Loss, 0U},
    {Outcome::Draw, 3U},
    {Outcome::Win, 6U}
};

Outcome getOutcome(Move opponentMove, Move yourMove) {
    if (yourMove == opponentMove) {
        return Outcome::Draw;
    }
    if ((yourMove == Move::Rock && opponentMove == Move::Scissors) ||
        (yourMove == Move::Paper && opponentMove == Move::Rock) ||
        (yourMove == Move::Scissors && opponentMove == Move::Paper) ){
            return Outcome::Win;
    }
    return Outcome::Loss;

}

Move getYourMove(Move opponentMove, Outcome outcome) {
    if (outcome == Outcome::Draw){
        return opponentMove;
    }
    if (outcome == Outcome::Win) {
        switch (opponentMove)
        {
            case Move::Rock: return Move::Paper;
            case Move::Paper: return Move::Scissors;
            case Move::Scissors: return Move::Rock;
        }
    }
    switch (opponentMove)
    {
        case Move::Rock: return Move::Scissors;
        case Move::Paper: return Move::Rock;
        case Move::Scissors: return Move::Paper;
    }

    // Should not reach here (where's std::unreachable when I need it)
    assert(false);

    return Move::Rock;

}

class Game {
public:

    friend std::istream& operator>>(std::istream& stream, Game& game) {
        stream >> game.opponentChar >> game.yourChar;
        return stream;
    }

    unsigned int getScore() const {

        const Move opponentMove = moveMapping.at(opponentChar);
        const Move yourMove = moveMapping.at(yourChar);

        return scoreMapping.at(yourMove) + outcomeMapping.at(getOutcome(opponentMove, yourMove));
    }

    unsigned int getRevisedScore() const {
        std::unordered_map<char, Outcome> outcomeStringMapping = {
            {'X', Outcome::Loss},
            {'Y', Outcome::Draw},
            {'Z', Outcome::Win}
        };
        const Move opponentMove = moveMapping.at(opponentChar);
        const Outcome outcome = outcomeStringMapping.at(yourChar);
        const Move yourMove = getYourMove(opponentMove, outcome);

        return scoreMapping.at(yourMove) + outcomeMapping.at(outcome);
    }

private:
    char opponentChar = 'A';
    char yourChar = 'X';
};

unsigned int getTotalScore(std::span<Game> games) {
    return std::accumulate(games.begin(), games.end(), 0U, [](unsigned int sum, const Game& game){
        return sum + game.getScore();
    });
}

unsigned int getRevisedTotalScore(std::span<Game> games) {
    return std::accumulate(games.begin(), games.end(), 0U, [](unsigned int sum, const Game& game){
        return sum + game.getRevisedScore();
    });
}

int main() {
    auto games = input::readLines<Game>("input/input2.txt");
    std::cout << "Total score is " << getTotalScore(games) << "\n";
    std::cout << "Total revised score is " << getRevisedTotalScore(games) << "\n";
    return 0;
}