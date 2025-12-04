#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    // Check for correct usage
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    // Open the input file provided as a command line argument
    std::ifstream inputFile(argv[1]);
    
    // Check if file opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open " << argv[1] << std::endl;
        return 1;
    }

    int currentPos = 50; // Dial starts pointing at 50
    long long zeroCount = 0; // Counter for how many times it hits 0 (using long long just in case)
    
    char direction;
    int distance;

    // Read instructions one by one
    while (inputFile >> direction >> distance) {
        
        // --- PART 1 LOGIC (OLD CODE) ---
        // This logic only checked the position at the END of the rotation.
        /*
        if (direction == 'R') {
            currentPos = (currentPos + distance) % 100;
        } 
        else if (direction == 'L') {
            currentPos = (currentPos - distance) % 100;
            if (currentPos < 0) {
                currentPos += 100;
            }
        }

        // Only count if it stopped on 0
        if (currentPos == 0) {
            zeroCount++;
        }
        */

        // --- PART 2 LOGIC (NEW CODE) ---
        // We need to count every single time the dial touches 0 during the rotation.
        // We simulate the movement 1 click at a time.
        for (int i = 0; i < distance; ++i) {
            if (direction == 'R') {
                // Moving Right (increasing numbers)
                currentPos++;
                if (currentPos == 100) {
                    currentPos = 0; // Wrap around from 99 to 0
                }
            } 
            else if (direction == 'L') {
                // Moving Left (decreasing numbers)
                currentPos--;
                if (currentPos < 0) {
                    currentPos = 99; // Wrap around from 0 to 99
                }
            }

            // Check if this specific click made the dial point to 0
            if (currentPos == 0) {
                zeroCount++;
            }
        }
    }

    std::cout << "The new password (method 0x434C49434B) is: " << zeroCount << std::endl;

    inputFile.close();
    return 0;
}