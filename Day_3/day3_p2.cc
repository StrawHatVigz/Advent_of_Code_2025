#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// Function to find the max 12-digit number from a bank using Greedy approach
unsigned long long getMaxTwelveDigitJoltage(const std::string& line) {
    int n = line.length();
    if (n < 12) return 0; // Error safety

    unsigned long long current_bank_val = 0;
    int last_picked_index = -1; // Position of the previously selected digit

    // We need to pick exactly 12 digits
    for (int k = 0; k < 12; ++k) {
        int digits_needed_after_this = 11 - k;
        
        // We can search from the char after the last one we picked...
        int search_start = last_picked_index + 1;
        
        // ...up to a point where we still have enough digits left for the rest.
        // E.g., if we need 11 more, we can't pick the very last digit now.
        int search_end = n - 1 - digits_needed_after_this;

        char max_digit_found = -1;
        int max_digit_index = -1;

        // Scan the valid window
        for (int i = search_start; i <= search_end; ++i) {
            char c = line[i];
            
            // Skip non-digits if dirty input
            if (c < '0' || c > '9') continue;

            if (c > max_digit_found) {
                max_digit_found = c;
                max_digit_index = i;
                
                // HARDWARE OPTIMIZATION: 
                // If we find a '9', we can't do better. 
                // Break early (Priority Encoder logic).
                if (c == '9') break; 
            }
        }

        // Accumulate the result
        // Effectively: current_bank_val * 10 + new_digit
        current_bank_val = (current_bank_val * 10) + (max_digit_found - '0');
        
        // Update position so next iteration searches AFTER this digit
        last_picked_index = max_digit_index;
    }

    return current_bank_val;
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
    unsigned long long total_output_joltage = 0;

    while (std::getline(inputFile, line)) {
        if (line.length() >= 12) {
            total_output_joltage += getMaxTwelveDigitJoltage(line);
        }
    }

    inputFile.close();
    std::cout << "Total Output Joltage: " << total_output_joltage << std::endl;

    return 0;
}
