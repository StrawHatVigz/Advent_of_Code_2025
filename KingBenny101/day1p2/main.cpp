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

    int rotate(std::string command)
    {
        bool direction;
        int steps;

        if (command[0] == 'L') {
            direction = false;
        } else if (command[0] == 'R') {
            direction = true;
        } else {
            std::cout << "Error: Invalid command direction" << std::endl;
            return 0;
        }

        try {
            steps = std::stoi(command.substr(1));
        } catch (const std::invalid_argument& e) {
            std::cout << "Error: Invalid command steps" << std::endl;
            return 0;
        }

        int zeroCrossings = 0;
        int increment = direction ? 1 : -1;

        for (int i = 0; i < steps; i++) {
            position += increment;
            position = ((position % 100) + 100) % 100;

            if (position == 0) {
                zeroCrossings++;
            }
        }

        return zeroCrossings;
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
        count += dial.rotate(line);
    }

    std::cout << count << std::endl;

    inputFile.close();
    return 0;
}
