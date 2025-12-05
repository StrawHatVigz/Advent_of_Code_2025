#include <fstream>
#include <iostream>
#include <string>

class Dial {
private:
    int position;

public:
    Dial()
    {
        position = 50;
    }

    bool rotate(std::string command)
    {
        bool direction;
        int steps;

        if (command[0] == 'L') {
            direction = false;
        } else if (command[0] == 'R') {
            direction = true;
        } else {
            std::cout << "Error: Invalid command direction" << std::endl;
            return false;
        }

        try {
            steps = std::stoi(command.substr(1));
        } catch (const std::invalid_argument& e) {
            std::cout << "Error: Invalid command steps" << std::endl;
            return false;
        }

        if (direction) {
            position += steps;
        } else {
            position -= steps;
        }

        // Insane wrap-around logic code from AI
        position = ((position % 100) + 100) % 100;

        return position == 0;
    }
};

int main()
{
    std::ifstream inputFile("inputs/input.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    Dial dial;
    int count = 0;

    std::string line;
    while (std::getline(inputFile, line)) {
        if (dial.rotate(line)) {
            count++;
        }
    }

    std::cout << count << std::endl;

    inputFile.close();
    return 0;
}
