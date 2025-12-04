#include <iostream>
#include <fstream>
#include <string>

// Solution for Part 2 (Optimized): Count EVERY time dial passes 0.
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

    int currentPos = 50; 
    long long zeroCount = 0; 
    
    char direction;
    int distance;

    while (inputFile >> direction >> distance) {
        if (direction == 'R') {
            // MATH LOGIC FOR RIGHT ROTATION
            // Calculate how many times we wrap around 100
            zeroCount += (currentPos + distance) / 100 - currentPos / 100;
            
            // Update actual dial position
            currentPos = (currentPos + distance) % 100;
        } 
        else if (direction == 'L') {
            // MATH LOGIC FOR LEFT ROTATION
            int distToNextZero = (currentPos == 0) ? 100 : currentPos;

            if (distance >= distToNextZero) {
                // Hit the first zero, plus one for every 100 steps after
                zeroCount += 1 + (distance - distToNextZero) / 100;
            }

            // Update actual dial position
            currentPos = (currentPos - distance) % 100;
            if (currentPos < 0) currentPos += 100;
        }
    }

    std::cout << "Part 2 Password: " << zeroCount << std::endl;

    inputFile.close();
    return 0;
}