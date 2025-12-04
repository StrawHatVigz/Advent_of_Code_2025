#include <iostream>
#include <fstream>
#include <string>

int main() {
    // Open the input file
    std::ifstream inputFile("input.txt");
    
    // Check if file opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input.txt" << std::endl;
        return 1;
    }

    int currentPos = 50; // Dial starts pointing at 50
    int zeroCount = 0;   // Counter for how many times it lands on 0
    
    char direction;
    int distance;

    // Read the file pair by pair. 
    // The extractor operator >> skips whitespace/newlines automatically.
    // It reads a char (L/R) then immediately reads the integer following it.
    while (inputFile >> direction >> distance) {
        if (direction == 'R') {
            // Rotate Right (towards higher numbers)
            currentPos = (currentPos + distance) % 100;
        } 
        else if (direction == 'L') {
            // Rotate Left (towards lower numbers)
            // We use % 100 first, but in C++ the result can be negative 
            // (e.g., -5 % 100 = -5), so we add 100 and mod again to ensure positive index.
            currentPos = (currentPos - distance) % 100;
            if (currentPos < 0) {
                currentPos += 100;
            }
        }

        // Check if the dial is pointing at 0 after the rotation
        if (currentPos == 0) {
            zeroCount++;
        }
    }

    std::cout << "The password is: " << zeroCount << std::endl;

    inputFile.close();
    return 0;
}