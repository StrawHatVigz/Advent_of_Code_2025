#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

/*
 * Optimized for Synthesis/Hardware:
 * 1. Single Pass (O(N)).
 * 2. ZERO multiplications inside the loop.
 * 3. Uses simple integer comparisons (cheap comparators).
 * 4. Only one shift-add calculation at the very end.
 */
int getMaxJoltageHardwareFriendly(const std::string& line) {
    int n = line.length();
    
    // Registers to hold the best pair found so far
    int best_tens = -1;
    int best_ones = -1;
    
    // Register to hold the max digit seen in the current suffix
    int max_right = -1; 

    // Iterate Right-to-Left
    for (int i = n - 1; i >= 0; --i) {
        // Filter out non-digit characters (like newlines)
        if (line[i] < '0' || line[i] > '9') continue;
        
        int current_val = line[i] - '0';

        // We can only form a pair if we have seen at least one digit to the right
        if (max_right != -1) {
            // LOGIC GATE OPTIMIZATION:
            // Instead of multiplying, we simply check if the current digit
            // is effectively 'better' than the best 'tens' digit we've stored.
            // Since max_right is non-decreasing, 'current >= best_tens' is sufficient.
            if (current_val >= best_tens) {
                best_tens = current_val;
                best_ones = max_right;
            }
        }
        
        // Update max_right for the next clock cycle/iteration
        if (current_val > max_right) {
            max_right = current_val;
        }
    }

    if (best_tens != -1) {
        // Post-processing: Single calculation per bank.
        // In hardware, * 10 is just (x << 3) + (x << 1), a shift-add.
        return (best_tens * 10) + best_ones; 
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // Fast I/O
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    long long total_output_joltage = 0;

    while (std::getline(inputFile, line)) {
        if (!line.empty()) {
            total_output_joltage += getMaxJoltageHardwareFriendly(line);
        }
    }

    inputFile.close();
    std::cout << "Total Output Joltage: " << total_output_joltage << std::endl;

    return 0;
}
