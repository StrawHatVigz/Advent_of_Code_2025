#include <iostream>
#include <fstream>
#include <string>

// Solution for Part 1: Count how many times the dial STOPS on 0.
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open " << argv[1] << std::endl;
        return 1;
    }

    int currentPos = 50; // Dial starts pointing at 50
    int zeroCount = 0;   // Counter for Part 1
    
    char direction;
    int distance;

    while (inputFile >> direction >> distance) {
        if (direction == 'R') {
            // Rotate Right (towards higher numbers)
            currentPos = (currentPos + distance) % 100;
        } 
        else if (direction == 'L') {
            // Rotate Left (towards lower numbers)
            currentPos = (currentPos - distance) % 100;
            // Handle negative result from modulo operator in C++
            if (currentPos < 0) {
                currentPos += 100;
            }
        }

        // Part 1 Rule: Only count if it STOPS on 0
        if (currentPos == 0) {
            zeroCount++;
        }
    }

    std::cout << "Part 1 Password: " << zeroCount << std::endl;

    inputFile.close();
    return 0;
}